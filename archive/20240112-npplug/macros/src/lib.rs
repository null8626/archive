use proc_macro::TokenStream;
use proc_macro2::Span;
use std::sync::Mutex;
use syn::{parse_macro_input, FnArg, ItemFn, LitInt, ReturnType, Type};

mod codegen;

mod shortcut;
use shortcut::Shortcut;

mod util;
use util::MetaDict;

lazy_static::lazy_static! {
  static ref STATE: Mutex<codegen::State> = Default::default();
}

#[proc_macro_attribute]
pub fn function(attrs: TokenStream, item: TokenStream) -> TokenStream {
  let func = parse_macro_input!(item as ItemFn);
  let func_ident = &func.sig.ident;
  let mut state = STATE.lock().unwrap();

  util::syn_assert!(!state.main, func_ident, "a main function is already declared before this, please move this declaration before the main function");

  let attrs = util::syn_unwrap!(MetaDict::new(attrs));

  util::validate_common_fn_mistakes!(func);

  util::syn_assert!(
    func.sig.inputs.is_empty(),
    func_ident,
    "this function must not accept any arguments"
  );

  if let ReturnType::Type(_, ref __) = func.sig.output {
    util::syn_error!(func_ident, "this function must not return anything");
  }

  let func_name = format!("{func_ident}");

  util::syn_assert!(
    func_name.is_ascii(),
    func_ident,
    "this function name must be within the ASCII range"
  );

  let mut overridden_name: Option<String> = None;
  let mut shortcut: Option<Shortcut> = None;

  for (key, value) in attrs {
    match key.as_str() {
      "name" => {
        util::syn_assert!(
          value.is_ascii(),
          func_ident,
          "this function name must be within the ASCII range"
        );
        util::syn_assert!(
          value.len() <= 63,
          func_ident,
          "this function name must not be longer than 63 characters"
        );

        overridden_name.replace(value);
      }
      "shortcut" => shortcut = Shortcut::parse(value),
      _ => util::syn_error!(func_ident, format!("unrecognized key: '{key}'")),
    };
  }

  let func_name_limit = match overridden_name {
    Some(_) => 254,
    None => 63,
  };

  util::syn_assert!(
    func_name.len() <= func_name_limit,
    func_ident,
    format!("this function name must not be longer than {func_name_limit} characters")
  );

  match shortcut {
    Some(shortcut) => {
      util::syn_assert!(
        state.insert_function(func_name, overridden_name, shortcut),
        func_ident,
        "a function with the same name already exists"
      );

      let func_unsafety = func.sig.unsafety;
      let func_attrs = func.attrs;
      let func_body = func.block.as_ref();

      (quote::quote! {
        #(#func_attrs)*
        #[no_mangle]
        pub #func_unsafety extern "C" fn #func_ident() {
          #func_body
        }
      })
      .into()
    }

    None => util::syn_error!(func_ident, "missing/invalid keyboard shortcut"),
  }
}

#[rustfmt::skip]
#[proc_macro_attribute]
pub fn main(attrs: TokenStream, item: TokenStream) -> TokenStream {
  let func = parse_macro_input!(item as ItemFn);
  let attrs = util::syn_unwrap!(MetaDict::new(attrs));
  
  let func_ident = &func.sig.ident;
  let mut state = STATE.lock().unwrap();

  if state.main {
    util::syn_error!(func_ident, "a main lifecycle function already exists");
  } else {
    state.main = true;
  }
  
  util::validate_common_fn_mistakes!(func);
  
  let is_argument_valid = match func.sig.inputs.first() {
    Some(FnArg::Typed(p)) => {
      if let Type::Path(p) = p.ty.as_ref() {
        let ty = p.path.segments.first().unwrap().ident.to_string();
        
        ty.as_str() == "bool"
      } else {
        false
      }
    },
    
    _ => false
  };
  
  util::syn_assert!(is_argument_valid, func_ident, "this function must accept a bool argument");
  
  let mut name_constant: Option<proc_macro2::TokenStream> = None;
  let mut get_name = quote::quote! { npplug::__private::get_name() };
  let mut register_name = Some(quote::quote! {
    npplug::__private::register_name(env!("CARGO_PKG_NAME"));
  });
  let mut unregister_name = Some(quote::quote! {
    npplug::__private::unregister_name();
  });
  
  let mut run_main_body = quote::quote! { #func_ident(input) };
  
  for (key, value) in attrs {
    match key.as_str() {
      "name" => {
        let _ = register_name.take();
        let _ = unregister_name.take();
        
        let wide_name = value.encode_utf16().fold(Vec::with_capacity((value.len() / 2) + 1), |mut v, c| {
          if c == 0 {
            v.push(LitInt::new("65533", Span::call_site()));
          } else {
            v.push(LitInt::new(&c.to_string(), Span::call_site()));
          }
          
          v
        });
        let wide_name_len = LitInt::new(&wide_name.len().to_string(), Span::call_site());
        
        name_constant.replace(quote::quote! {
          const __NAME: [u16; #wide_name_len] = [#(#wide_name),*];
        });
        
        get_name = quote::quote! { __NAME.as_ptr() };
      },
      "asynchronous" => {
        run_main_body = quote::quote! {
          if input {
            #func_ident(input);
          } else {
            std::thread::spawn(move || #func_ident(input));
          }
        };
      },
      _ => util::syn_error!(func_ident, format!("unrecognized key: '{key}'")),
    };
  }
  
  let function_names = state.function_names();
  let functions = state.functions();
  let functions_len = LitInt::new(&functions.len().to_string(), Span::call_site());

  (quote::quote! {
    static mut __FUNCTIONS: [npplug::__private::__nppFuncItem; #functions_len] = [#(#functions),*];
    
    #func
    
    #[inline(always)]
    fn __run_main(input: bool) {
      #run_main_body
    }
    
    #name_constant
    
    #[no_mangle]
    pub extern "C" fn getName() -> *const u16 {
      #get_name
    }
    
    #[no_mangle]
    pub unsafe extern "system" fn DllMain(dll_module: isize, call_reason: u32, _: *mut u8) -> i32 {
      match call_reason {
        1 => {
          #register_name
          npplug::__private::register_functions(dll_module, #function_names.as_ptr(), &mut __FUNCTIONS as _, #functions_len);
          
          __run_main(false)
        },
        0 => {
          #unregister_name
          
          __run_main(true)
        },
        _ => {}
      };
    
      1
    }
  }).into()
}

use crate::Shortcut;
use proc_macro2::{Span, TokenStream};
use syn::{LitByteStr, LitInt};

struct Function {
  shortcut_idx: u32,
  name: String,
  overridden_name: Option<String>,
}

impl Function {
  fn name(&self) -> Vec<LitInt> {
    let mut output = self
      .overridden_name
      .as_ref()
      .unwrap_or(&self.name)
      .as_bytes()
      .iter()
      .fold(Vec::with_capacity(64), |mut c, x| {
        c.push(LitInt::new(&x.to_string(), Span::call_site()));
        c
      });

    output.extend(vec![LitInt::new("0", Span::call_site()); 64 - output.len()]);
    output
  }
}

impl PartialEq for Function {
  #[inline(always)]
  fn eq(&self, other: &Self) -> bool {
    self.name == other.name
  }
}

#[derive(Default)]
pub(crate) struct State {
  pub(crate) main: bool,
  shortcuts: Vec<Shortcut>,
  functions: Vec<Function>,
}

fn insert_return_index<T>(vec: &mut Vec<T>, value: T) -> (usize, bool)
where
  T: PartialEq<T>,
{
  for (i, v) in vec.iter().enumerate() {
    if v == &value {
      return (i, true);
    }
  }

  let out = vec.len();
  vec.push(value);

  (out, false)
}

impl State {
  pub(crate) fn insert_function(
    &mut self,
    name: String,
    overridden_name: Option<String>,
    shortcut: Shortcut,
  ) -> bool {
    let (name_index, already_exists) = insert_return_index(
      &mut self.functions,
      Function {
        shortcut_idx: 0,
        name,
        overridden_name,
      },
    );

    if already_exists {
      false
    } else {
      self.functions[name_index].shortcut_idx =
        insert_return_index(&mut self.shortcuts, shortcut).0 as _;

      true
    }
  }

  pub(crate) fn functions(&self) -> Vec<TokenStream> {
    self.functions.iter().enumerate().fold(
      Vec::with_capacity(self.functions.len()),
      |mut tokens, (id, func)| {
        let item_name = func.name();
        let id = LitInt::new(&id.to_string(), Span::call_site());

        tokens.push(quote::quote! {
          npplug::__private::__nppFuncItem {
            item_name: [#(#item_name),*],
            func: 0,
            cmd_id: #id,
            init_to_check: false,
            shortcut: 0,
          }
        });

        tokens
      },
    )
  }

  #[inline(always)]
  pub(crate) fn function_names(&self) -> LitByteStr {
    names_literal(self.functions.iter().map(|x| x.name.as_str()))
  }
}

pub(crate) fn names_literal<I>(names: I) -> LitByteStr
where
  I: Iterator,
  I::Item: AsRef<str>,
{
  let mut out = Vec::new();

  for name in names {
    let name = name.as_ref();

    out.reserve_exact(name.len() + 2);

    out.push(name.len() as _);
    out.extend_from_slice(name.as_bytes());
    out.push(0);
  }

  LitByteStr::new(&out, Span::call_site())
}

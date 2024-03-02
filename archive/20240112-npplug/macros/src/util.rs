use proc_macro::TokenStream;
use syn::{
  parse::Parser,
  punctuated::{self, Punctuated},
  Expr, ExprLit, Lit, MetaNameValue, Result, Token,
};

macro_rules! syn_error {
  ($token:expr,$message:expr) => {
    return syn::parse::Error::new(syn::spanned::Spanned::span(&$token), $message)
      .into_compile_error()
      .into()
  };
}

pub(crate) use syn_error;

macro_rules! syn_assert {
  ($e:expr,$token:expr,$message:expr) => {
    if !$e {
      $crate::util::syn_error!($token, $message)
    }
  };
}

pub(crate) use syn_assert;

macro_rules! syn_unwrap {
  ($value:expr) => {
    match $value {
      Ok(inner) => inner,
      Err(err) => {
        return err.into_compile_error().into();
      }
    }
  };
}

pub(crate) use syn_unwrap;

macro_rules! validate_common_fn_mistakes {
  ($func:ident) => {
    match $func.vis {
      syn::Visibility::Public(_) => {}
      _ => $crate::util::syn_error!($func.sig.ident, "this function must be public"),
    };

    $crate::util::syn_assert!(
      $func.sig.asyncness.is_none(),
      $func.sig.ident,
      "this function must not be asynchronous"
    );

    $crate::util::syn_assert!(
      $func.sig.constness.is_none(),
      $func.sig.ident,
      "this function must not be const"
    );
  };
}

pub(crate) use validate_common_fn_mistakes;

/// Parses #[key = "value", pairs = "and", stuff = "!!!"]
pub(crate) struct MetaDict {
  iter: punctuated::IntoIter<MetaNameValue>,
}

impl MetaDict {
  pub(crate) fn new(attrs: TokenStream) -> Result<Self> {
    Parser::parse(Punctuated::parse_terminated, attrs).map(
      |parsed_attrs: Punctuated<MetaNameValue, Token![,]>| Self {
        iter: parsed_attrs.into_iter(),
      },
    )
  }
}

impl Iterator for MetaDict {
  type Item = (String, String);

  fn next(&mut self) -> Option<Self::Item> {
    let next = self.iter.next()?;
    let key = format!("{}", next.path.get_ident()?);

    match next.value {
      Expr::Lit(ExprLit {
        lit: Lit::Str(v), ..
      }) => Some((key, v.value())),
      Expr::Lit(ExprLit {
        lit: Lit::Bool(b), ..
      }) => {
        if b.value {
          Some((key, String::new()))
        } else {
          None
        }
      }
      _ => None,
    }
  }
}

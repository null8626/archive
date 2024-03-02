use core::slice::Iter;

macro_rules! nonzero {
  ($value:expr) => {{
    let value = $value;

    if value != 0 {
      value
    } else {
      return Err($crate::NativeError::new());
    }
  }};
}

macro_rules! nonnull {
  ($value:expr) => {
    $crate::util::nonzero!(unsafe { core::mem::transmute::<_, usize>($value) });
  };
}

pub(crate) use nonnull;
pub(crate) use nonzero;

pub(crate) fn encode_utf16(s: &str) -> Vec<u16> {
  let mut result = Vec::with_capacity(s.len() / 2);

  for code in s.encode_utf16() {
    if code == 0 {
      result.push(0xFFFD);
    } else {
      result.push(code);
    }
  }

  result
}

#[inline(always)]
pub(crate) fn encode_utf16_null(s: &str) -> Vec<u16> {
  let mut result = encode_utf16(s);
  result.push(0);

  result
}

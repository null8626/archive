use core::slice;
use std::{
  error,
  fmt::{self, Debug, Display, Formatter},
};
use windows_sys::Win32::{
  Foundation::*,
  System::{Diagnostics::Debug::*, SystemServices::*},
};

const LANG_ID: u32 = (LANG_NEUTRAL << 10) | SUBLANG_DEFAULT;

#[derive(Debug)]
pub struct NativeError {
  code: WIN32_ERROR,
  message: String,
}

impl NativeError {
  pub(crate) fn new() -> Self {
    unsafe {
      let code = GetLastError();
      let buffer = 0 as *mut u16;

      let size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0 as _,
        code,
        LANG_ID,
        buffer,
        0,
        0 as _,
      );

      let message = String::from_utf16_lossy(slice::from_raw_parts(buffer, size as _));

      LocalFree(buffer as _);

      Self { code, message }
    }
  }

  pub const fn code(&self) -> WIN32_ERROR {
    self.code
  }
}

impl error::Error for NativeError {}

impl Display for NativeError {
  #[inline(always)]
  fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
    write!(f, "error code {}: {}", self.code, self.message)
  }
}

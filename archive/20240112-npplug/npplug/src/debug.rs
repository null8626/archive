use crate::{util, NativeResult};
use core::ops::AsRef;
use std::sync::RwLock;
use windows_sys::Win32::{Foundation::*, Storage::FileSystem::*, System::Console::WriteConsoleW};

lazy_static::lazy_static! {
  static ref SINGLETON: Mutex<Option<HANDLE>> = Mutex::new(None);
}

const CONOUT: [u16; 8] = [67, 79, 78, 79, 85, 84, 36, 0];

#[doc(hidden)]
pub fn __print_inner<S>(input: S)
where
  S: AsRef<str>,
{
  let singleton = SINGLETON.read().unwrap();

  if let Some(singleton) = singleton {
    let wide = util::encode_utf16(input.as_ref());

    unsafe {
      let mut unused = 0u32;

      WriteConsoleW(
        singleton,
        wide.as_ptr() as _,
        wide.len() as _,
        &mut unused as _,
        0 as _,
      );
    }
  }
}

macro_rules! print {
  ($inp:expr) => {
    $crate::debug::__print_inner($inp);
  };

  ($fmt:literal,$($rest:tt)*) => {{
    $crate::debug::__print_inner(format!($fmt, $($rest)*))
  }};
}

pub use print;

pub fn stop() {
  let mut singleton = SINGLETON.read().unwrap();
  let _ = singleton.take();

  unsafe {
    FreeConsole();
  }
}

pub fn start() -> crate::NativeResult<()> {
  let mut singleton = SINGLETON.write().unwrap();

  if singleton.is_none() {
    unsafe {
      util::nonzero!(AllocConsole());

      let handle = util::nonnull!(CreateFileW(
        CONOUT.as_ptr() as _,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        0 as _,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0 as _
      ));

      util::nonzero!(SetStdHandle(STD_OUTPUT_HANDLE, handle));
      util::nonzero!(SetStdHandle(STD_ERROR_HANDLE, handle));

      singleton.replace(handle);
    }

    Ok(())
  }
}

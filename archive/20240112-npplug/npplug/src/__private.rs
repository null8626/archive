use core::mem::{transmute, ManuallyDrop};
pub use lazy_static::lazy_static;
use windows_sys::Win32::{Foundation::*, System::LibraryLoader::*};

#[repr(C)]
pub struct __nppData {
  pub npp_handle: HWND,
  pub scintilla_main_handle: HWND,
  pub scintilla_second_handle: HWND,
}

#[repr(C)]
pub struct __nppShortcutKey {
  pub is_ctrl: bool,
  pub is_alt: bool,
  pub is_shift: bool,
  pub key: u8,
}

#[repr(C)]
pub struct __nppFuncItem {
  pub item_name: [u16; 64],
  pub func: usize,
  pub cmd_id: i32,
  pub init_to_check: bool,
  pub shortcut: usize,
}

static mut PLUGIN_NAME: Option<ManuallyDrop<Vec<u16>>> = None;

#[inline(always)]
pub fn get_name() -> *const u16 {
  unsafe {
    if let Some(ref inner) = PLUGIN_NAME {
      inner.as_ptr()
    } else {
      unreachable!()
    }
  }
}

pub unsafe fn register_name(name: &'static str) {
  let mut vec = name.encode_utf16().fold(
    ManuallyDrop::new(Vec::with_capacity((name.len() / 2) + 1)),
    |mut v, c| {
      if c == 0 {
        v.push(0xFFFD);
      } else {
        v.push(c);
      }

      v
    },
  );

  vec.push(0);

  PLUGIN_NAME.replace(vec);
}

#[inline(always)]
pub unsafe fn unregister_name() {
  if let Some(ref mut inner) = PLUGIN_NAME {
    ManuallyDrop::drop(inner);
  }
}

pub unsafe fn register_functions(
  module: isize,
  mut names: *const u8,
  funcs: *mut __nppFuncItem,
  funcs_size: u32,
) {
  for i in 0..funcs_size {
    let jump_size = *names;
    let func = funcs.offset(i as _);

    (*func).func = transmute(GetProcAddress(module, names.offset(1)));

    names = names.offset(jump_size as _);
  }
}

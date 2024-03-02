use core::mem::transmute;
use std::hash::{Hash, Hasher};

#[derive(Copy, Clone)]
pub(crate) struct Shortcut {
  pub(crate) is_ctrl: bool,
  pub(crate) is_alt: bool,
  pub(crate) is_shift: bool,
  pub(crate) key: u8,
}

impl Shortcut {
  pub(crate) fn parse(input: String) -> Option<Self> {
    let mut out = Self {
      is_ctrl: false,
      is_alt: false,
      is_shift: false,
      key: 0,
    };

    for chunk in input.split('+') {
      let chunk = chunk.to_ascii_lowercase();

      // kill me.
      match chunk.as_str() {
        "ctrl" | "control" => out.is_ctrl = true,
        "alt" => out.is_alt = true,
        "shift" => out.is_shift = true,
        "click" => out.key = 0x01,
        "rightclick" | "right-click" => out.key = 0x02,
        "backspace" => out.key = 0x08,
        "tab" => out.key = 0x09,
        "clear" => out.key = 0x0C,
        "enter" => out.key = 0x0D,
        "capslock" | "caps-lock" => out.key = 0x14,
        "space" | " " => out.key = 0x20,
        "left" => out.key = 0x25,
        "up" => out.key = 0x26,
        "right" => out.key = 0x27,
        "down" => out.key = 0x28,
        "del" => out.key = 0x2E,
        "0" => out.key = 0x30,
        "1" => out.key = 0x31,
        "2" => out.key = 0x32,
        "3" => out.key = 0x33,
        "4" => out.key = 0x34,
        "5" => out.key = 0x35,
        "6" => out.key = 0x36,
        "7" => out.key = 0x37,
        "8" => out.key = 0x38,
        "9" => out.key = 0x39,
        "a" => out.key = 0x41,
        "b" => out.key = 0x42,
        "c" => out.key = 0x43,
        "d" => out.key = 0x44,
        "e" => out.key = 0x45,
        "f" => out.key = 0x46,
        "g" => out.key = 0x47,
        "h" => out.key = 0x48,
        "i" => out.key = 0x49,
        "j" => out.key = 0x4A,
        "k" => out.key = 0x4B,
        "l" => out.key = 0x4C,
        "m" => out.key = 0x4D,
        "n" => out.key = 0x4E,
        "o" => out.key = 0x4F,
        "p" => out.key = 0x50,
        "q" => out.key = 0x51,
        "r" => out.key = 0x52,
        "s" => out.key = 0x53,
        "t" => out.key = 0x54,
        "u" => out.key = 0x55,
        "v" => out.key = 0x56,
        "w" => out.key = 0x57,
        "x" => out.key = 0x58,
        "y" => out.key = 0x59,
        "z" => out.key = 0x5A,
        "win" => out.key = 0x5B,
        "f1" => out.key = 0x70,
        "f2" => out.key = 0x71,
        "f3" => out.key = 0x72,
        "f4" => out.key = 0x73,
        "f5" => out.key = 0x74,
        "f6" => out.key = 0x75,
        "f7" => out.key = 0x76,
        "f8" => out.key = 0x77,
        "f9" => out.key = 0x78,
        "f10" => out.key = 0x79,
        "f11" => out.key = 0x7A,
        "f12" => out.key = 0x7B,
        "plus" => out.key = 0xBB,
        "," => out.key = 0xBC,
        "-" => out.key = 0xBD,
        "." => out.key = 0xBE,
        _ => {}
      };
    }

    if out.key != 0 {
      Some(out)
    } else {
      None
    }
  }
}

impl Hash for Shortcut {
  #[inline(always)]
  fn hash<H: Hasher>(&self, state: &mut H) {
    let value = unsafe { transmute::<_, u32>(*self) };

    value.hash(state);
  }
}

impl PartialEq for Shortcut {
  #[inline(always)]
  fn eq(&self, other: &Self) -> bool {
    unsafe { transmute::<_, u32>(*self) == transmute::<_, u32>(*other) }
  }
}

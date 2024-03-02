#[cfg(feature = "debug")]
pub mod error;
#[cfg(feature = "debug")]
pub(crate) mod util;
#[cfg(feature = "debug")]
pub use error::NativeError;

#[cfg(feature = "debug")]
pub mod debug;

#[cfg(feature = "debug")]
pub type NativeResult<T> = core::result::Result<T, NativeError>;

#[doc(hidden)]
pub mod __private;

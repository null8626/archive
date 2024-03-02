@echo off
setlocal

if "%1" == "vc-clang" goto clang
if "%1" == "clang" goto clang
if "%1" == "gcc" goto mingw

echo using MSVC to compile...

set CC=cl.exe

set CFLAGS=/MD /O2 /nologo /DSUS_TCC_LIB_FILE="libtcc1.a"
set LFLAGS=/MACHINE:%VSCMD_ARG_HOST_ARCH% /SUBSYSTEM:CONSOLE

if not exist libtcc.lib call build-libtcc-msvc.bat

:msvc

for /f %%I in ('dir /a:-d-h /b %cd%\src\*.c') do (
  %CC% -c %CFLAGS% src/%%~nxI
)

rc.exe /nologo /n /fo sus.res sus.rc
%CC% %CFLAGS% /Fe"sus.exe" *.obj sus.res libtcc.lib /link %LFLAGS%
del *.obj

goto end

:mingw

echo using GCC to compile...

if not exist libtcc.a call build-libtcc.bat gcc.exe

set CC=gcc.exe
set CFLAGS=-Wall -DSUS_TCC_LIB_FILE="libtcc1.a"
set OBJS=

for /f %%I in ('dir /a:-d-h /b %cd%\src\*.c') do (
  %CC% -c %CFLAGS% %cd%\src\%%~nxI -o %cd%\src\%%~nxI.o
)

for /f %%F in ('dir /b /a:-d-h "%cd%\src\*.o"') do call set OBJS=%%OBJS%% "%cd%\src\%%F"

%CC% %CFLAGS% %OBJS% -L%cd% -ltcc -o sus

goto end

:clang

echo using Clang to compile...

if not exist libtcc.lib call %cd%\build-libtcc.bat clang.exe

set CC=clang.exe
set CFLAGS=-Wall -D_CRT_SECURE_NO_WARNINGS -DSUS_TCC_LIB_FILE="libtcc1.a"
set OBJS=

for /f %%I in ('dir /a:-d-h /b %cd%\src\*.c') do (
  %CC% -c %CFLAGS% %cd%\src\%%~nxI -o %cd%\src\%%~nxI.o
)

for /f %%F in ('dir /b /a:-d-h "%cd%\src\*.o"') do call set OBJS=%%OBJS%% "%cd%\src\%%F"

llvm-rc.exe /n /fo sus.res sus.rc
%CC% %CFLAGS% %OBJS% sus.res -L%cd% -llibtcc -o sus.exe

:end
endlocal
@echo off
setlocal

echo #define TCC_VERSION "0.9.27" 1>%cd%\deps\tcc\config.h

set CC=%1
if "%1" == "" set CC=gcc.exe

:cmp

set TGT=-DTCC_TARGET_X86_64
set ATGT=-DTCC_TARGET_I386
set IDK=i386-win32

if "%PROCESSOR_ARCHITECTURE%" == "x86" set TGT=-DTCC_TARGET_I386
if "%PROCESSOR_ARCHITECTURE%" == "x86" set ATGT=-DTCC_TARGET_X86_64
if "%PROCESSOR_ARCHITECTURE%" == "x86" set IDK=x86_64-win32

if "%1" == "clang.exe" goto cmp_clang

gcc.exe -c %cd%\deps\tcc\libtcc.c -DTCC_TARGET_PE %TGT% -O3 -o %cd%\libtcc.o
ar.exe rcs -o %cd%\libtcc.a %cd%\libtcc.o
gcc.exe %cd%\deps\tcc\tcc.c -L%cd% -ltcc -DTCC_TARGET_PE %TGT% -DONE_SOURCE"=0" -O3 -o %cd%\tcc.exe

goto cmp_tcc1

:cmp_clang

clang.exe -c %cd%\deps\tcc\libtcc.c -D_CRT_SECURE_NO_WARNINGS -DTCC_TARGET_PE %TGT% -O3 -o %cd%\libtcc.o
llvm-lib.exe %cd%\libtcc.o
clang.exe %cd%\deps\tcc\tcc.c -D_CRT_SECURE_NO_WARNINGS -L%cd% -llibtcc -DTCC_TARGET_PE %TGT% -DONE_SOURCE"=0" -O3 -o %cd%\tcc.exe

:cmp_tcc1

set TCCFLAGS=-I%cd%\deps\tcc\win32\include -I%cd%\deps\tcc\win32\include\winapi -I%cd%\deps\tcc\include

if "%PROCESSOR_ARCHITECTURE%" == "x86" goto cmp_32

%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\lib\libtcc1.c
%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\win32\lib\crt1.c
%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\win32\lib\crt1w.c
%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\win32\lib\wincrt1.c
%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\win32\lib\wincrt1w.c
%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\win32\lib\dllcrt1.c
%cd%\tcc.exe %TCCFLAGS% -m64 -c %cd%\deps\tcc\win32\lib\dllmain.c
%cd%\tcc.exe -m64 -c %cd%\deps\tcc\win32\lib\chkstk.S
%cd%\tcc.exe %TCCFLAGS% -m64 -w -c %cd%\deps\tcc\lib\bcheck.c
%cd%\tcc.exe -m64 -c %cd%\deps\tcc\lib\alloca86_64.S
%cd%\tcc.exe -m64 -c %cd%\deps\tcc\lib\alloca86_64-bt.S
%cd%\tcc.exe -m64 -ar %cd%\libtcc1.a libtcc1.o crt1.o crt1w.o wincrt1.o wincrt1w.o dllcrt1.o dllmain.o chkstk.o bcheck.o alloca86_64.o alloca86_64-bt.o

goto end

:cmp_32

%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\lib\libtcc1.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\crt1.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\crt1w.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\wincrt1.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\wincrt1w.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\dllcrt1.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\dllmain.c
%cd%\tcc.exe %TCCFLAGS% -m32 -c %cd%\deps\tcc\win32\lib\chkstk.S
%cd%\tcc.exe %TCCFLAGS% -m32 -w -c %cd%\deps\tcc\lib\bcheck.c
%cd%\tcc.exe -m32 -c %cd%\deps\tcc\lib\alloca86.S
%cd%\tcc.exe -m32 -c %cd%\deps\tcc\lib\alloca86-bt.S
%cd%\tcc.exe -m32 -ar %cd%\libtcc1.a libtcc1.o crt1.o crt1w.o wincrt1.o wincrt1w.o dllcrt1.o dllmain.o chkstk.o bcheck.o alloca86.o alloca86-bt.o

:end
del /s *.exe *.o

endlocal
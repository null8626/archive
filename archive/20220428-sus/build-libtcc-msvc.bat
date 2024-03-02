@echo off
setlocal

echo #define TCC_VERSION "0.9.27" 1>%cd%\deps\tcc\config.h

set TGT=-DTCC_TARGET_I386
set ATGT=-DTCC_TARGET_X86_64
set IDK=x86_64-win32

if "%VSCMD_ARG_TGT_ARCH%" == "x64" set TGT=-DTCC_TARGET_X86_64
if "%VSCMD_ARG_TGT_ARCH%" == "x64" set ATGT=-DTCC_TARGET_I386
if "%VSCMD_ARG_TGT_ARCH%" == "x64" set IDK=i386-win32

cl.exe -Fe%cd%\libtcc.dll -LD %cd%\deps\tcc\libtcc.c -DTCC_TARGET_PE %TGT% -DLIBTCC_AS_DLL -O2 -W2 -Zi -MT -GS- -nologo -link -opt:ref,icf
cl.exe -Fe%cd%\tcc.exe %cd%\deps\tcc\tcc.c %cd%\libtcc.lib -DTCC_TARGET_PE %TGT% -DONE_SOURCE"=0" -O2 -W2 -Zi -MT -GS- -nologo -link -opt:ref,icf
cl.exe -Fe%cd%\%IDK%-tcc.exe %cd%\deps\tcc\tcc.c -DTCC_TARGET_PE %ATGT% -O2 -W2 -Zi -MT -GS- -nologo -link -opt:ref,icf

set TCCFLAGS=-I%cd%\deps\tcc\win32\include -I%cd%\deps\tcc\win32\include\winapi -I%cd%\deps\tcc\include

if "%VSCMD_ARG_TGT_ARCH%" == "x64" goto cmp_64

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

goto end

:cmp_64

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

:end
del /s *.exe *.obj *.pdb *.o *.exp

endlocal

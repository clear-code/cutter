@echo off

set BASE_DIR=%~p0
set TOP_DIR=%BASE_DIR%..

cd %TOP_DIR%
nmake -nologo -f Makefile.win32

set PATH=%PATH%;%TOP_DIR%\vcc\vendor\bin;%TOP_DIR%\cppcutter;%TOP_DIR%\test\lib

set CUTTER=%TOP_DIR%\cutter\cutter.exe

set CUT_UI_MODULE_DIR=%TOP_DIR%\module\ui
set CUT_UI_FACTORY_MODULE_DIR=%TOP_DIR%\module\ui
set CUT_REPORT_MODULE_DIR=%TOP_DIR%\module\report
set CUT_REPORT_FACTORY_MODULE_DIR=%TOP_DIR%\module\report
set CUT_STREAM_MODULE_DIR=%TOP_DIR%\module\stream
set CUT_STREAM_FACTORY_MODULE_DIR=%TOP_DIR%\module\stream

@echo on

%CUTTER% -s %BASE_DIR% --exclude-directory fixtures --exclude-directory lib %BASE_DIR% %1 %2 %3 %4 %5 %6 %7 %8 %9

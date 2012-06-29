
set ROOT_DIR=%CD%
echo %ROOT_DIR%

set MCPP_DIR=%ROOT_DIR%\build\mcpp
mkdir %MCPP_DIR%
cd %MCPP_DIR%
xcopy /Y /E %ROOT_DIR%\mcpp-2.7.2\* %MCPP_DIR%
cd %MCPP_DIR%/src

patch -p0 < ..\noconfig\vc2008.dif
nmake MCPP_LIB=1 /f ..\noconfig\visualc.mak mcpplib
cd %ROOT_DIR%
mkdir %MCPP_DIR%\lib
copy /Y %MCPP_DIR%\src\*.lib %MCPP_DIR%\lib

REM exit 0

set ICEE_DIR=%ROOT_DIR%\build\icee
mkdir %ICEE_DIR%
cd %ICEE_DIR%
xcopy /Y /E %ROOT_DIR%\IceE-1.3.0\* %ICEE_DIR%

REM Build slice2cppe
cd %ICEE_DIR%\cpp\
nmake /f Makefile.mak

REM Build IceE
cd %ICEE_DIR%
nmake DEBUG=0 /f Makefile.mak configure
nmake DEBUG=0 /f Makefile.mak

cd %ROOT_DIR%
rem DONE!!!

@echo off

REM 64-bit build
REM .\ci.bat 64

REM 32-bit build
REM .\ci.bat 32

REM Look for VS 2019
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\" (
  call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars%1.bat"
) else (
  if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\" (
    call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars%1.bat"
  ) else (
    echo Could not find Visual Studio 2019
    exit /b 1
  )
)

REM Where pip installs exes
set Path=%Path%;%AppData%\Python\Python37\Scripts

REM rmake builds and runs unit tests
REM TODO: eventually the products will be passed to a CD job to be wrapped up in an MSI.
call rmake Debug
call rmake Release

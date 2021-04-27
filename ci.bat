echo off

REM Look for VS 2019
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\" (
  call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
) else (
  if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\" (
    call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
  ) else (
    echo Could not find Visual Studio 2019
    exit /b 1
  )
)

REM Where pip installs exes
set Path=%Path%;%AppData%\Python\Python37\Scripts

call rmake Debug
call rmake Release

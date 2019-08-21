@echo off

REM rmake [build_type [git_branch [build_shared_libs]]]
REM default: rmake Debug master False
REM examples:
REM rmake
REM rmake Debug
REM rmake Release my-branch
REM rmake Debug master True
REM
REM Builds are done in build\<build_type> (for x86) or build\<build_type>x64 (for x64)
REM e.g. build\Debugx64, build\Release. Target architecture is determined by environment.

REM ----- Argument parsing -----

REM BUILD_TYPE (1st arg): Debug or Release. Default is Debug.
set BUILD_TYPE=%1
if "%BUILD_TYPE%" == "" set BUILD_TYPE=Debug

REM GIT_BRANCH (2nd arg): Any branch from the github repo to use with conan create.
REM Default is master.
set GIT_BRANCH=%2
if "%GIT_BRANCH%" == "" set GIT_BRANCH=master
REM This will be validated by the conan create step at the end, in case the branch doesn't exist.

REM BUILD_SHARED_LIBS (3rd arg): True or False. Determines whether to build DLLs instead of
REM static libs (all deps, including this SDK). Default is False.
set BUILD_SHARED_LIBS=%3
if "%BUILD_SHARED_LIBS%" == "" set BUILD_SHARED_LIBS=False

REM ----- Argument validation ------

if NOT "%BUILD_TYPE%" == "Debug" IF NOT "%BUILD_TYPE%" == "Release" (
  echo Invalid argument "%BUILD_TYPE%" for BUILD_TYPE (1st arg^). Must be Debug or Release.
  exit /b 1
)

if NOT "%BUILD_SHARED_LIBS%" == "True" IF NOT "%BUILD_SHARED_LIBS%" == "False" (
  echo Invalid argument "%BUILD_SHARED_LIBS%" for BUILD_SHARED_LIBS (3rd arg^). Must be True or False.
  exit /b 1
)

REM ----- End argument parsing and validation -----

REM Determine TARGET_ARCH from environment.
REM Note conan requires x86 or x86_64 for this parameter. TARGET_ARCH is passed to conan -s arch.
REM cmake requires Win32 or x64. CMAKE_TARGET_ARCH is passed to cmake ..\.. -A arch.
REM %VSCMD_ARG_TGT_ARCH% == x86 or x64.
if "%VSCMD_ARG_TGT_ARCH%" == "x64" (
  set TARGET_ARCH=x86_64
) else (
  set TARGET_ARCH=%VSCMD_ARG_TGT_ARCH%
)

if "%TARGET_ARCH%" == "" (
  echo "TARGET_ARCH not set. Please use a Native Tools Command Prompt."
  exit /b 1
)

echo rmake.bat configuration:
echo  BUILD_TYPE        %BUILD_TYPE%
echo  TARGET_ARCH       %TARGET_ARCH%
echo  BUILD_SHARED_LIBS %BUILD_SHARED_LIBS%
echo  GIT_BRANCH        %GIT_BRANCH%
echo.

REM Determine some derived params for conan, cmake, etc.
REM Pass -A <arch> to cmake generator to be consistent with Conan
if %TARGET_ARCH% == x86_64 (
  set CMAKE_TARGET_ARCH=x64
) else (
  set CMAKE_TARGET_ARCH=Win32
)

REM Set VC runtime arg for conan, cmake
if %BUILD_TYPE% == Debug (
  set RUNTIME=MDd
) else (
  set RUNTIME=MD
)

REM Determine the subdirectory of build to use
if %TARGET_ARCH% == x86 (
  set BUILD_SUBDIR=%BUILD_TYPE%
) else (
  set BUILD_SUBDIR=%BUILD_TYPE%x64
)

REM ----- End configuration -----

REM Silence errors from mkdir with 2>NUL (Windows for /dev/null)
mkdir build 2> NUL
cd build

mkdir %BUILD_SUBDIR% 2> NUL
cd %BUILD_SUBDIR%

REM Build/install dependencies needed for this SDK
conan install ..\..\conanfile.py^
  -s build_type=%BUILD_TYPE%^
  -s arch=%TARGET_ARCH%^
  -o *:shared=%BUILD_SHARED_LIBS%^
  --build outdated

if ERRORLEVEL 1 (
  echo "conan install failed"
  cd ..\..
  exit /b -1
)

cmake ..\.. -G "Visual Studio 16 2019" -A %CMAKE_TARGET_ARCH% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DSKIP_INSTALL_ALL=true -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%

if ERRORLEVEL 1 (
  echo "cmake configure failed"
  cd ..\..
  exit /b -1
)

REM 8 threads available for parallel builds
cmake --build . -- /m:8

if ERRORLEVEL 1 (
  echo "cmake build failed"
  cd ..\..
  exit /b -1
)

ctest -C %BUILD_TYPE%

if ERRORLEVEL 1 (
  echo "unit tests failed"
  cd ..\..
  exit /b -1
)

cd ..\..

REM Install into the Conan cache
conan create . branch/testing^
  -s build_type=%BUILD_TYPE%^
  -s arch=%TARGET_ARCH%^
  -o *:shared=%BUILD_SHARED_LIBS%^
  -o BranchIO:git_branch=%GIT_BRANCH%^
  --build outdated

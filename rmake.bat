@echo off

REM rmake [BUILD_TYPE [CONAN_PROFILE [BUILD_SHARED_LIBS]]]
REM default: rmake Debug default False
REM examples:
REM rmake
REM rmake Debug
REM rmake Release
REM rmake Debug my-profile
REM rmake Release my-profile True
REM
REM Builds are done in build\<build_type> (for x86) or build\<build_type>x64 (for x64)
REM e.g. build\Debugx64, build\Release. Target architecture is determined by environment.

REM ----- Argument parsing -----

REM BUILD_TYPE (1st arg): Debug or Release. Default is Debug.
set BUILD_TYPE=%1
if "%BUILD_TYPE%" == "" set BUILD_TYPE=Debug

REM CONAN_PROFILE (2nd arg): Any valid conan profile. Default is Debug.
set CONAN_PROFILE=%2
if "%CONAN_PROFILE%" == "" set CONAN_PROFILE=default

REM BUILD_SHARED_LIBS (3rd arg): True or False. Determines whether to build DLLs instead of
REM static libs (all deps, including this SDK). Default is False.
set BUILD_SHARED_LIBS=%3
if "%BUILD_SHARED_LIBS%" == "" set BUILD_SHARED_LIBS=False

REM ----- Argument validation ------

if NOT "%BUILD_TYPE%" == "Debug" IF NOT "%BUILD_TYPE%" == "Release" (
  echo Invalid argument "%BUILD_TYPE%" for BUILD_TYPE (1st arg^). Must be Debug or Release.
  exit /b 1
)

SET profile_match=No
FOR /F %%p IN ('conan profile list') DO IF %%p == %CONAN_PROFILE% SET profile_match=Yes
if "%profile_match%" == "No" (
  echo Invalid conan profile "%CONAN_PROFILE%" (2nd arg^). Please use one of the following:
  for /f %%p in ('conan profile list') do @echo %%p
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
echo  CONAN_PROFILE     %CONAN_PROFILE%
echo  BUILD_SHARED_LIBS %BUILD_SHARED_LIBS%
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

if %BUILD_TYPE% == Debug (
  set RUNTIME=MDd
) else (
  set RUNTIME=MD
)

REM The Branch SDK only requires:
REM   PocoFoundation
REM   PocoJSON
REM   PocoNetSSLWin
REM These in turn require:
REM   PocoNet
REM   PocoUtil
REM   PocoXML
REM All are included by default. Disable all other optional
REM libraries that are enabled by default (all of the following).
REM
REM See https://github.com/conan-io/conan-center-index/blob/master/recipes/poco/all/conanfile.py#L31

set Poco_Options=^
  --options Poco:enable_crypto=False^
  --options Poco:enable_data=False^
  --options Poco:enable_data_mysql=False^
  --options Poco:enable_data_odbc=False^
  --options Poco:enable_data_postgresql=False^
  --options Poco:enable_data_sqlite=False^
  --options Poco:enable_encodings=False^
  --options Poco:enable_jwt=False^
  --options Poco:enable_mongodb=False^
  --options Poco:enable_netssl=False^
  --options Poco:enable_redis=False^
  --options Poco:enable_zip=False

REM Build/install dependencies needed for this SDK
conan install ..\..\conanfile.py^
  --settings build_type=%BUILD_TYPE%^
  --settings arch=%TARGET_ARCH%^
  --settings compiler.runtime=%RUNTIME%^
  --options *:shared=%BUILD_SHARED_LIBS%^
  %Poco_Options%^
  --build outdated^
  --update

if ERRORLEVEL 1 (
  echo conan install failed.
  cd ..\..
  exit /b -1
)

cmake ..\.. -G "Visual Studio 16 2019" -A %CMAKE_TARGET_ARCH% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DSKIP_INSTALL_ALL=true -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%

if ERRORLEVEL 1 (
  echo cmake configure failed.
  cd ..\..
  exit /b -1
)

REM 8 threads available for parallel builds
cmake --build . -- /m:8

if ERRORLEVEL 1 (
  echo cmake build failed.
  cd ..\..
  exit /b -1
)

ctest -C %BUILD_TYPE%

if ERRORLEVEL 1 (
  echo Unit tests failed.
  cd ..\..
  exit /b -1
)

REM For now, don't run this second build in CI. The last steps are preparation for release to conan.
REM TODO: Refactor the batch files in this repo.
if "%GITHUB_ACTION%" == "true" (
  exit /b 0
)

REM Install into the Conan cache
conan create ..\.. branch/testing^
  --json conan-install.json^
  --settings build_type=%BUILD_TYPE%^
  --settings arch=%TARGET_ARCH%^
  --settings compiler.runtime=%RUNTIME%^
  --options *:shared=%BUILD_SHARED_LIBS%^
  %Poco_Options%^
  --build outdated

echo Building stage from conan cache
python ..\..\BranchSDK\tools\stage.py

if ERRORLEVEL 1 (
  echo stage.py failed.
  cd ..\..
  exit /b -1
)

cd ..\..

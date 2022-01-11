@ECHO OFF

REM rmake [RUNTIME [ARCHITECTURE [CONAN_PROFILE]]]
REM default: rmake MDd x64 default

REM ----- Argument parsing -----

REM RUNTIME: MD, MDd, MT or MTd
SET RUNTIME=%1
IF "%RUNTIME%" == "" SET RUNTIME=MDd

REM ARCHITECTURE: x86 or x64
SET ARCHITECTURE=%2
IF "%ARCHITECTURE%" == "" SET ARCHITECTURE=x64

REM CONAN_PROFILE: Any valid conan profile.
SET CONAN_PROFILE=%3
IF "%CONAN_PROFILE%" == "" SET CONAN_PROFILE=default

REM ----- Argument validation ------

IF NOT "%RUNTIME%" == "MD" IF NOT "%RUNTIME%" == "MDd" IF NOT "%RUNTIME%" == "MT" IF NOT "%RUNTIME%" == "MTd" (
  ECHO Invalid argument "%RUNTIME%" for RUNTIME. MD, MDd, MT or MTd.
  EXIT /b 1
)

IF NOT "%ARCHITECTURE%" == "x86" IF NOT "%ARCHITECTURE%" == "x64" (
  ECHO Invalid argument "%ARCHITECTURE%" for ARCHITECTURE. x86 or x64.
  EXIT /b 1
)

SET profile_match=No
FOR /F %%p IN ('conan profile list') DO IF %%p == %CONAN_PROFILE% SET profile_match=Yes
IF "%profile_match%" == "No" (
  ECHO Invalid conan profile "%CONAN_PROFILE%" (2nd arg^). Please use one of the following:
  FOR /f %%p IN ('conan profile list') DO @ECHO %%p
  EXIT /b 1
)

IF NOT "%VSCMD_ARG_TGT_ARCH%" == "x64" IF NOT "%VSCMD_ARG_TGT_ARCH%" == "x86" (
  ECHO "Please use a Native Tools Command Prompt."
  EXIT /b 1
)

REM ----- Configuration -----

REM conan requires x86 or x86_64 passed via "-s arch"
REM cmake requires Win32 or x64 passed via "-A arch"
IF "%ARCHITECTURE%" == "x64" (
  SET CONAN_TARGET_ARCH=x86_64
  SET CMAKE_TARGET_ARCH=x64
  SET BUILD_SUBDIR=%RUNTIME%_64
) ELSE IF "%ARCHITECTURE%" == "x86" (
  SET CONAN_TARGET_ARCH=x86
  SET CMAKE_TARGET_ARCH=Win32
  SET BUILD_SUBDIR=%RUNTIME%
)

REM set BUILD_TYPE based on runtime option
SET BUILD_TYPE=Debug
IF NOT "%RUNTIME%" == "MDd" IF NOT "%RUNTIME%" == "MTd" (
  SET BUILD_TYPE=Release
)

REM build static libraries by default
REM TODO: support building dlls when this is set to True
set BUILD_SHARED_LIBS=False

ECHO rmake.bat configuration:
ECHO RUNTIME           %RUNTIME%
ECHO ARCHITECTURE      %ARCHITECTURE%
ECHO CONAN_PROFILE     %CONAN_PROFILE%
ECHO CONAN_TARGET_ARCH %CONAN_TARGET_ARCH%
ECHO CMAKE_TARGET_ARCH %CMAKE_TARGET_ARCH%
ECHO BUILD_SUBDIR      %BUILD_SUBDIR%
ECHO BUILD_TYPE        %BUILD_TYPE%
ECHO BUILD_SHARED_LIBS %BUILD_SHARED_LIBS%
ECHO.

REM ----- Build -----

REM Silence errors from mkdir with 2>NUL (Windows for /dev/null)
mkdir build 2> NUL
cd build

mkdir %BUILD_SUBDIR% 2> NUL
cd %BUILD_SUBDIR%

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

SET Poco_Options=^
  --options Poco:enable_crypto=False^
  --options Poco:enable_netssl=False^
  --options Poco:enable_netssl_win=True^
  --options Poco:enable_active_record=False^
  --options Poco:enable_data=False^
  --options Poco:enable_data_mysql=False^
  --options Poco:enable_data_odbc=False^
  --options Poco:enable_data_postgresql=False^
  --options Poco:enable_data_sqlite=False^
  --options Poco:enable_encodings=False^
  --options Poco:enable_jwt=False^
  --options Poco:enable_mongodb=False^
  --options Poco:enable_redis=False^
  --options Poco:enable_zip=False

ECHO Install SDK dependencies with conan
conan install ..\..\conanfile.py^
  --settings build_type=%BUILD_TYPE%^
  --settings arch=%CONAN_TARGET_ARCH%^
  --settings compiler.runtime=%RUNTIME%^
  --options *:shared=%BUILD_SHARED_LIBS%^
  %Poco_Options%^
  --build outdated^
  --update

IF ERRORLEVEL 1 (
  ECHO conan install failed.
  cd ..\..
  EXIT /b -1
)

ECHO Setup Branch SDK with cmake and Visual Studio
cmake ..\.. -G "Visual Studio 16 2019"^
  -A %CMAKE_TARGET_ARCH%^
  -DCMAKE_BUILD_TYPE=%BUILD_TYPE%^
  -DRUNTIME=%RUNTIME%^
  -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%

IF ERRORLEVEL 1 (
  ECHO cmake configure failed.
  cd ..\..
  EXIT /b -1
)

REM 8 threads available for parallel builds
cmake --build . -- /m:8

IF ERRORLEVEL 1 (
  ECHO cmake build failed.
  cd ..\..
  exit /b -1
)

ctest -C %BUILD_TYPE%

IF ERRORLEVEL 1 (
  ECHO Unit tests failed.
  cd ..\..
  EXIT /b -1
)

REM For now, don't run this second build in CI. The last steps are preparation for release to conan.
REM TODO: Refactor the batch files in this repo.
IF "%GITHUB_ACTIONS%" == "true" (
  cd ..\..
  EXIT /b 0
)

REM Install into the conan cache
conan create ..\.. branch/testing^
  --json conan-install.json^
  --settings build_type=%BUILD_TYPE%^
  --settings arch=%CONAN_TARGET_ARCH%^
  --settings compiler.runtime=%RUNTIME%^
  --options *:shared=%BUILD_SHARED_LIBS%^
  %Poco_Options%^
  --build outdated

cd ..\..

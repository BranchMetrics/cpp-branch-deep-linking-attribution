# Branch Metrics C++ SDK

[Platform prerequisites]: #platform-prerequisites
[Install CMake and Conan]: #install-cmake-and-conan
[Installation with Conan]: #installation-with-conan
[Integration]: #integration
[Sample apps]: #sample-apps

## Contents
- [Platform prerequisites]
- [Install CMake and Conan]
- [Installation with Conan]
- [Integration]
- [Sample apps]

## Platform prerequisites

### Windows

- Visual Studio 2019
- Python/Pip (can be installed through Visual Studio or otherwise)
- (Optional): Conan Visual Studio Extension
- Ensure that python, pip and the pip installation bin folder are in your PATH.

### macOS

```bash
xcode-select --install # XCode command-line tools required
brew install python    # Python and pip required
```

### Ubuntu

```bash
sudo apt-get install -y g++
sudo apt-get install -y python
sudo apt-get install -y python-pip
```

## Install CMake and Conan

```
pip install cmake
pip install conan
```

Note that in some cases it may be necessary to run `pip install --user cmake`,
etc. It may also be necessary to use sudo.

Note that cmake may be installed in different ways. This SDK requires version
3.12 or later. On Ubuntu, `apt-get install cmake` will only provide 3.10, so
`pip install cmake` is required.

## Installation with Conan

**(Pending release of the SDK. This step will not be needed once the Conan
recipe is published.)**

Use `conan create` to install into the Conan cache, e.g.
`conan create . branch/testing`. This will be correctly done for you using the
`rmake` scripts in the repo.

Windows:
```
rmake
rmake Debug
rmake Release
```
Debug is the default. Note that you must use an x86 or x64 Native Tools Command
Prompt for this purpose. Choose the environment corresponding to the
architecture you wish to build (32 or 64 bits).

Unix:
```bash
./rmake
./rmake Debug
./rmake Release
```
Release is the default.

Note that on Unix doxygen is also required for rmake to finish cleanly. It will,
however, build and install the SDK in the Conan cache before failing to
generate the documentation. To install doxygen:

macOS:
```bash
brew install doxygen
```

Ubuntu:
```
sudo apt-get install doxygen
```

Doxygen is only required when working with the repo. It is not required for
apps using the SDK.

## Integration

Add the following to your conanfile.txt:

```
[requires]
BranchIO/0.0.1@branch/testing
```

And then `conan install`.

### Visual Studio

The [Conan Visual Studio Extension](https://marketplace.visualstudio.com/items?itemName=conan-io.conan-vs-extension) automatically integrates the dependencies from your conanfile.txt into a Visual Studio project. Version 1.2.1 or later is required.

## Sample apps

Running `rmake` on any platform builds a number of sample apps. Sample apps are
built in `build/Debug|Release/bin` (Unix) or `build\Debug|Release[x64]\bin`.
For example:

Unix:
```bash
./rmake Debug
build/Debug/bin/hello
```

Windows (64-bit):
```
rmake Debug
build\Debugx64\bin\hello
```

Windows (32-bit):
```
rmake Debug
build\Debug\bin\hello
```

### Hello app

The `hello` app sends a number of requests and logs the output to the console.

### Example app

The `example` app is menu driven and allows you to try a number of scenarios.
It also prints output to the console.

Note that in Release builds, debug and verbose output log messages are compiled
out. If you run, e.g.:

```bash
./rmake Release
build/Release/bin/hello
```

you will not see the same output from the SDK as a Debug build.

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

## Set up branch remote

_This step will no longer be necessary once the package is contributed to
conan-center._

Add the Branch Conan repository. You can call it anything at all. Here it is
`branch`:

```
conan remote add branch https://api.bintray.com/conan/branchsdk/cpp-branch-deep-linking-attribution
```

## Integration

Add the following to your conanfile.txt:

```
[requires]
BranchIO/0.1.0@branch/testing
```

And then `conan install . --build outdated`.

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

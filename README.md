# Branch Metrics C++ SDK

[![License](https://img.shields.io/badge/license-MIT-green.svg?style=flat)](https://github.com/BranchMetrics/cpp-branch-deep-linking-attribution/blob/master/LICENSE)
[![CircleCI](https://img.shields.io/circleci/project/github/BranchMetrics/cpp-branch-deep-linking-attribution.svg)](https://circleci.com/gh/BranchMetrics/cpp-branch-deep-linking-attribution)

[Platform prerequisites]: #platform-prerequisites
[Install CMake and Conan]: #install-cmake-and-conan
[Set up Branch remote]: #set-up-branch-remote
[Set up a default conan profile]: #set-up-a-default-conan-profile
[Integration]: #integration
[Sample apps]: #sample-apps

## Contents
- [Platform prerequisites]
- [Install CMake and Conan]
- [Set up a default Conan profile]
- [Set up Branch remote]
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

## Set up a default conan profile

Before installing software with Conan, you have to create a default profile.
Run the following command once to create a profile called default.

```
conan profile new default --detect
```

See https://docs.conan.io for detailed information on using Conan.

## Set up Branch remote

_This step will no longer be necessary once the package is contributed to
conan-center._

Add the Branch Conan repository. You can call it anything at all. Here and below it is
called `branch`:

```
conan remote add branch https://api.bintray.com/conan/branchsdk/cpp-branch-deep-linking-attribution
```

## Integration

Add a file called conanfile.txt to your project root with the following
contents:

```
[requires]
BranchIO/0.2.0@branch/alpha

[options]
Poco:enable_mongodb=False
Poco:enable_data_sqlite=False
```

And then `conan install . --build outdated -s build_type=CONFIGURATION`,
where `CONFIGURATION` is Debug or Release. When building on Windows at the
command line, also pass `-s arch=ARCHITECTURE`, where `ARCHITECTURE` is
x86 or x86_64.

See the note below on using the Branch remote to download
prebuilt binary packages for Windows. If you don't specify `-r branch` to
`conan install` to install from the Branch repository, dependencies will be
found in conan-center without prebuilt binaries, and you'll be building from
source. OpenSSL requires the assembler. Use a Native Tools Command Prompt
when building OpenSSL from source.

**Note:** Once ready for release, the Conan channel will change from `branch/testing` to
`branch/stable`.

### Visual Studio

The [Conan Visual Studio Extension](https://marketplace.visualstudio.com/items?itemName=conan-io.conan-vs-extension) automatically integrates the dependencies from your conanfile.txt into a Visual Studio project. Version 1.2.1 or later is required.

See [BranchDemo](./BranchSDK-Samples/Windows/BranchDemo) for a working example
and more information on using the Conan Visual Studio extension.

**Note:**

Binary builds of the packages this SDK depends on are available from the Branch
Conan repository, but not from conan-center. If you use the Visual Studio
extension out of the box, it will always initially build OpenSSL, Poco and
gtest for any configuration that is not yet in the cache. To speed up this
process considerably, first put the following two-line conanfile.txt anywhere at all on
your Windows 10 host (not necessarily your project):

```
[requires]
BranchIO/0.2.0@branch/alpha

[options]
Poco:enable_mongodb=False
Poco:enable_data_sqlite=False
```

After you [add the Branch remote](#set-up-branch-remote) and [set up a default profile](#set-up-a-default-conan-profile), just run:

```
conan install . -r branch --build BranchIO -s build_type=CONFIGURATION -s arch=ARCHITECTURE
```

from a Developer Command Prompt (not necessarily a Native Tools prompt) from the
directory containing conanfile.txt. CONFIGURATION is Debug or Release.
ARCHITECTURE is x86 or x86_64.

This will download all dependencies
prebuilt. The BranchIO package does not yet have binary builds available.
That will always be built locally from source and cached by Conan.

Building the dependencies takes time, though it is a one-time cost. After you
build locally, they will be cached. Manually running `conan install -r branch`
is much faster. This situation will doubtless evolve.

## Sample apps

Running `rmake` in this repo on any platform builds a number of sample apps. Sample apps are
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

Use the appropriate Native Tools Command Prompt. This script will use
conan-center for dependencies.

The hello and example command-line apps require a specific Branch key in
order to run successfully. Provide it as a command-line argument when running
these programs. The key is `key_live_bcMneVUDd5sM5vD4BzfcbikdwrmnRDA8`, e.g.

```bash
build/Debug/bin/example -k key_live_bcMneVUDd5sM5vD4BzfcbikdwrmnRDA8
```

```bash
build/Debug/bin/hello key_live_bcMneVUDd5sM5vD4BzfcbikdwrmnRDA8
```

Note the `-k` option to example.

### Hello app

The `hello` app sends a number of requests and logs the output to the console.

### Example app

The `example` app is menu driven and allows you to try a number of scenarios.
It also prints output to the console.

Note that in Release builds, debug and verbose output log messages are compiled
out. If you run, e.g.:

```bash
./rmake Release
build/Release/bin/hello key_live_bcMneVUDd5sM5vD4BzfcbikdwrmnRDA8
```

you will not see the same output from the SDK as a Debug build.

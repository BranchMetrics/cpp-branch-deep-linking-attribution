# BranchInstaller

## Prerequisites

The [WiX Toolset build tools](https://wixtoolset.org/releases)
The [WiX extension for Visual Studio](https://marketplace.visualstudio.com/items?itemName=WixToolset.WixToolsetVisualStudio2019Extension)

## Build first with rmake

It is necessary to build both Debug and Release for x86 and x64. To build
for each platform, use the appropriate Native Tools Command Prompt (x86 or x64).
In each window, run:

```
rmake Debug
rmake Release
```

This generates a `stage` folder under each build type (build\Debugx64,
build\Release, etc.). This is the content that will be packaged.

## Build from BranchInstaller.sln

Select Release (probably doesn't matter, since all libraries are included) and
Build Solution (F7). This generates the MSI at bin/Release/BranchInstaller.msi.

Double-clicking the MSI installs the Branch C++ SDK for Windows. There is no
prompt beyond the system prompting for permission to update.

## Building against the installed SDK

See [BranchDemo](../../../BranchSDK-Samples/Windows/BranchDemo) for an
example of a project building against the installed MSI.

The SDK will be installed in C:\Program Files (x86)\Branch SDK.

Add C:\Program Files (x86)\Branch SDK\include to Additional Include Directories
for all configurations and platforms.

Define `POCO_STATIC=ON` and `POCO_NO_AUTOMATIC_LIBS` in Preprocessor Defines.
Also define `DEBUG` in Debug builds.

Add the library directory for each combination of configuration and platform to
Additional Library Search Paths:
- Debug x64:
  C:\Program Files (x86)\Branch SDK\lib\x64\Debug
- Release x64:
  C:\Program Files (x86)\Branch SDK\lib\x64\Release
- Debug x86:
  C:\Program Files (x86)\Branch SDK\lib\x86\Debug
- Release x86:
  C:\Program Files (x86)\Branch SDK\lib\x86\Release

Add the following (in order) to Additional Dependencies, depending on the
configuration:

Debug:
```
BranchIO.lib
advapi32.lib
PocoNetmd.lib
PocoNetSSLWinmdd.lib
PocoCryptomdd.lib
PocoDatamdd.lib
PocoZipmdd.lib
PocoUtilmdd.lib
PocoXMLmdd.lib
PocoJSONmdd.lib
PocoFoundationmdd.lib
Iphlpapi.lib
Crypt32.lib
ssleay32.lib
libeay32.lib
crypt32.lib
msi.lib
ws2_32.lib
zlib.lib
```

Release:
```
BranchIO.lib
advapi32.lib
PocoNetmd.lib
PocoNetSSLWinmd.lib
PocoCryptomd.lib
PocoDatamd.lib
PocoZipmd.lib
PocoUtilmd.lib
PocoXMLmd.lib
PocoJSONmd.lib
PocoFoundationmd.lib
Iphlpapi.lib
Crypt32.lib
ssleay32.lib
libeay32.lib
crypt32.lib
msi.lib
ws2_32.lib
zlib.lib
```

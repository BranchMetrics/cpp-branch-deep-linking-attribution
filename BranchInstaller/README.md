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

Select Release and Build Solution (F7). This generates the MSI at bin/Release/BranchSDK.msi.

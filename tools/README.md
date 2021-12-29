## Check out the Branch Win32 SDK and the Poco library side by side.

[https://github.com/BranchMetrics/cpp-branch-deep-linking-attribution](https://github.com/BranchMetrics/cpp-branch-deep-linking-attribution)

[https://github.com/pocoproject/poco](https://github.com/pocoproject/poco)

### Tool versions
* python - 3.9.6
* cmake - 3.22.1
* conan - 1.43.2
* doxygen - 1.9.2
* Visual Studio - 16.11.7

### Build Branch SDK
* Change into the cpp-branch-deep-linking-attribution directory.
* `python tools\build.py` 

##### Workaround release binary naming issue
* Rename the `build\MD_64\BranchSDK\lib\BranchIO.lib` to `build\MD_64\BranchSDK\lib\BranchIOmd.lib`
* Rename the `build\MT_64\BranchSDK\lib\BranchIO.lib` to `build\MT_64\BranchSDK\lib\BranchIOmt.lib`

### Stage build products
* `python tools\stage.py`

##### Workaround invalid Poco binaries built by conan
* `python tools\build_poco.py`
* Copy the Poco MD libraries from `poco\cmake-build\lib\Release` to `stage\lib\MD_64`
* Copy the Poco MT libraries from `poco\cmake-build\lib\Release` to `stage\lib\MT_64`
* Copy the Poco MDd libraries from `poco\cmake-build\lib\Debug` to `stage\lib\MDd_64`
* Copy the Poco MTd libraries from `poco\cmake-build\lib\Debug` to `stage\lib\MTd_64`

Now the library binaries should be in `stage`

### Build WIX Installer (Optional)
* `python tools\wix.py`
* Open `stage\BranchInstaller.sln` and build BranchInstaller
* MSI will be in `stage\bin\Debug\`

# Windows C++ 2.0


### **Build Process**

**Supported OS :** Windows 10+

**Visual Studio Version** : 2019 +

1. Install **Visual Studio ( 2019 +):**

Microsoft Visual Studio 2019/2022 - Professional Edition

[https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/)

- From within the Visual Studio Installer, install the **Universal Windows Platform development** workload.
- In Installation Details -> Universal Windows Platform development, check the C++ (v14x) Universal Windows Platform tools option(s)

2. Install **C++/WinRT Visual Studio Extension (VSIX)**:

Download and install the latest version of the [C++/WinRT Visual Studio Extension (VSIX)](https://marketplace.visualstudio.com/items?itemName=CppWinRTTeam.cppwinrt101804264) from the Visual Studio Marketplace.

3. **Microsoft.Windows.CppWinRT NuGet package** :

Latest version of the VSIX extension automatically installs the [Microsoft.Windows.CppWinRT NuGet package](https://www.nuget.org/packages/Microsoft.Windows.CppWinRT/). If its not installed automatically , install it manually. Click **Project** > **Manage NuGet Packages...** > search **Microsoft.Windows.CppWinRT** and then **Install.**


4. **Set Project Properties :** C++/WinRT uses features from the C++17 standard, so check if appropriate version is set in project properties - 
      In Visual Studio, open **C/C++** > **Language** > **C++ Language Standard** > **ISO C++17 Standard (/std:c++17)**


## Command Line **Build Procedure**

**Note :** Build from **Visual Studio Developer Command Prompt / Developer Powershell.** Visual Studio 2019+ comes with integrated terminal. Open it using menu ***View->Terminal.***

**Build Environment Setup**

- Visual Studio - 2022/2019
- python - Install from Microsoft store.
- Install cmake and conan -

    *`> pip install --user cmake conan`*

- Setup default conan profile if not done earlier.

    *`> conan profile new default —detect`*

- Install doxygen using brew or you can use installer as well.

    *`> brew install doxygen`*

**Build**

For build use build.py script file inside the tools folder in the repo. Note: For build, you should be inside **tools** folder. If you try to run it from other location, it will not find helper scripts and fail.

    > python .\build.py

Build will create final **msi** inside the **stage\bin\Debug** folder.

# Win32 TestBed Sample App

## Prerequisites

- Windows 10 (version 1809 or greater)
- Visual Studio 2019 with C++ and UWP workflows (the latter may be necessary for MSIX)
- [Conan Visual Studio Extension](https://marketplace.visualstudio.com/items?itemName=conan-io.conan-vs-extension)

## Build and install

1. Open `TestBed.sln` with Visual Studio.
2. Install dependencies with Conan: `Tools > Conan Package Management > Install (Entire Solution)`. It may be
  necessary to do this more than once.
3. Deploy the solution to the Start menu: `Build > Deploy Solution`

## Run the app

You can run the app in the debugger in Visual Studio. You can also launch it from
the Start menu or via a link on the win32.app.link domain, e.g. https://win32.app.link/ZIZc1yp3F9.
You can generate links via the Get Short URL option in the app that will open
the app.

## Logs

SDK logs appear in `%LocalAppData%\Branch\TestBed\branch-sdk.log`. The `LocalAppData` environment variable usually expands to `%HomeDrive%%HomePath%\AppData\Local`. Note that `AppData` is a hidden folder. In Git Bash, the easiest way to view the log is:

`$ tail -f ~/AppData/Local/Branch/TestBed/branch-sdk.log`

Note that the logs may roll over when they exceed a certain size.

## Uninstall

You can uninstall the app via the "Add or remove programs" option
in Windows settings.

Native C++ ColorPicker Sample
===================================

Branch Sample written for Windows that demonstrates

* Branch SDK Integration
* Branch Initialization
* Responding to a deep link event

Pre-requisites
--------------

- Visual Studio

Getting Started
---------------

This sample uses Visual Studio. 

To build this project, use the "Open Project" in Visual Studio and select the solution.


Using the ColorPicker App
-------------------------

### Basic Usage

Running the application out of the box will demonstrate Branch initialization.   
Debug Output will show in the debugger when Branch responds with the **Open** request.

The window background is a nice shade of blue by default.

Further, a Color Dialog will show when the application is first opened allowing you to
pick a different background color.  Selecting a new color will send an event to Branch
inidicating "TUTORIAL_COMPLETE".

### Advanced Usage

Clicking on a branch link prior to opening the application can show what happens when the application
responds to the **Open** request and the data that comes from Branch has the __extra_color__ value.
If the sample detects the __extra_color__ in the response, it will parse the color and change the
background on start.


Here are some sample links to click prior to opening the application.

* [Teal](https://uxzce.app.link/K77X6pN1A0)
* [Olive Drab](https://uxzce.app.link/LbwQ9Dq2A0)
* [Blue Bell](https://uxzce.app.link/L9dmODrcKZ)

Selecting a link and then opening the application afterwards will demonstrate Branch's ability to 
attribute the click with the application opening.

License
-------

The MIT License (MIT)

Copyright (c) 2019 Branch Metrics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
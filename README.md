![alt text](https://github.com/The-ivyleague-sloth/RaceDetectionTool/blob/master/pictures/cutout.jpg)

![alt text](https://github.com/The-ivyleague-sloth/RaceDetectionTool/blob/master/pictures/usain.png)

# RaceDetectionTool Overview
A Tool that utilizes the Intel Pin framework to dynamically test race conditions in an executable.
For more information on the Intel Pin framework please consult
https://software.intel.com/sites/landingpage/pintool/docs/81205/Pin/html/index.html

# Installation
download and unpack the intel Pin Framework for your OS (Link Below)
https://software.intel.com/en-us/articles/pin-a-binary-instrumentation-tool-downloads

Installation for Intel Pin can be found in link below
https://software.intel.com/sites/landingpage/pintool/docs/81205/Pin/html/index.html#INSTALLATION

Our version is developed on the Pin 3.7version on a 64 bit Linux OS.

# Build 
* Navigate to the unpacked pin tools directory (mine is "pin-3.7-97619/source/tools")
* clone the repo into this directory
* in this same tools directory modify the makefile and add the new folder into the section ALL_TEST_DIRS (should append RaceDetectionTool)
* save. Pin uses a CMake style makefile structure so it will build every test in this directory if make command is executed inside the tool directory.
* cd into RaceDetectionTool and type make to make the .so and .o files for the Race Detection Tool
* If no failures The tool is set to use

# Run
* in the RaceDetection Directory type in something like
* ../../../pin -t obj-intel64/MyPinTool.so -- ../../../../race_tests/race

this command line does the following:
* ../../../pin --> executes the intel pin tool
* -t obj-intel64/MyPinTool.so --> -t option tells the pin executable which tool .so file to use. In this case it is our RaceDetection tool called MyPinTool. 
* -- ../../../../race_tests/race This tells what program we are doing our dynamic analysis on

#Timing

time ./naiverace
time valgrind --tool=helgrind race_tests/naiverace
time ../../../pin -t obj-intel64/MyPinTool.so -o 
out -- race_tests/naiverace




# Talos-CNC
Complete CNC Control (Thats the goal)

Somehow the repository got corrupted and would not upload my changes so I had to delete and re-upload the repo. 

# Refactoring is mostly complete.
# There are quite a few that have cloned the repo. If you would care to help with some portions of it, I would greatly appreciate it. 

This has been a 2 year project. This is only one third of the entire project. This code is written in C++11 using Atmel Studio version 7.

The purpose of this project is to make an industrial type control for the open build community. Similar to grbl, but with full support 
of all turning,milling,plasma gcode support. Think LinuxCNC in an embedded system. 

This code was not developed with a particularr embedded system in mind, but initial development occured using Atmel 2560 controllers
on the Arduino 2560 development board. This code is completley hardware independent, and will even run on a PC.

It is currently compliant with 99% of the gcode standard, including tool offsets, cutter radius compensation, all feedrate modes, etc.. 

# I will be updating this often, and if anyone would like to lend a hand it would be greatly appreciated. 

I have recently made chagnes to this project to allow it to run in Microsoft Visual Studio 2017. It is much easier to debug in that environment. If you want a preconfigured project for Visual Studio I have uploaded it into the test app folder. 

Atmel 328P update. Sadly the code has grown to a point that it will not fit on the 328P anymore. Optimization may help, but its 200% beyond the memory of a 328P. 

If you are looking for thr old 'grblRTC' repository I have deleted it because all of the RTC closed loop feedback is now incorporated into this project. 

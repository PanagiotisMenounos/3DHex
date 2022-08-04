# 3DHex

3DHex is a windows host controller 3D printer firmware.
Αll kinematics calculations are executed by the computer while the MCU accepts raw 0s & 1s. 

## Downloads
								
Still in development, first BETA release soon

## Features

* SD/USB Printing
* Compiled setup installation 
* Direct configuration from the UI 
* Up to 25 configurations   
* S-Curve analytic mathematic solver for G0/G1 & G2/G3
* S-Curve = 0: Trapezoidal analytic mathematic solver for G0/G1 & G2/G3
* Optimized arc motion algorithm for G2/G3: microstepping grid resolution  
* Smart curve detection post processing algorithm, integrating multiple movements into one velocity profile if the direction change is below threshold angle  
* XY-XZ-YZ Skew correction      
* PID/Bang-Bang temperature control
* AutoPID tuning
* Auto Bed Leveling plane interpollation
* Auto Bed Leveling mesh interpollation algorithms: Multiquadric, Inverse, Gaussian, Linear, Cubic, Quintic, Thin plate
* Auto Bed Leveling plane/mesh view
* USB idle functions: Temperature control, Fan control, Move toolhead, Enable/Disable stepper, Homing, AutoPID  
* USB on the fly functions: Temperature control, Fan control, Pause, Cancel, (Jump/Jerk/Acceleration/Feedrate)% control  
* Supported G/M commands: G0, G1, G2, G3, G4, G28, G90, G91, G92, M104, M109, M140, M190, M106, M107, M17, M18, M84, M204, M205, M82,M83, M226
* Basic thermal protection

## Supported hardware
* Arduino Mega 2560 - RAMPS board 
* Ramps 2004 LCD Smart Controller
* Cartesian 3D printers only

## Pending

* 32bit boards   
* CoreXY and delta printers   
* Update manuals  

## User interface
<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Icons/GUI_Main.PNG" />
<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Icons/GUI_Heaters.PNG" />
<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Icons/GUI_Adv.PNG" />
<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Icons/GUI_ABL.PNG" />
<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Icons/GUI_Mesh.PNG" />

## Theory 

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/3D%20Printer.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/Host.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/MCU.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/S-Curve.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/Jerk.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/Curve%20detection.JPG" />
# 3DHex

3DHex is a windows host controller 3D printer firmware. 
The Host runs on windows and uses the CPU power of any typical desktop computer to solve really complex mathematical equations for the kinematics. In it's core function the MCU sets it's outputs according to a "move" byte which contains the step(4bits) & direction(4bits) signal for XYZE axes and one more "delay" byte corresponding to the steprate at that time. When printing from USB the host is calculating real time and sends the raw binary data via USB. Unlike typical 3D Printer firmwares, the MCU cannot accept GCODE and in order to print from SD Card the Host saves the generated data to a .bin file which then can be transferred to the MCU with SD card.

## User interface
<img align="center" src="https://github.com/3DHexfw/3DHex/blob/master/Host/GUI.png" />

## Specs

-Easy configuration from the UI
-Up to 10 different printer configurations
-Trapezoidal / S-Curve motion planner supported for all kind of motions: G0/G1 & G2/G3
-Optimized arc motion algorithm for G2/G3: microstepping grid resolution
-Smart curve detection algorithm: No decelaration will occur when the direction change is small for the next move. Both linear G0/G1 & G2/G3 are considered
-Basic thermal protection
-Software axis alignment 
-Support only for Arduino Mega 2560 - RAMPS board
-LCD Display 16x2 Module HD44780 only
-Cartesian 3D printers only
-PID/Bang-Bang temperature control
-USB idle functions: Temperature control, Fan control, Move toolhead, Homing, AutoPID
-USB on the fly functions: Temperature control, Fan control, Pause, Cancel, (Jump/Jerk/Acceleration/Feedrate)% control
-Supported G/M commands: G0, G1, G2, G3, G4, G28, G90, G91, G92, M104, M109, M140, M190, M106, M107, M17, M18, M84, M204, M205, M82,M83, M226

## Pending

-AutoPID tuning
-32bit boards
-G29 Autobed leveling
-CoreXY and delta printers
-Realtime kinematics plots
-Update manuals

## Download compiled

Please note: DO NOT LEAVE YOUR PRINTER UNATTENDED!!! ...compiled arem not final releases but just for quick review with major bugs and pending features.

[3DHex_setup.exe(32bit)](https://www.youtube.com/channel/UCmxyTgfH-faXP00cXr8jxtA?view_as=subscriber)
[3DHex_setup.exe(64bit)](https://www.youtube.com/channel/UCmxyTgfH-faXP00cXr8jxtA?view_as=subscriber)

## Manual

Take a look at the (old) [Guide](https://github.com/3DHexfw/3DHex/blob/master/Host/Host%20saved%20files/3DHex/Guide_1.0.2.pdf)

## Presentation 

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/3D%20Printer.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/Host.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/MCU.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/S-Curve.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/Jerk.JPG" />

<img align="center" src="https://github.com/3DHexfw/3DHex/blob/develop/Docs/Presentation/Curve%20detection.JPG" />
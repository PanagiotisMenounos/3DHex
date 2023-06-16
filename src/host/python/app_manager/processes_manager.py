#*******************EXTERNAL IMPORTS************************
import shutil
import sys
import serial.tools.list_ports
from PyQt5 import QtWidgets
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from PyQt5 import QtGui
from PyQt5.QtGui import *
import time
import os
import easygui
import subprocess
import struct
import win32pipe, win32file
import numpy as np
from matplotlib import pyplot
import matplotlib.pyplot as plt
from scipy.interpolate import Rbf
from numpy import loadtxt

#********************LOCAL IMPORTS**************************
from printer_controller.fly_commander import FLYWorker
from calibration.auto_bed_leveling import ABL_Interpolation

class ProcessesManager:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def USB(self): #call this func whenever USB_calculate button is pressed
        if self.main_window.USB_CONNECTED==1 and self.main_window.A==0:
            self.main_window.pipe = win32pipe.CreateNamedPipe(
            r'\\.\pipe\Foo',
            win32pipe.PIPE_ACCESS_DUPLEX,
            win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
            1, 65536, 65536,
            0,
            None)
            self.main_window.pause_state=0
            self.main_window.A=1 #printing mode
            self.main_window.usb_printing=1
            self.main_window.start_bar()
            if self.main_window.ABL==0:
                gcode_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
                gcode = self.main_window.GCODE_Panel.toPlainText()
                gcode_file.write(gcode)
                gcode_file.close()
                self.main_window.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
                self.main_window.data = self.main_window.GCODE_Panel.toPlainText()
                self.main_window.file1.write(self.main_window.data)
                self.main_window.file1.close()
            self.main_window.filehandler.save_settings()
            self.main_window.savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w')
            self.main_window.savepathfile.close()
            self.main_window.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w')
            self.main_window.child_file.write(str(struct.pack("i",self.main_window.mirror)))
            self.main_window.child_file.close()
            self.main_window.widgethandler.disable_idle_buttons()

    def SD_CARD(self):
        gcode_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
        gcode = self.main_window.GCODE_Panel.toPlainText()
        gcode_file.write(gcode)
        gcode_file.close()
        self.main_window.Message_panel.append(">>> Mode: SD Printing")
        path = QtWidgets.QFileDialog.getExistingDirectory(self.main_window, "Select Directory")
        savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w',encoding="utf-8") #encoding is important
        savepathfile.write(path)
        savepathfile.close()
        self.main_window.filehandler.save_settings()
        if(path != ''):
           self.main_window.pipe = win32pipe.CreateNamedPipe(
           r'\\.\pipe\Foo',
           win32pipe.PIPE_ACCESS_DUPLEX,
           win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
           1, 65536, 65536,
           0,
           None)
           self.main_window.Message_panel.append(">>> path: " + path)
           self.main_window.start_bar()
           self.main_window.file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.main_window.data = self.main_window.GCODE_Panel.toPlainText()
           self.main_window.file.write(self.main_window.data)
           self.main_window.file.close()
           self.main_window.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w')
           self.main_window.child_file.write(str(struct.pack("i",self.main_window.mirror)))
           self.main_window.child_file.close()
           p2 = subprocess.Popen("3DBrain.exe")
        else:
           self.main_window.Message_panel.append(">>> Aborted ")

    def PAUSE(self):
        if self.main_window.usb_printing==1 and self.main_window.froze==0:
           if self.main_window.pause_state==0: #pass M226 command to C
              self.main_window.MM=226
              self.main_window.SS=0
              self.main_window.pause_state=1 #paused
              self.main_window.froze=1
              self.main_window.froze_loop=1
              self.main_window.Message_panel.append(">>> M226: Pause request")
              self.main_window.fly_thread = FLYWorker(self.main_window)
              self.main_window.fly_thread.start()
           else:
             self.main_window.ser.write(struct.pack("B",self.main_window.A)) #trash, to resume printing
             self.main_window.pause_state=0 #resume
             self.main_window.froze=1
             self.main_window.froze_loop=0
             self.main_window.Message_panel.append(">>> Resume printing... ")
    def CANCEL(self):
        self.main_window.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')
        if self.main_window.child_buffer_size!=0:
            self.main_window.Message_panel.append(">>> Aborted")
            child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w') #reset child so 3DHex.C to terminate
            child_file.close()
            self.main_window.usb_printing=0
            self.main_window.A=0
        if self.main_window.USB_CONNECTED==1 and self.main_window.rapid_pos==1:
            self.main_window.Message_panel.append(">>> Aborted")
            self.main_window.ser.write(struct.pack("B",self.main_window.A))



    def execute_ABL(self):
        self.main_window.width = float(self.main_window.b53.toPlainText().strip())
        self.main_window.length = float(self.main_window.b54.toPlainText().strip())
        self.main_window.rows = float(self.main_window.b55.toPlainText().strip())
        self.main_window.columns = float(self.main_window.b56.toPlainText().strip())
        self.main_window.iterations = float(self.main_window.b57.toPlainText().strip())
        self.main_window.xtool_offset = float(self.main_window.b58.toPlainText().strip())
        self.main_window.ytool_offset = float(self.main_window.b59.toPlainText().strip())
        self.main_window.XY_Feed = float(self.main_window.b60.toPlainText().strip())
        safez = float(self.main_window.b62.toPlainText().strip())
        self.main_window.Z_Feed = float(self.main_window.b63.toPlainText().strip())
        self.main_window.Xstart = float(self.main_window.b64.toPlainText().strip())
        self.main_window.Xend = float(self.main_window.b65.toPlainText().strip())
        self.main_window.Ystart = float(self.main_window.b67.toPlainText().strip())
        self.main_window.Yend = float(self.main_window.b68.toPlainText().strip())
        self.main_window.repeat = int(self.main_window.b69.toPlainText().strip())
        self.main_window.abl_type = self.main_window.comboBox4.currentIndex()

        centerX = (self.main_window.width / 2.0) - self.main_window.xtool_offset
        centerY = (self.main_window.length / 2.0) - self.main_window.ytool_offset
        stepx = (self.main_window.Xend - self.main_window.Xstart) / (self.main_window.columns - 1)
        stepy = (self.main_window.Yend - self.main_window.Ystart) / (self.main_window.rows - 1)

        gpsx = self.main_window.Xstart
        gpsy = self.main_window.Ystart
        with open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\support files\\GCODE.txt', 'w') as ABL_f:
            if self.main_window.abl_type == 2:  # MESH Interpolation
                ABL_f.write('G2929' + '\n')
                ABL_f.write('G1 Z' + str(safez) + ' F' + str(int(self.main_window.Z_Feed)) + '\n')
                ABL_f.write('G1 X' + str(centerX) + ' Y' + str(centerY) + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G92 X' + str(self.main_window.width / 2.0) + ' Y' + str(self.main_window.length / 2.0) + '\n')
                ABL_f.write('G28 Z' + '\n')
                x_iter = 0
                y_iter = 0
                while y_iter < self.main_window.rows:
                    gpsx = self.main_window.Xstart
                    while x_iter < self.main_window.columns:
                        ABL_f.write('G1 X' + str(gpsx) + ' Y' + str(gpsy) + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                        ABL_f.write('G28 Z' + '\n')
                        gpsx = gpsx + stepx
                        x_iter = x_iter + 1
                    x_iter = 0
                    gpsy = gpsy + stepy
                    y_iter = y_iter + 1
                ABL_f.write('G1 X' + str(self.main_window.width / 2.0) + ' Y' + str(self.main_window.length / 2.0) + ' F' + str(
                    int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G2929' + '\n')
                ABL_f.write('G92 X' + str(centerX) + ' Y' + str(centerY) + '\n')
                ABL_f.write('G1 X0 Y0' + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G1 Z0' + ' F' + str(int(self.main_window.Z_Feed)) + '\n')
                self.main_window.m1.setPlainText('MESH')
            elif self.main_window.abl_type == 1:  # PLANE Interpolation
                ABL_f.write('G2929' + '\n')
                ABL_f.write('G1 Z' + str(safez) + ' F' + str(int(self.main_window.Z_Feed)) + '\n')
                ABL_f.write('G1 X' + str(centerX) + ' Y' + str(centerY) + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G92 X' + str(self.main_window.width / 2.0) + ' Y' + str(self.main_window.length / 2.0) + '\n')
                ABL_f.write('G28 Z' + '\n')
                ABL_f.write('G1 X' + str(self.main_window.Xstart) + ' Y' + str(self.main_window.Ystart) + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G28 Z' + '\n')
                ABL_f.write('G1 X' + str(self.main_window.Xend) + ' Y' + str(self.main_window.Ystart) + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G28 Z' + '\n')
                ABL_f.write('G1 X' + str((self.main_window.Xend + self.main_window.Xstart) / 2.0) + ' Y' + str(self.main_window.Yend) + ' F' + str(
                    int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G28 Z' + '\n')
                ABL_f.write('G1 X' + str(self.main_window.width / 2.0) + ' Y' + str(self.main_window.length / 2.0) + ' F' + str(
                    int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G2929' + '\n')
                ABL_f.write('G92 X' + str(centerX) + ' Y' + str(centerY) + '\n')
                ABL_f.write('G1 X0 Y0' + ' F' + str(int(self.main_window.XY_Feed)) + '\n')
                ABL_f.write('G1 Z0' + ' F' + str(int(self.main_window.Z_Feed)) + '\n')
                self.main_window.m1.setPlainText('PLANE')
            elif self.main_window.abl_type == 0:
                self.main_window.m1.setPlainText('NULL')
        ABL_f.close()
        self.main_window.ABL = 1
        self.main_window.USB()

    def View(self):
        if pyplot.fignum_exists(self.main_window.plot_num)==False:
            bedx = float(self.main_window.b53.toPlainText().strip())
            bedy = float(self.main_window.b54.toPlainText().strip())
            grid = float(self.main_window.b66.toPlainText().strip())
            interpolation = 1
            if grid < 1:
                interpolation = 0
                self.main_window.Message_panel.append(">>> Invalid grid value: <0")
            if grid % 1 != 0:
                interpolation = 0
                self.main_window.Message_panel.append(">>> Invalid grid value: float")
            if bedx % grid !=0:
                interpolation = 0
                self.main_window.Message_panel.append(">>> Invalid grid value: bed x_size modulus grid ")
            if bedy % grid != 0:
                interpolation = 0
                self.main_window.Message_panel.append(">>> Invalid grid value: bed y_size modulus grid ")
            if interpolation == 1:
                self.main_window.ABL_interpolation_thread = ABL_Interpolation(self.main_window)
                self.main_window.ABL_interpolation_thread.message.connect(self.main_window.widgethandler.Plot)
                self.main_window.ABL_interpolation_thread.start()

    def setJFAJ(self):
        if self.main_window.usb_printing==1 and self.main_window.USB_CONNECTED==1 and self.main_window.froze==0 and self.main_window.froze_loop==0:
            self.main_window.froze=1
            self.main_window.MM=0
            self.main_window.Message_panel.append(">>> Set JFAJ")
            self.main_window.fly_thread = FLYWorker(self.main_window)
            self.main_window.fly_thread.start()

    def nozz_AUTOTUNE(self):
        self.main_window.nozz_auto_tune=1
        self.main_window.A=0
        self.main_window.B=5
        self.main_window.C=1
    def bed_AUTOTUNE(self):
        self.main_window.nozz_auto_tune=1
        self.main_window.A=0
        self.main_window.B=5
        self.main_window.C=0
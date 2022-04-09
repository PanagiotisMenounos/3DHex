#----------------------------------------------------------------------
#      COPYRIGHT NOTICE FOR 3DHex:
#----------------------------------------------------------------------
#
#      3DHex - 3D Printer Firmware
#
#Copyright (c) 2021 Panagiotis Menounos
#Contact: 3DHexfw@gmail.com
#
#
#3DHex is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#3DHex is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with 3DHex.  If not, see <http://www.gnu.org/licenses/>.



import sys
import serial.tools.list_ports
from PyQt5 import QtWidgets, uic, QtCore
from PyQt5.QtWidgets import QInputDialog, QLineEdit
from PyQt5.QtCore import Qt, QThread, pyqtSignal
import time
import os
import easygui
from mainwindow_design import Ui_MainWindow
from printer_name import Ui_New_Printer
from autotune import Ui_AutoTune
from multiprocessing import Process #for multiprocessing
import threading
import subprocess
import multiprocessing
import struct
import win32pipe, win32file, pywintypes
import numpy as np
from scipy import interpolate
from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
from scipy.interpolate import Rbf
import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
from numpy import loadtxt
from PyQt5 import QtGui
from PyQt5.QtGui import *

class ProgressBarWorker(QThread):
  message = pyqtSignal(str) #define signal
  progress_value = pyqtSignal(int) #define signal
  def run(self):
        try:
            received_progress=0
            win32pipe.ConnectNamedPipe(window.pipe, None)
            resp = win32file.ReadFile(window.pipe,4)	
            (received_progress,)=struct.unpack("i",resp[1])
            while received_progress <=100:
                self.progress_value.emit(received_progress) #emit the signal
                resp = win32file.ReadFile(window.pipe,4)	
                (received_progress,)=struct.unpack("i",resp[1])
                if received_progress==100:
                   self.progress_value.emit(received_progress) #emit the signal
                   received_progress=101
        except:
             self.progress_value.emit(0)
             win32file.CloseHandle(window.pipe)
        finally:
              win32file.CloseHandle(window.pipe)
              #print("closed pipe")
              self.message.emit(">>> Completed") #emit the signal

class COMPortWorker(QThread):
    def run(self):
        while True:
            prtsNow = serial.tools.list_ports.comports()
            if len(window.ports) < len(prtsNow):
                for i  in prtsNow:
                    if i not in window.ports:
                        window.comboBox.addItem(i.device)
                window.ports = serial.tools.list_ports.comports()
            elif len(window.ports) > len(prtsNow):
                for i  in window.ports:
                    if i not in prtsNow:
                        if i.device == window.chosenPort:
                            window.con_state = 0
                            #self.p0.setText("Connect")
                            window.comboBox.setCurrentIndex(0)
                        window.comboBox.removeItem(window.comboBox.findText(i.device))
                window.ports = serial.tools.list_ports.comports()
            time.sleep(0.5)
            
class FLYWorker(QThread):
    message = pyqtSignal(str) #define signal
    def run(self):
        if window.MM!=0: #M command
           win32file.WriteFile(window.pipe,(struct.pack("if",window.MM,window.SS))) #send command to C via pipe
        else: # % command
           win32file.WriteFile(window.pipe,(struct.pack("i",window.MM)))
           window.SS=float(window.d100.value())
           win32file.WriteFile(window.pipe,(struct.pack("f",window.SS)))
           window.SS=float(window.d101.value())
           win32file.WriteFile(window.pipe,(struct.pack("f",window.SS)))
           window.SS=float(window.d102.value())
           win32file.WriteFile(window.pipe,(struct.pack("f",window.SS)))
           window.SS=float(window.d103.value())
           win32file.WriteFile(window.pipe,(struct.pack("f",window.SS)))       
        window.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\fly.bin','wb')
        window.child_file.write(struct.pack("i",window.mirror))
        window.child_file.close()   
        time.sleep(3)
        window.froze=0        

class ABL_Interpolation(QThread):
    message = pyqtSignal(str)
    def run(self):
        method = str(window.comboBox1.currentText())
        view_percentage = int(window.comboBox2.currentText().replace("%", ""))
        width=float(window.b53.toPlainText().strip())
        length=float(window.b54.toPlainText().strip())
        grid=float(window.b66.toPlainText().strip())
  
        x = loadtxt(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\abl_x.txt')
        y = loadtxt(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\abl_y.txt')
        z = loadtxt(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\abl_z.txt')
        
        xx = np.arange(0, width+1, grid)
        yy = np.arange(0, length+1, grid)
        xx, yy = np.meshgrid(xx, yy)
        
        rbfi = Rbf(x, y, z,function=method)  # radial basis function interpolator instance
        znew = rbfi(xx, yy)   # interpolated values
        
        
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\XYZ.txt','w')
        size = xx.ravel().size
        view_step = int((100/view_percentage)*grid)
        i=0
        while i < size:
            file.write(str(xx.ravel()[i])+' '+str(yy.ravel()[i])+' '+str(round(znew.ravel()[i]*int(window.b3.toPlainText().strip())))+'\n')
            i=i+1
        file.close()
        
        xx = np.arange(0, width, view_step)
        yy = np.arange(0, length, view_step)
        xx, yy = np.meshgrid(xx, yy)
        
        rbfi = Rbf(x, y, z,function=method)  # radial basis function interpolator instance
        znew = rbfi(xx, yy)

        view_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\XYZ_view.txt','w')      
        size = xx.ravel().size
        i=0
        while i < size:
            view_file.write(str(xx.ravel()[i])+' '+str(yy.ravel()[i])+' '+str(znew.ravel()[i])+'\n')
            if(i%view_step==0):
                view_file.write(str(xx.ravel()[i])+' '+str(yy.ravel()[i])+' '+str(znew.ravel()[i])+'\n')
            i=i+1
        view_file.close()
        self.message.emit("nothing")

class USBWorker(QThread): #This thread starts when 3DHEX connects successfully to the Printer
    message = pyqtSignal(str) #define signal
    new_nozz_temp = pyqtSignal(float)
    new_bed_temp = pyqtSignal(float)
    autotune_p = pyqtSignal(float)
    x_pos_report = pyqtSignal(float)
    y_pos_report = pyqtSignal(float)
    z_pos_report = pyqtSignal(float)
    
    def run(self):
        time.sleep(2)
        self.send_buffer() #Set printer to idle mode temp only report
        self.message.emit(">>> Mode: Idle") #emit the signal
        try:
            while window.USB_CONNECTED==1:
                (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #Read temperature
                #print("XPOS="+str(window.X_POS))
                if window.A==0: #if in idle mode
                    if self.serial_command==-200: #-200 autotune read settings
                      window.Auto_P = round(window.nozz_temp,2)
                      window.Auto_I = round(window.bed_temp,2)
                      (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #Read temperature
                      window.Auto_D = round(window.nozz_temp,2)
                      self.message.emit(">>> AUTOTUNE RESULTS") #emit the signal
                      self.message.emit(">>> P=" +str(window.Auto_P))
                      self.message.emit(">>> I=" +str(window.Auto_I))
                      self.message.emit(">>> D=" +str(window.Auto_D))
                      self.autotune_p.emit(window.Auto_I)
                      self.serial_command=-243
                    elif self.serial_command==-301: #temporary
                        self.message.emit(">>> ZTRACK=" +str(int(window.nozz_temp)))
                        self.serial_command=-243
                    else: #just update temp
                      self.new_nozz_temp.emit(window.nozz_temp) #emit the signal
                      self.new_bed_temp.emit(window.bed_temp) #emit the signal
                      window.X_POS=window.InvertX*window.X_POS
                      self.x_pos_report.emit(window.X_POS)
                      self.y_pos_report.emit(window.Y_POS)
                      self.z_pos_report.emit(window.Z_POS)
                    time.sleep(0.5)
                    self.check_idle_commands() #Check if any idle command has triggered
                else:
                   self.message.emit(">>> Mode: USB printing") #emit the signal
                   #window.Message_panel.append(">>> Start usb printing")
                   time.sleep(0.5)
                   self.usb_printing() #Go into USB Print function
                   time.sleep(5)
                   window.A=0 #After USB printing has completed declare idle mode
                   window.B=0
                   window.C=0
                   window.ser.flushInput() #very important without delay to fix a bug
                   window.ser.flushOutput()#very important without delay to fix a bug
                   self.send_buffer() #Then send idle mode temp only report to printer
                   window.enable_idle_buttons()
        except:
            window.USB_CONNECTED=0

    def send_buffer(self):
        window.ser.write(struct.pack("8B4H2B6H",window.A,window.B,window.C,window.D,window.E,window.F,window.G,window.H,window.I,window.J,window.K,window.L,window.M,window.N,window.O,window.P,window.Q,window.R,window.S,window.T))
        (pass_fail,)=struct.unpack("B",window.ser.read(1)) #Wait for arduino to confirm everything is ok
        #if pass_fail==1: #pass_fail should be 1, else communication has failed
           #print("PASS")
        #else:
           #print("FAILED")

    def packet_decode(self):
        (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #This first time read buffer1 contains all the necessary settings for Printer
        while int(self.serial_command)!=-253 and self.child_buffer_size!=0 and window.usb_printing==1 and self.serial_command!=-260:
            if int(self.serial_command)==-300: #300-> start of ABL
                self.abl_z_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\abl_z.txt',"w")
            if int(self.serial_command)==-301: #-301-> TRACK_Z
                self.trackZ=self.trackZ+window.nozz_temp
                if window.ABL_Sample==0:
                    if window.min==0:
                        window.min = window.nozz_temp
                    if window.nozz_temp < window.min:
                        window.min = window.nozz_temp
            if int(self.serial_command)==-303: #-303-> end of iterations
                    self.AVG_traxkZ=self.trackZ/2
                    self.trackZ=0
                    if window.ABL_Sample==0:
                        window.ABL_Z_CENTER=window.min
                        self.AVG_traxkZ=window.min
                        window.ABL_Sample=1
                    self.AVG_traxkZ=(window.ABL_Z_CENTER-self.AVG_traxkZ)/window.STPZ
                    self.message.emit(">>> AVG:"+str("{:.4f}".format(round(float(self.AVG_traxkZ), 4)))+"mm") #emit the signal  
                    self.abl_z_file.write(str("{:.4f}".format(round(float(self.AVG_traxkZ), 4)))+"\n")
            if int(self.serial_command)==-302: #302-> end of ABL
                window.ABL_Sample=0
                self.AVG_traxkZ=0
                self.trackZ=0
                self.abl_z_file.close()
                window.ABL=0
                #self.ABL_interpolation_thread=ABL_Interpolation()
                #self.ABL_interpolation_thread.start()
            if int(self.serial_command)==-243: #temp report
                self.new_nozz_temp.emit(window.nozz_temp) #emit the signal
                self.new_bed_temp.emit(window.bed_temp) #emit the signal
                self.x_pos_report.emit(window.X_POS)
                self.y_pos_report.emit(window.Y_POS)
                self.z_pos_report.emit(window.Z_POS)
                ##print("XPOS="+str(window.X_POS))               
            (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #This first time read buffer1 contains all the necessary settings for Printer
            self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')

    def usb_printing(self): #USB Printing function 
        window.usb_printing=1
        self.trackZ=0
        window.A=1 #printing mode
        self.serial_command=0 #reset serial command from arduino
        flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
        flag_file.write(struct.pack('2i', 5, 5)) #Write some trash data in order for 3DHex.C to know Python is in printing function
        flag_file.close()
        flag_py_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag_py.bin',"wb") #Clear flag file
        flag_py_file.close()
        buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin', "wb") #Reset buffer1 binary file
        buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin', "wb") #Reset buffer2 binary file
        buffer1_file.close()
        buffer2_file.close()
        self.message.emit(">>> GCODE Post processing..this may take a while..") #emit the signal
        p1 = subprocess.Popen("Brain.exe") #Start 3DHex.C Proccess 
        flag_py_buffer=0 #Reset flag_py_buffer
        filecase=1 #Read from buffer1 file
        buffer_file_size=3100 #Declare buffer file size (This is max arduino buffer array size until all RAM is full)
        self.child_buffer_size=1 #Means 3DHex.C is still running
        while flag_py_buffer==0 and window.usb_printing==1:#wait for C to fill binary data to buffer1+buffer2 binary files
            (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #Read arduino temp report
            self.new_nozz_temp.emit(window.nozz_temp) #emit the signal
            self.new_bed_temp.emit(window.bed_temp) #emit the signal
            self.x_pos_report.emit(window.X_POS)
            self.y_pos_report.emit(window.Y_POS)
            self.z_pos_report.emit(window.Z_POS)
            flag_py_buffer=os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag_py.bin') #Get size of flag_py_file, if 1 then C has fill binary files
        window.ser.flushInput() #very important without delay to fix a bug
        window.ser.flushOutput()#very important without delay to fix a bug
        self.send_buffer() #Command Printer to go into printig mode window.A=1
        self.message.emit(">>> Post processing successfully completed") #emit the signal
        self.message.emit(">>> Printing...") #emit the signal        
        if buffer_file_size==3100 and self.child_buffer_size!=0 and self.serial_command!=-260: #Firt time send binary data to Printer
            if filecase==1: #Read from buffer1 binary file
                filecase=2  #Note to read buffer2 next time
                buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin', "rb")
                self.packet_decode()
                window.ser.write(buffer1_file.read(3100)) #Send binary data to Printer
                buffer1_file.close() 
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5)) #Write some trash data to tell C that buffer1 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            if filecase==2:
                filecase=1
                buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin', "rb")
                self.packet_decode()
                window.ser.write(buffer2_file.read(3100))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5)) #Write some trash data to tell C that buffer2 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')
        while self.serial_command!=-10 and self.child_buffer_size!=0 and window.usb_printing==1: #Wait for printer to read commanded temp
            (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #Read arduino temp report
            self.new_nozz_temp.emit(window.nozz_temp) #emit the signal
            self.new_bed_temp.emit(window.bed_temp) #emit the signal
            self.x_pos_report.emit(window.X_POS)
            self.y_pos_report.emit(window.Y_POS)
            self.z_pos_report.emit(window.Z_POS)
        while buffer_file_size==3100 and self.child_buffer_size!=0 and self.serial_command!=-260 and window.usb_printing==1: #Start binary data streaming to Printer 
            if filecase==1:
                filecase=2
                buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin', "rb")
                self.packet_decode()                      
                window.ser.write(buffer1_file.read(3100))
                buffer1_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            elif filecase==2:
                filecase=1
                buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin', "rb")
                self.packet_decode()
                window.ser.write(buffer2_file.read(3100))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')
        self.child_buffer_size=1 #catch last packet after C terminates, if MCU does ot send -260 this will stuck in loop
        self.packet_decode()
        child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w') #reset child so 3DHex.C to terminate
        child_file.close()
        savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w') #reset path
        savepathfile.close()
        buffer1_file.close()
        buffer2_file.close()
        window.usb_printing=0

    def check_idle_commands(self): #idle mode commands func
            if window.set_temp==1: #1 Set NOZZLE temp
                window.set_temp=0
                window.B=0
                window.C=0
                window.D=0
                window.E=int(window.c26.isChecked())
                window.G=int(window.c22.isChecked())
                window.I=int(window.b35.toPlainText().strip())
                window.K=int(window.b39.toPlainText().strip())
                window.M=int(window.b37.toPlainText().strip())
                window.O=int(float(window.b41.toPlainText().strip())*10.0)
                window.P=int(float(window.b43.toPlainText().strip())*10.0)
                window.Q=int(float(window.b45.toPlainText().strip())*10.0)
                print(window.O)
                time.sleep(0.2)
                self.send_buffer()
                window.enable_idle_buttons()
            if window.set_temp==2: #2 SET bed temp
                window.set_temp=0
                window.B=0
                window.C=1
                window.D=0
                window.E=int(window.c27.isChecked())
                window.G=int(window.c23.isChecked())
                window.I=int(window.b36.toPlainText().strip())
                window.K=int(window.b40.toPlainText().strip())
                window.M=int(window.b38.toPlainText().strip())
                window.O=int(float(window.b42.toPlainText().strip())*10.0)
                window.P=int(float(window.b44.toPlainText().strip())*10.0)
                window.Q=int(float(window.b46.toPlainText().strip())*10.0)
                time.sleep(0.2)
                self.send_buffer()            			   
                window.enable_idle_buttons()
            if window.set_fan==1:
               window.set_fan=0
               window.B=4
               window.J=int(window.d104.value())
               #print(window.J)
               self.send_buffer()
            if window.set_motor==1: #Enable/Disable motor
                window.set_motor=0
                window.B=1
                window.E=0
                window.F=0
                window.G=0
                window.H=0
                window.I=0
                window.J=0
                window.K=0
                window.L=0
                window.M=0
                window.N=0
                time.sleep(0.2)
                self.send_buffer()
                window.enable_idle_buttons()

            if window.home_axis==1: #Homing
                window.A=0
                window.B=2
                if window.C==0:
                    window.D = int(window.c2.isChecked())                                             #X ENABLE
                    window.E = int(window.c12.isChecked())                                            #HOME_X_ENABLE
                    window.F = int(window.c17.isChecked())                                            #HOME_X_STATE
                    window.I=int(1000000/(int(window.b1.toPlainText().strip())*window.d5.value()*2))  #DURATION
                if window.C==1:
                    window.D = int(window.c3.isChecked())                                             #Y ENABLE
                    window.E = int(window.c13.isChecked())                                            #HOME_Y_ENABLE
                    window.F = int(window.c18.isChecked())                                            #HOME_Y_STATE
                    window.I=int(1000000/(int(window.b2.toPlainText().strip())*window.d6.value()*2))  #DURATION
                if window.C==2:
                    window.D = int(window.c4.isChecked())                                             #Z ENABLE
                    window.E = int(window.c14.isChecked())                                            #HOME_Z_ENABLE
                    window.F = int(window.c19.isChecked())                                            #HOME_Z_STATE
                    window.I=int(1000000/(int(window.b3.toPlainText().strip())*window.d7.value()*2))  #DURATION
                time.sleep(0.2)
                self.send_buffer()
                if window.C==0:
                      if window.H==1:
                          window.p5.setEnabled(True)
                      else:
                          window.p6.setEnabled(True)
                if window.C==1:
                      if window.H==1:
                          window.p9.setEnabled(True)
                      else:
                          window.p10.setEnabled(True)
                if window.C==2:
                      if window.H==1:
                          window.p13.setEnabled(True)
                      else:
                          window.p14.setEnabled(True)
                window.enable_idle_buttons()
                window.home_axis=0

            if window.rapid_pos==1: #Rapid positioning command
                self.send_buffer()
                (self.serial_command,window.nozz_temp,window.bed_temp,window.X_POS,window.Y_POS,window.Z_POS,)=struct.unpack("3f3l",window.ser.read(24)) #wait for arduino to end process
                self.new_nozz_temp.emit(window.nozz_temp) #emit the signal
                self.new_bed_temp.emit(window.bed_temp) #emit the signal
                if window.C==0:
                   if window.F==1:
                      window.p7.setEnabled(True)
                   else:
                      window.p8.setEnabled(True)
                if window.C==1:
                   if window.F==1:
                      window.p11.setEnabled(True)
                   else:
                      window.p12.setEnabled(True)
                if window.C==2:
                   if window.F==1:
                      window.p15.setEnabled(True)
                   else:
                      window.p16.setEnabled(True)
                if window.C==3:
                   if window.F==1:
                      window.p19.setEnabled(True)
                   else:
                      window.p20.setEnabled(True)
                window.enable_idle_buttons()
                window.rapid_pos=0
            
            if window.nozz_auto_tune==1: 
                window.nozz_auto_tune=0
                if window.C==1:
                     self.message.emit(">>> NOZZLE AUTOTUNE") #emit the signal 
                     print(window.B)                     
                else:          
                     self.message.emit(">>> BED AUTOTUNE")                
                self.send_buffer()                
                                

class PrinterWindow(QtWidgets.QMainWindow, Ui_New_Printer):
    def __init__(self, *args, obj=None, **kwargs):
        super(PrinterWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.ok_printer.clicked.connect(self.OK_Printer)
        self.cancel_printer.clicked.connect(self.CANCEL_Printer)
        window.Message_panel.append(">>> Add new settings")
     
    def OK_Printer(self):
        text = self.input_name.toPlainText().replace("\n","")
        if text== '':
            window.Message_panel.append(">>> Aborted")
            self.close()
        else:
            #print(text)
            window.freeroom=0
            window.added=0
            window.Message_panel.append(">>> Added: " + text)
            printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','r')
            printers = printer_file.readlines()
            printer_file.close()
            printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','w')
            window.printers = len(printers)
            #print(window.printer)
            for temp in printers:
                printer = temp.split(',')
                printer[2]=printer[2].replace("\n","")
                if int(printer[2])==0:
                    window.freeroom=1
            if window.freeroom==1:
                for temp in printers:
                    printer = temp.split(',')
                    printer[2]=printer[2].replace("\n","")
                    if int(printer[2])==0 and window.added==0:
                        printer_file.write("1,"+text+",1\n")
                        window.Message_panel.append(">>> Added: " + text)
                        window.added=1
                    else:
                        printer_file.write("0,"+printer[1]+","+printer[2]+"\n")
            else:
                window.Message_panel.append(">>> Cannot add more than " + str(window.printers) + " configurations")
                for temp in printers:
                    printer = temp.split(',')
                    printer[2]=printer[2].replace("\n","")
                    if window.added==0:
                        printer_file.write("1,Default,1\n")
                        window.added=1
                    else:
                        printer_file.write("0,"+printer[1]+","+printer[2]+"\n")
            window.freeroom=0
            window.added=0
            printer_file.close()
            window.save_settings()
            window.load_printers()
            self.close()
        
    def CANCEL_Printer(self):
        self.close()
        window.Message_panel.append(">>> Aborted")

class AutoTuneWindow(QtWidgets.QMainWindow, Ui_AutoTune):
    def __init__(self, *args, obj=None, **kwargs):
        super(AutoTuneWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.ok_autotune.clicked.connect(self.OK_Printer)
        self.cancel_autotune.clicked.connect(self.CANCEL_Printer)
     
    def OK_Printer(self):
        if window.C==1:
            window.b41.clear()
            window.b41.insertPlainText(str(window.Auto_P)) 
            window.b43.clear()
            window.b43.insertPlainText(str(window.Auto_I)) 
            window.b45.clear()
            window.b45.insertPlainText(str(window.Auto_D)) 
        else:
            window.b42.clear()
            window.b42.insertPlainText(str(window.Auto_P)) 
            window.b44.clear()
            window.b44.insertPlainText(str(window.Auto_I)) 
            window.b46.clear()
            window.b46.insertPlainText(str(window.Auto_D))
        window.Message_panel.append(">>> PID updated")
        self.close()
        
    def CANCEL_Printer(self):
        self.close()
        window.Message_panel.append(">>> Aborted")

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, *args, obj=None, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.Message_panel.append(">>> 3DHex")
        self.declare_vars()
        self.load_printers()
        self.setStyleSheet("QMenu{color: rgb(255, 255, 255);background-color: rgb(47, 47, 47);} QMenuBar{color: rgb(255, 255, 255);background-color: rgb(63, 63, 63);} QMenu::item:selected{background-color: rgb(83, 83, 83);} QMenuBar::item:selected{background-color: rgb(83, 83, 83);}");
        #self.actionPrinter2_2.setVisible(False) #test only
        self.UNITS()
        self.ABL_include()
        font_id =QtGui.QFontDatabase.addApplicationFont("digital-7.ttf")
        #self.l19_5.setFont(QFont("LCD", 16))
        self.XPOSITION.setFont(QFont("Digital-7", 20))
        self.YPOSITION.setFont(QFont("Digital-7", 20))
        self.ZPOSITION.setFont(QFont("Digital-7", 20))
        self.NOZZ_TEMP.setFont(QFont("Digital-7", 20))
        self.BED_TEMP.setFont(QFont("Digital-7", 20))

    def declare_vars(self):
        self.MM = 0
        self.SS = 0
        self.froze_loop=0 #prevent fly command when paused
        self.froze=0 #prevent pause to be pressed multiple times
        self.pause_state=0 #not at pause state
        self.mirror=5 #used as flag in C-Python
        self.printer=0
        self.set_motor=0
        self.USB_CONNECTED=0
        self.usb_connected = 1
        self.set_temp = 0	
        self.set_fan = 0
        self.home_axis=0
        self.usb_printing=0
        self.rapid_pos=0
        self.nozz_auto_tune=0
        self.A=0
        self.B=0
        self.C=0
        self.D=0
        self.E=0
        self.F=0
        self.G=0
        self.H=0
        self.I=0
        self.J=0
        self.K=0
        self.L=0
        self.M=0
        self.N=0
        self.O=0
        self.P=0
        self.Q=0
        self.R=0
        self.S=0
        self.T=0
        self.Auto_P=0
        self.Auto_I=0
        self.Auto_D=0
        self.update_temp=0
        self.ABL=0
        self.iterations=1.0
        self.STPZ=100
        self.ABL_Z_CENTER=0
        self.ABL_Sample=0
        self.plot_num=0
        self.min=0
        self.x_pos_last=0
        self.x_overflow=0
        self.x_overflow_last=0
        self.y_pos_last=0
        self.y_overflow=0
        self.y_overflow_last=0
        self.z_pos_last=0
        self.z_overflow=0
        self.z_overflow_last=0
        self.int16_base=0
        self.chosenPort = ""
        self.InvertX=1
        self.InvertY=1
        self.InvertZ=1
        self.ports = serial.tools.list_ports.comports()
        self.comboBox.addItem("")
        for p in self.ports:
            self.comboBox.addItem(p.device)

    def new_printer(self):
        self.window2 = PrinterWindow(self)
        self.window2.show()

    def test(self):
        self.window3 = AutoTuneWindow(self)
        self.window3.show()
        print(self.Auto_P)

    def select_printer(self,printer_selection):
        window.save_settings()
        printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','r')
        printers = printer_file.readlines()
        printer_file.close()
        printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','w')
        total_printers=len(printers)
        p=0
        for temp in printers:
            printer = temp.split(',')
            if p < total_printers:
                if p == printer_selection:
                    printer_file.write("1,"+printer[1]+","+printer[2])
                else:
                    printer_file.write("0,"+printer[1]+","+printer[2])
            p=p+1
        printer_file.close()
        self.load_printers()

    def remove_printer(self):
        printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','r')
        printers = printer_file.readlines()
        printer_file.close()
        printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','w')
        total_printers=len(printers)
        p=0
        for temp in printers:
            printer = temp.split(',')
            if p==0:
                printer_file.write("1,"+printer[1].replace("\n","")+",1\n")
            else:
                if p == self.printer:
                    printer_file.write("0,"+printer[1].replace("\n","")+",0\n")
                    self.Message_panel.append(">>> Removed: " + str(printer[1].replace("\n","")))
                else:
                    printer_file.write("0,"+printer[1].replace("\n","")+","+printer[2])
            p=p+1
        
        printer_file.close()
        self.load_printers()
        
    def load_printers(self):
        printer_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\printers\\printers.txt','r')
        printers = printer_file.readlines()
        total_printers=len(printers)
        p=0
        for temp in printers:
            printer = temp.split(',')
            if p < total_printers:
                if int(printer[0]) == 1:
                    self.printer = p
                    self.selected_printer=p
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    self.Message_panel.append(">>> Loaded: " + str(printer[1].replace("\n","")))
                    action_printer.setText(str("● "+printer[1].replace("\n","")))
                else:
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    action_printer.setText(str("   "+printer[1].replace("\n","")))
                if int(printer[2])==1:
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    action_printer.setVisible(True)
                else:
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    action_printer.setVisible(False)
            p=p+1
        printer_file.close()
        self.load_settings()
       
    def assign_buttons(self):
        self.p1.clicked.connect(self.CONNECT)
        self.p3.clicked.connect(self.USB)
        self.p5.clicked.connect(self.HOME_X_MIN)
        self.p6.clicked.connect(self.HOME_X_MAX)
        self.p9.clicked.connect(self.HOME_Y_MIN)
        self.p10.clicked.connect(self.HOME_Y_MAX)
        self.p13.clicked.connect(self.HOME_Z_MIN)
        self.p14.clicked.connect(self.HOME_Z_MAX)
        self.p7.clicked.connect(lambda:self.rapid_idle_position(0,1))
        self.p8.clicked.connect(lambda:self.rapid_idle_position(0,0))
        self.p11.clicked.connect(lambda:self.rapid_idle_position(1,1))
        self.p12.clicked.connect(lambda:self.rapid_idle_position(1,0))
        self.p15.clicked.connect(lambda:self.rapid_idle_position(2,1))
        self.p16.clicked.connect(lambda:self.rapid_idle_position(2,0))
        self.p19.clicked.connect(lambda:self.rapid_idle_position(3,1))
        self.p20.clicked.connect(lambda:self.rapid_idle_position(3,0))
        self.p21.clicked.connect(self.PAUSE)
        self.p4.clicked.connect(self.SD_CARD)
        self.p88.clicked.connect(self.setNOZZTEMP)
        self.p89.clicked.connect(self.setBEDTEMP)
        self.p22.clicked.connect(self.clear_GCODE)
        self.p23.clicked.connect(self.CANCEL)
        self.p24.clicked.connect(self.setJFAJ)
        self.p25.clicked.connect(self.setFAN1)
        self.p90.clicked.connect(self.nozz_AUTOTUNE)
        self.p91.clicked.connect(self.bed_AUTOTUNE)
        self.p29.clicked.connect(self.View)
        self.p27.clicked.connect(self.execute_ABL)
        self.action_Open.triggered.connect(self.openfile)
        #self.p2.clicked.connect(self.start_USB_worker)
        self.comboBox.currentTextChanged.connect(self.selectPort)#https://zetcode.com/pyqt/qcheckbox/
        self.c2.stateChanged.connect(self.setXmotor)                                                               
        self.c3.stateChanged.connect(self.setYmotor)                                                             
        self.c4.stateChanged.connect(self.setZmotor)                                                             
        self.c5.stateChanged.connect(self.setEmotor)
        #self.c7.stateChanged.connect(self.readInvertX)
        #self.c8.stateChanged.connect(self.readInvertY)
        #self.c9.stateChanged.connect(self.readInvertZ)
        self.c12.stateChanged.connect(self.setHOME_Xbuttons)
        self.c13.stateChanged.connect(self.setHOME_Ybuttons)
        self.c14.stateChanged.connect(self.setHOME_Zbuttons)
        self.c21.stateChanged.connect(self.UNITS)
        self.c29.stateChanged.connect(self.ABL_include)
        self.actionPrinter0.triggered.connect(lambda:self.select_printer(0))
        self.actionPrinter1.triggered.connect(lambda:self.select_printer(1))
        self.actionPrinter2.triggered.connect(lambda:self.select_printer(2))
        self.actionPrinter3.triggered.connect(lambda:self.select_printer(3))
        self.actionPrinter4.triggered.connect(lambda:self.select_printer(4))
        self.actionPrinter5.triggered.connect(lambda:self.select_printer(5))
        self.actionPrinter6.triggered.connect(lambda:self.select_printer(6))
        self.actionPrinter7.triggered.connect(lambda:self.select_printer(7))
        self.actionPrinter8.triggered.connect(lambda:self.select_printer(8))
        self.actionPrinter9.triggered.connect(lambda:self.select_printer(9))
        self.actionPrinter10.triggered.connect(lambda:self.select_printer(10))
        self.actionPrinter11.triggered.connect(lambda:self.select_printer(11))
        self.actionPrinter12.triggered.connect(lambda:self.select_printer(12))
        self.actionPrinter13.triggered.connect(lambda:self.select_printer(13))
        self.actionPrinter14.triggered.connect(lambda:self.select_printer(14))
        self.actionPrinter15.triggered.connect(lambda:self.select_printer(15))
        self.actionPrinter16.triggered.connect(lambda:self.select_printer(16))
        self.actionPrinter17.triggered.connect(lambda:self.select_printer(17))
        self.actionPrinter18.triggered.connect(lambda:self.select_printer(18))
        self.actionPrinter19.triggered.connect(lambda:self.select_printer(19))
        self.actionPrinter20.triggered.connect(lambda:self.select_printer(20))
        self.actionPrinter21.triggered.connect(lambda:self.select_printer(21))
        self.actionPrinter22.triggered.connect(lambda:self.select_printer(22))
        self.actionPrinter23.triggered.connect(lambda:self.select_printer(23))
        self.actionPrinter24.triggered.connect(lambda:self.select_printer(24))
        self.actionNew.triggered.connect(self.new_printer)
        self.actionRemove.triggered.connect(self.remove_printer)


    def UNITS(self):
        if self.c21.isChecked() == True:
            self.c21.setText("[mm/min]")
        else:
            self.c21.setText("[mm/sec]")

    def ABL_include(self):
        if self.c29.isChecked() == True:
            self.p27.setEnabled(False)
        else:
            self.p27.setEnabled(True)
  
  
    def CONNECT(self):
        try:
            if self.USB_CONNECTED==0:            
                self.save_settings()
                COM_PORT = self.chosenPort
                BAUD_RATE = int(self.b48.toPlainText().strip())
                self.ser=serial.Serial(COM_PORT,BAUD_RATE) #first time communicate with Printer
                self.ser.flushOutput()
                self.ser.flushInput() 
                self.A=0 #A=0 => Idle mode
                self.B=0 #B=0 => Temperature command
                self.C=0 #C=0 => Report only temperature		
                self.usb_thread = USBWorker() 
                self.usb_thread.message.connect(self.print2user_usb) #connect thread to message window
                self.usb_thread.new_nozz_temp.connect(self.update_nozz_temp) #connect thread to message window
                self.usb_thread.new_bed_temp.connect(self.update_bed_temp) #connect thread to message window
                self.usb_thread.x_pos_report.connect(self.update_xpos) 
                self.usb_thread.y_pos_report.connect(self.update_ypos) 
                self.usb_thread.z_pos_report.connect(self.update_zpos) 
                self.usb_thread.autotune_p.connect(self.test) #connect thread to autotune window
                self.usb_thread.start()       #Start usb communication handling thread
                self.enable_rapid_buttons()    #Enable home buttons
                for i in range (2,6): #c0-cmax
                     c = getattr(self, "c{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                     c.setChecked(1) 
                self.USB_CONNECTED=1
                self.Message_panel.append(">>> Successfully connected to: " + str(COM_PORT))
                self.p1.setStyleSheet('QPushButton {background-color:rgb(47, 47, 47);color: rgb(0,255,0);border-style: solid;border-radius:5px;border-width:2px; border-color:rgb(0,255,0);}\
                QPushButton:hover {border-width:3px; border-color:rgb(255, 115, 30);}\
                QPushButton:pressed {background-color:rgb(255, 115, 30);border-color: rgb(255, 195, 110);border-width: 4px;}\
                QPushButton:disabled{background-color: rgb(255, 149, 62);color: rgb(83, 83, 83);border-color:rgb(83, 83, 83);}')
            else:
                if self.usb_printing==0: #disconnect only allowed when on idle
                    self.p1.setStyleSheet('QPushButton {background-color:rgb(47, 47, 47);color: rgb(255,255,255);border-style: solid;border-radius:5px;border-width:2px; border-color:rgb(255,85,0);}\
                    QPushButton:hover {border-width:3px; border-color:rgb(255, 115, 30);}\
                    QPushButton:pressed {background-color:rgb(255, 115, 30);border-color: rgb(255, 195, 110);border-width: 4px;}\
                    QPushButton:disabled{background-color: rgb(255, 149, 62);color: rgb(83, 83, 83);border-color:rgb(83, 83, 83);}')
                    self.USB_CONNECTED=0
                    self.ser.close()
                    self.disable_rapid_buttons()
                    self.Message_panel.append(">>> Printer disconnected")
                    self.XPOSITION.setText("{:.3f}".format(round(0, 3)))
                    self.YPOSITION.setText("{:.3f}".format(round(0, 3)))
                    self.ZPOSITION.setText("{:.3f}".format(round(0, 3)))
                    self.NOZZ_TEMP.setText("{:.2f}".format(round(0, 2)))
                    self.BED_TEMP.setText("{:.2f}".format(round(0, 2)))
        except:
            self.Message_panel.append(">>> Failed to connect")		

    def USB(self): #call this func whenever USB_calculate button is pressed
        if self.USB_CONNECTED==1 and self.A==0:
            self.pipe = win32pipe.CreateNamedPipe(
            r'\\.\pipe\Foo',
            win32pipe.PIPE_ACCESS_DUPLEX,
            win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
            1, 65536, 65536,
            0,
            None)
            self.pause_state=0
            self.A=1 #printing mode
            self.usb_printing=1
            self.start_bar()
            if self.ABL==0:
                self.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
                self.data = self.GCODE_Panel.toPlainText()
                self.file1.write(self.data)
                self.file1.close()
            self.save_settings()
            self.savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w')
            self.savepathfile.close()
            self.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w')
            self.child_file.write(str(struct.pack("i",self.mirror)))
            self.child_file.close()
            self.disable_idle_buttons()

    def CANCEL(self):
        self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')
        if self.child_buffer_size!=0:
            self.Message_panel.append(">>> Aborted")
            child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w') #reset child so 3DHex.C to terminate
            child_file.close()
            self.usb_printing=0
            self.A=0
        if self.USB_CONNECTED==1 and self.rapid_pos==1:
            self.Message_panel.append(">>> Aborted")
            self.ser.write(struct.pack("B",self.A))

    def execute_ABL(self): 
        self.width=float(self.b53.toPlainText().strip())
        self.length=float(self.b54.toPlainText().strip())
        self.rows=float(self.b55.toPlainText().strip())
        self.columns=float(self.b56.toPlainText().strip())
        self.iterations=float(self.b57.toPlainText().strip())
        self.xtool_offset=float(self.b58.toPlainText().strip())
        self.ytool_offset=float(self.b59.toPlainText().strip())
        self.XY_Feed=float(self.b60.toPlainText().strip())
        safez=float(self.b62.toPlainText().strip())
        self.Z_Feed=float(self.b63.toPlainText().strip())
        self.Xstart=float(self.b64.toPlainText().strip())
        self.Xend=float(self.b65.toPlainText().strip())
        self.Ystart=float(self.b67.toPlainText().strip())
        self.Yend=float(self.b68.toPlainText().strip())
        self.repeat=int(self.b69.toPlainText().strip())
        
        centerX = (self.width/2.0)-self.xtool_offset
        centerY = (self.length/2.0)-self.ytool_offset
        stepx = (self.Xend-self.Xstart)/(self.columns-1)
        stepy = (self.Yend-self.Ystart)/(self.rows-1)
        
        gpsx = self.Xstart
        gpsy = self.Ystart
        with open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w') as ABL_f:
            ABL_f.write('G2929'+'\n')
            ABL_f.write('G1 Z'+str(safez)+' F'+str(int(self.Z_Feed))+'\n')
            ABL_f.write('G1 X'+str(centerX)+' Y'+str(centerY)+' F'+str(int(self.XY_Feed))+'\n')
            ABL_f.write('G92 X'+str(self.width/2.0)+' Y'+str(self.length/2.0)+'\n')
            ABL_f.write('G28 Z'+'\n')
            x_iter=0
            y_iter=0
            while y_iter < self.rows:
                gpsx = self.Xstart
                while x_iter < self.columns:
                    ABL_f.write('G1 X'+str(gpsx)+' Y'+str(gpsy)+' F'+str(int(self.XY_Feed))+'\n')
                    ABL_f.write('G28 Z'+'\n')
                    gpsx = gpsx + stepx
                    x_iter=x_iter+1
                x_iter=0
                gpsy = gpsy + stepy
                y_iter=y_iter+1
            ABL_f.write('G1 X'+str(self.width/2.0)+' Y'+str(self.length/2.0)+' F'+str(int(self.XY_Feed))+'\n')
            ABL_f.write('G92 X'+str(centerX)+' Y'+str(centerY)+'\n')
            ABL_f.write('G1 X0 Y0'+' F'+str(int(self.XY_Feed))+'\n')
            ABL_f.write('G1 Z0'+' F'+str(int(self.Z_Feed))+'\n')
            ABL_f.write('G2929'+'\n')
            
        ABL_f.close()
        self.ABL=1
        self.USB()
        
    def View(self):
        if pyplot.fignum_exists(self.plot_num)==False:
            self.ABL_interpolation_thread=ABL_Interpolation()
            self.ABL_interpolation_thread.message.connect(self.Plot)
            self.ABL_interpolation_thread.start()

    def Plot(self,message):    
        plt.style.use('dark_background')
        fig = pyplot.figure()
        self.plot_num=fig.number
        ax = fig.add_subplot(projection='3d')
        ax.set_zlim3d(-5, 5)
        ax.set_xlabel('X axis')
        ax.set_ylabel('Y axis')
        ax.set_zlabel('Z axis')
        my_cmap = plt.get_cmap('viridis')
        XYZ=loadtxt(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\XYZ_view.txt',delimiter=' ')
        surf=ax.plot_trisurf(XYZ[:,0], XYZ[:,1], XYZ[:,2],cmap = my_cmap,linewidth = 0.2,antialiased = True,edgecolor = 'grey')
        fig.colorbar(surf, location='right', shrink=0.4, aspect=20, pad=0.1)
        fig.tight_layout()
        pyplot.ion()
        pyplot.show()
            
    def enable_idle_buttons(self): #Enable after idle command
        window.c2.setEnabled(True)  
        window.c3.setEnabled(True) 
        window.c4.setEnabled(True) 
        window.c12.setEnabled(True)  
        window.c13.setEnabled(True) 
        window.c14.setEnabled(True)
        window.c5.setEnabled(True)
        window.p88.setEnabled(True)
        window.p89.setEnabled(True)
        

    def disable_idle_buttons(self): #Disable while in idle command
       if self.USB_CONNECTED==1:
          self.c2.setEnabled(False)  
          self.c3.setEnabled(False) 
          self.c4.setEnabled(False) 
          self.c12.setEnabled(False)  
          self.c13.setEnabled(False) 
          self.c14.setEnabled(False)
          self.c5.setEnabled(False)
          #self.p88.setEnabled(False)
          #self.p89.setEnabled(False)

    def enable_rapid_buttons(self):
        if self.c12.isChecked():
            self.p5.setEnabled(True)
            self.p6.setEnabled(True)
        if self.c13.isChecked():   
            self.p9.setEnabled(True)
            self.p10.setEnabled(True)
        if self.c14.isChecked():  
            self.p13.setEnabled(True)
            self.p14.setEnabled(True)
        self.p7.setEnabled(True)
        self.p8.setEnabled(True)
        self.p11.setEnabled(True)
        self.p12.setEnabled(True)
        self.p15.setEnabled(True)
        self.p16.setEnabled(True)
        self.p19.setEnabled(True)
        self.p20.setEnabled(True)

    def disable_rapid_buttons(self):
        self.p5.setEnabled(False)
        self.p6.setEnabled(False)  
        self.p9.setEnabled(False)
        self.p10.setEnabled(False)
        self.p13.setEnabled(False)
        self.p14.setEnabled(False)
        self.p7.setEnabled(False)
        self.p8.setEnabled(False)
        self.p11.setEnabled(False)
        self.p12.setEnabled(False)
        self.p15.setEnabled(False)
        self.p16.setEnabled(False)
        self.p19.setEnabled(False)
        self.p20.setEnabled(False)    

    def setHOME_Xbuttons(self,state):
        if state == Qt.Checked and self.c2.isChecked() and self.USB_CONNECTED==1:
            self.p5.setEnabled(True)
            self.p6.setEnabled(True)
        else:
            self.p5.setEnabled(False)
            self.p6.setEnabled(False) 
    def setHOME_Ybuttons(self,state):
        if state == Qt.Checked and self.USB_CONNECTED==1:
            self.p9.setEnabled(True)
            self.p10.setEnabled(True)
        else:
            self.p9.setEnabled(False)
            self.p10.setEnabled(False)
    def setHOME_Zbuttons(self,state):
        if state == Qt.Checked and self.USB_CONNECTED==1:
            self.p13.setEnabled(True)
            self.p14.setEnabled(True)
        else:
            self.p13.setEnabled(False)
            self.p14.setEnabled(False)

    def setXmotor(self,state):
        self.C=0
        if state == Qt.Checked:
            self.D=1
            if self.USB_CONNECTED==1:
                if self.c12.isChecked():
                    self.p5.setEnabled(True)
                    self.p6.setEnabled(True)
                self.p7.setEnabled(True)
                self.p8.setEnabled(True)
        else:
            self.D=0
            if self.USB_CONNECTED==1:
                self.p5.setEnabled(False)
                self.p6.setEnabled(False)
                self.p7.setEnabled(False)
                self.p8.setEnabled(False)
        self.disable_idle_buttons()
        self.set_motor=1

    def setYmotor(self,state):
        self.C=1
        if state == Qt.Checked:
            self.D=1
            if self.USB_CONNECTED==1:
                if self.c13.isChecked():
                    self.p9.setEnabled(True)
                    self.p10.setEnabled(True)
                self.p11.setEnabled(True)
                self.p12.setEnabled(True)
        else:
            self.D=0
            if self.USB_CONNECTED==1:
                self.p9.setEnabled(False)
                self.p10.setEnabled(False)
                self.p11.setEnabled(False)
                self.p12.setEnabled(False)
        self.disable_idle_buttons()
        self.set_motor=1

    def setZmotor(self,state):
        self.C=2
        if state == Qt.Checked:
            self.D=1
            if self.USB_CONNECTED==1:
                if self.c14.isChecked():
                    self.p13.setEnabled(True)
                    self.p14.setEnabled(True)
                self.p15.setEnabled(True)
                self.p16.setEnabled(True)
        else:
            self.D=0
            if self.USB_CONNECTED==1:
                self.p13.setEnabled(False)
                self.p14.setEnabled(False)
                self.p15.setEnabled(False)
                self.p16.setEnabled(False)
        self.disable_idle_buttons()
        self.set_motor=1

    def setEmotor(self,state):
        self.C=3
        if state == Qt.Checked:
            self.D=1
            if self.USB_CONNECTED==1:
                if self.c15.isChecked():
                    self.p17.setEnabled(True)
                    self.p18.setEnabled(True)
                self.p19.setEnabled(True)
                self.p20.setEnabled(True)
        else:
            self.D=0
            if self.USB_CONNECTED==1:
                self.p17.setEnabled(False)
                self.p18.setEnabled(False)
                self.p19.setEnabled(False)
                self.p20.setEnabled(False)
        self.disable_idle_buttons()
        self.set_motor=1

    def HOME_X_MIN(self):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.C=0
            self.G = int(self.c7.isChecked())                                      #HOME_X_DIRECTION
            self.H=1
            if self.G==1:
               self.G=0
            else:
               self.G=1 
            self.p5.setEnabled(False)
            self.home_axis=1

    def HOME_X_MAX(self):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.C=0
            self.G = int(self.c7.isChecked())                                      #HOME_X_DIRECTION
            self.H=0
            self.p6.setEnabled(False)
            self.home_axis=1

    def HOME_Y_MIN(self):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.C=1
            self.H=1
            self.G = int(self.c8.isChecked())                                      #HOME_X_DIRECTION
            if self.G==1:
               self.G=0
            else:
               self.G=1 
            self.p9.setEnabled(False)
            self.home_axis=1

    def HOME_Y_MAX(self):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.C=1
            self.H=0
            self.G = int(self.c8.isChecked())                                      #HOME_X_DIRECTION
            self.p10.setEnabled(False)
            self.home_axis=1

    def HOME_Z_MIN(self):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.C=2
            self.H=1
            self.G = int(self.c9.isChecked())                                      #HOME_X_DIRECTION
            if self.G==1:
               self.G=0
            else:
               self.G=1 
            self.p13.setEnabled(False)
            self.home_axis=1

    def HOME_Z_MAX(self):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.C=2
            self.H=0
            self.p14.setEnabled(False)
            self.G = int(self.c9.isChecked())                                      #HOME_X_DIRECTION
            self.home_axis=1

    def rapid_idle_position(self,axis,dir):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.disable_idle_buttons()
            self.A=0
            self.B=3
            self.C=axis
            self.F=dir #only for python
            if axis == 0:
               self.I=int(1000000/(int(window.b1.toPlainText().strip())*window.d5.value()*2))  #delay time
               self.J=int(window.d1.value()*int(window.b1.toPlainText().strip()))
               self.D=int(self.c2.isChecked()) #Enabled/Disabled
               self.E=int(self.c7.isChecked()) #Direction
               if dir == 1:                    #-X command
                   self.p7.setEnabled(False)
                   if self.E==1:               
                       self.E=0                #Direction
                       self.InvertX = -1
                       print("1")
                   else:
                       self.E=1
                       self.InvertX = 1
                       print("-1")
               else:
                   self.p8.setEnabled(False)
            if axis == 1:
               self.I=int(1000000/(int(window.b2.toPlainText().strip())*window.d6.value()*2))  #delay time
               self.J=int(window.d2.value()*int(window.b2.toPlainText().strip()))
               self.D=int(self.c3.isChecked()) #Enabled/Disabled
               self.E=int(self.c8.isChecked()) #Direction
               if dir == 1:                    #-Y command
                   self.p11.setEnabled(False)
                   if self.E==1:               
                       self.E=0                #Direction
                   else:
                       self.E=1
               else:
                   self.p12.setEnabled(False)
            if axis == 2:
               self.I=int(1000000/(int(window.b3.toPlainText().strip())*window.d7.value()*2))  #delay time
               self.J=int(window.d3.value()*int(window.b3.toPlainText().strip()))
               self.D=int(self.c4.isChecked()) #Enabled/Disabled
               self.E=int(self.c9.isChecked()) #Direction
               if dir == 1:                    #-Z command
                   self.p15.setEnabled(False)
                   if self.E==1:               
                       self.E=0                #Direction
                   else:
                       self.E=1
               else:
                   self.p16.setEnabled(False)
            if axis == 3:
               self.I=int(1000000/(int(window.b4.toPlainText().strip())*window.d8.value()*2))  #delay time
               self.J=int(window.d4.value()*int(window.b4.toPlainText().strip()))
               self.D=int(self.c5.isChecked()) #Enabled/Disabled
               self.E=int(self.c10.isChecked()) #Direction
               if dir == 1:                    #-E command
                   self.p19.setEnabled(False)
                   if self.E==1:               
                       self.E=0                #Direction
                   else:
                       self.E=1
               else:
                   self.p20.setEnabled(False)
            self.rapid_pos=1

    def setNOZZTEMP(self):
        if self.A==0:
           self.disable_idle_buttons()
           self.set_temp=1
        if self.usb_printing==1 and self.USB_CONNECTED==1 and self.froze==0 and self.froze_loop==0:
            self.froze=1
            self.MM=104
            self.SS=float(window.b35.toPlainText().strip())
            self.Message_panel.append(">>> M104 S"+str(self.SS)+": Set NOZZLE temp ")
            self.fly_thread = FLYWorker()
            self.fly_thread.start()            

    def setBEDTEMP(self):
        if self.A==0:
            self.disable_idle_buttons()
            self.set_temp=2
        if self.usb_printing==1 and self.USB_CONNECTED==1 and self.froze==0 and self.froze_loop==0:
            self.froze=1
            self.MM=140
            self.SS=float(window.b36.toPlainText().strip())
            self.Message_panel.append(">>> M140 S"+str(self.SS)+": Set BED temp ")
            self.fly_thread = FLYWorker()
            self.fly_thread.start()
    
    def setFAN1(self):
        if self.A==0:
            self.set_fan=1
        if self.usb_printing==1 and self.USB_CONNECTED==1 and self.froze==0 and self.froze_loop==0:
            self.froze=1
            self.MM=106
            self.SS=float(window.d104.value())
            self.Message_panel.append(">>> M106: Set FAN1 ")
            self.fly_thread = FLYWorker()
            self.fly_thread.start()
       
    def setJFAJ(self):
        if self.usb_printing==1 and self.USB_CONNECTED==1 and self.froze==0 and self.froze_loop==0:
            self.froze=1
            self.MM=0
            self.Message_panel.append(">>> Set JFAJ")
            self.fly_thread = FLYWorker()
            self.fly_thread.start()
 
    def nozz_AUTOTUNE(self):
        self.nozz_auto_tune=1
        self.A=0
        self.B=5
        self.C=1

    def bed_AUTOTUNE(self):
        self.nozz_auto_tune=1
        self.A=0
        self.B=5
        self.C=0

    def openfile(self):#call this function whenever file->open is pressed
        self.Message_panel.append(">>> select GCODE...loading...")
        path = easygui.fileopenbox() #open window to select file and save path
        if path != None:
           self.Message_panel.append(">>> Successfully loaded: "+path)
           self.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.file = open(path) #open this file
           self.data = self.file.read() #read the file
           self.GCODE_Panel.setPlainText(self.data)
           self.file.close() #close the file
           self.file1.write(self.data)
           self.file1.close()
        else:
           self.Message_panel.append(">>> Aborted")

    def save_settings(self):
        bfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\boxes settings.txt','w')
        cfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cboxes settings.txt','w')
        dfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\dboxes settings.txt','w')
        cbfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cbboxes settings.txt','w')
        b_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\boxes settings.txt','w')
        c_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cboxes settings.txt','w')
        d_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\dboxes settings.txt','w')
        cb_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cbboxes settings.txt','w')
        i=0
        for i in range (0,70):
          b = getattr(self, "b{}".format(i)) #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
          text = b.toPlainText().strip() #strip() removes'/n'
          if (text==''): #check if it is aan empty string
              bfile.write("\n")
              b_file.write("\n")
          else:
              bfile.write(text+"\n")
              b_file.write(text+"\n")
        i=1
        for i in range (1,30): #c0-cmax
            c = getattr(self, "c{}".format(i))#self.c[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            check = c.isChecked()
            if check==0:
                cfile.write("0\n")
                c_file.write("0\n")
            else:
               	cfile.write("1\n")
                c_file.write("1\n")                
        i=1
        for i in range (1,9): #c0-cmax
            d = getattr(self, "d{}".format(i))
            value = d.value()
            dfile.write(str(value)+"\n")
            d_file.write(str(value)+"\n")
        i=1
        for i in range (1,4): #c0-cmax
            cb = getattr(self, "comboBox{}".format(i))
            value = cb.currentIndex()
            cbfile.write(str(value)+"\n")
            cb_file.write(str(value)+"\n")
        bfile.close()
        cfile.close()
        dfile.close()
        cbfile.close()
        b_file.close()
        c_file.close()
        d_file.close()
        cb_file.close()
                    

    def load_settings(self):
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\boxes settings.txt','r') #read general setting file and set them
        boxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cboxes settings.txt','r') #read general setting file and set them
        cboxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\dboxes settings.txt','r') #read general setting file and set them
        dboxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cbboxes settings.txt','r') #read general setting file and set them
        cbboxes = file.readlines()
        file.close()
        for i in range (0,70): #b0-bmax
           b = getattr(self, "b{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           b.setPlainText('')
           b.insertPlainText(boxes[i].strip()) #strip() removes'/n'
        for i in range (2,6): #c0-cmax
           c = getattr(self, "c{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           c.setChecked(1)                       #set Enable/Disable cboxes always on at start
        for i in range (7,30): #c0-cmax
           c = getattr(self, "c{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           c.setChecked(int(cboxes[i-1].strip())) #strip() removes'/n'
        for i in range (1,9): #c0-cmax
           d = getattr(self, "d{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           d.setValue(float(dboxes[i-1].strip())) #strip() removes'/n'
        for i in range (1,4): #c0-cmax
           cb = getattr(self, "comboBox{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           cb.setCurrentIndex(int(cbboxes[i-1].strip()))

    def clear_GCODE(self):
        self.data=''
        self.GCODE_Panel.setPlainText('')
        self.Message_panel.append(">>> GCODE cleared")
        #self.import_GCODE(self.data)

    def SD_CARD(self):
        self.Message_panel.append(">>> Mode: SD Printing")
        path = QtWidgets.QFileDialog.getExistingDirectory(self, "Select Directory")
        savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w',encoding="utf-8") #encoding is important
        savepathfile.write(path)
        savepathfile.close()
        self.save_settings()
        if(path != ''):
           self.pipe = win32pipe.CreateNamedPipe(
           r'\\.\pipe\Foo',
           win32pipe.PIPE_ACCESS_DUPLEX,
           win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
           1, 65536, 65536,
           0,
           None)
           self.Message_panel.append(">>> path: " + path)
           self.start_bar()
           self.file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.data = self.GCODE_Panel.toPlainText()
           self.file.write(self.data)
           self.file.close()
           self.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w')
           self.child_file.write(str(struct.pack("i",self.mirror)))
           self.child_file.close()
           p2 = subprocess.Popen("Brain.exe")
        else:
           self.Message_panel.append(">>> Aborted ")

    def PAUSE(self):
        if self.usb_printing==1 and self.froze==0:
           if self.pause_state==0: #pass M226 command to C
              self.MM=226
              self.SS=0
              self.pause_state=1 #paused
              self.froze=1
              self.froze_loop=1
              self.Message_panel.append(">>> M226: Pause request")  
              self.fly_thread = FLYWorker()
              self.fly_thread.start()	
           else:
             self.ser.write(struct.pack("B",self.A)) #trash, to resume printing
             self.pause_state=0 #resume
             self.froze=1
             self.froze_loop=0 
             self.Message_panel.append(">>> Resume printing... ")

    def update_nozz_temp(self, new_nozz_temp):
        self.NOZZ_TEMP.setText("{:.2f}".format(round(new_nozz_temp, 2)))
 
    def update_bed_temp(self, new_bed_temp):
        self.BED_TEMP.setText("{:.2f}".format(round(new_bed_temp, 2)))

    def update_xpos(self, x_pos_report):
        if abs(x_pos_report-self.x_pos_last) > 10000: #Below algorithm handles 16bit overflow arduino position
            if x_pos_report<0:
                 if abs(self.x_overflow) !=1:
                    if abs(self.x_overflow) !=0:
                        self.x_overflow_last = self.x_overflow
                        self.x_overflow = self.x_overflow + 2
                        self.int16_base = 32768
                    else:
                        self.x_overflow_last = self.x_overflow
                        self.x_overflow = self.x_overflow + 1
                        self.int16_base = -32768
                 else:
                    if self.x_overflow_last == 0:
                        self.x_overflow = self.x_overflow + 2
                    else:
                        self.x_overflow = self.x_overflow + 1
                    self.x_overflow_last = self.x_overflow
                    self.int16_base = -32768
            else:
                 if abs(self.x_overflow) !=1:
                    if abs(self.x_overflow) !=0:
                        self.x_overflow = self.x_overflow - 2
                        self.int16_base = -32768
                    else:
                        self.x_overflow = self.x_overflow - 1
                        self.int16_base = 32768
                 else:
                    if self.x_overflow_last == 0:
                        self.x_overflow = self.x_overflow - 2
                    else:
                        self.x_overflow = self.x_overflow - 1
                    self.x_overflow_last = self.x_overflow
                    self.int16_base = 32768
        if (self.x_overflow==0):
            self.int16_base=0
        self.x_pos_last=x_pos_report
        x_pos_report = (self.x_overflow*32768) + (x_pos_report-self.int16_base)
        x_pos_report=self.InvertX*x_pos_report/100 #pulses to mm
        self.XPOSITION.setText("{:.3f}".format(round(x_pos_report, 3)))
        #b.insertPlainText(boxes[i].strip())
        
    def update_ypos(self, y_pos_report):
        if abs(y_pos_report-self.y_pos_last) > 10000: #Below algorithm handles 16bit overflow arduino position
            if y_pos_report<0:
                 if abs(self.y_overflow) !=1:
                    if abs(self.y_overflow) !=0:
                        self.y_overflow_last = self.y_overflow
                        self.y_overflow = self.y_overflow + 2
                        self.int16_base = 32768
                    else:
                        self.y_overflow_last = self.y_overflow
                        self.y_overflow = self.y_overflow + 1
                        self.int16_base = -32768
                 else:
                    if self.y_overflow_last == 0:
                        self.y_overflow = self.y_overflow + 2
                    else:
                        self.y_overflow = self.y_overflow + 1
                    self.y_overflow_last = self.y_overflow
                    self.int16_base = -32768
            else:
                 if abs(self.y_overflow) !=1:
                    if abs(self.y_overflow) !=0:
                        self.y_overflow = self.y_overflow - 2
                        self.int16_base = -32768
                    else:
                        self.y_overflow = self.y_overflow - 1
                        self.int16_base = 32768
                 else:
                    if self.y_overflow_last == 0:
                        self.y_overflow = self.y_overflow - 2
                    else:
                        self.y_overflow = self.y_overflow - 1
                    self.y_overflow_last = self.y_overflow
                    self.int16_base = 32768
        if (self.y_overflow==0):
            self.int16_base=0
        self.y_pos_last=y_pos_report
        y_pos_report = (self.y_overflow*32768) + (y_pos_report-self.int16_base)
        y_pos_report=self.InvertY*y_pos_report/100 #pulses to mm
        #self.Nozz_LCD_3.display(y_pos_report)
        #self.Y_POS.setPlainText(y_pos_report)
        self.YPOSITION.setText("{:.3f}".format(round(y_pos_report, 3)))

    def update_zpos(self, z_pos_report):
        if abs(z_pos_report-self.z_pos_last) > 10000: #Below algorithm handles 16bit overflow arduino position
            if z_pos_report<0:
                 if abs(self.z_overflow) !=1:
                    if abs(self.z_overflow) !=0:
                        self.z_overflow_last = self.z_overflow
                        self.z_overflow = self.z_overflow + 2
                        self.int16_base = 32768
                    else:
                        self.z_overflow_last = self.z_overflow
                        self.z_overflow = self.z_overflow + 1
                        self.int16_base = -32768
                 else:
                    if self.z_overflow_last == 0:
                        self.z_overflow = self.z_overflow + 2
                    else:
                        self.z_overflow = self.z_overflow + 1
                    self.z_overflow_last = self.z_overflow
                    self.int16_base = -32768
            else:
                 if abs(self.z_overflow) !=1:
                    if abs(self.z_overflow) !=0:
                        self.z_overflow = self.z_overflow - 2
                        self.int16_base = -32768
                    else:
                        self.z_overflow = self.z_overflow - 1
                        self.int16_base = 32768
                 else:
                    if self.z_overflow_last == 0:
                        self.z_overflow = self.z_overflow - 2
                    else:
                        self.z_overflow = self.z_overflow - 1
                    self.z_overflow_last = self.z_overflow
                    self.int16_base = 32768
        if (self.z_overflow==0):
            self.int16_base=0
        self.z_pos_last=z_pos_report
        z_pos_report = (self.z_overflow*32768) + (z_pos_report-self.int16_base)
        z_pos_report=self.InvertZ*z_pos_report/100 #pulses to mm
        #self.Nozz_LCD_4.display(z_pos_report)
        #self.Z_POS.setPlainText(z_pos_report)
        self.ZPOSITION.setText("{:.3f}".format(round(z_pos_report, 3)))

    def start_bar(self):
        self.bar_thread = ProgressBarWorker()
        #self.bar_thread.setDaemon(True) #Terminate at the end,only threading.Thread
        self.bar_thread.message.connect(self.print2user_bar) #connect thread to message window
        self.bar_thread.progress_value.connect(self.setProgressVal) #connect thread to bar
        self.bar_thread.start()
       
    def print2user_usb(self,message):
        self.Message_panel.append(message)

    def print2user_bar(self,message):
        self.Message_panel.append(message)

    def setProgressVal(self,progress_value):
        self.progressBar.setValue(progress_value)

    def closeEvent(self, event): #trigger on closing
        self.save_settings() #save settings
        #self.clear_GCODE()

    def selectPort(self,COM):
        self.chosenPort = COM

    def start_COMPort_worker(self):
        self.comport_thread = COMPortWorker()
        #self.comport_thread.setDaemon(True) #Terminate at the end,only threading.Thread
        self.comport_thread.start()

    def start_USB_worker(self):
        self.usb_thread = USBWorker()
        #self.usb_thread.setDaemon(True) #Terminate at the end,only threading.Thread
        self.usb_thread.start()	

if __name__ == "__main__":
    QtWidgets.QApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #https://vicrucann.github.io/tutorials/osg-qt-high-dpi/?fbclid=IwAR3lhrM1zYX615yAGoyoJdAYGdKY5W-l5NsQiWu7gEVoQfzsqWML2iPOWBg
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.assign_buttons()
    window.start_COMPort_worker()
    window.show()
    sys.exit(app.exec_())
    #app.exec()
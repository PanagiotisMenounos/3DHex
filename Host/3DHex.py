#----------------------------------------------------------------------
#      COPYRIGHT NOTICE FOR 3DHex:
#----------------------------------------------------------------------
#
#      3DHex - 3D Printer Firmware
#
#Copyright (c) 2019 Panagiotis Menounos
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
from PyQt5.QtCore import Qt, QThread, pyqtSignal
import time
import os
import easygui
from mainwindow_design import Ui_MainWindow
from multiprocessing import Process #for multiprocessing
import threading
import subprocess
import multiprocessing
import struct
import win32pipe, win32file, pywintypes

class ProgressBarWorker(QThread):
  progress_value = pyqtSignal(int) #define signal
  def run(self):
        print("start bar")
        pipe = win32pipe.CreateNamedPipe(
        r'\\.\pipe\Foo',
        win32pipe.PIPE_ACCESS_DUPLEX,
        win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
        1, 65536, 65536,
        0,
        None)
        try:
            received_progress=0
            win32pipe.ConnectNamedPipe(pipe, None)
            resp = win32file.ReadFile(pipe,4)	
            (received_progress,)=struct.unpack("i",resp[1])
            while received_progress <=100:
                self.progress_value.emit(received_progress) #emit the signal
                resp = win32file.ReadFile(pipe,4)	
                (received_progress,)=struct.unpack("i",resp[1])
                if received_progress==100:
                   self.progress_value.emit(received_progress) #emit the signal
                   received_progress=101
        finally:
              win32file.CloseHandle(pipe)
              print("closed pipe")

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

class TEMPWorker(QThread):
    def run(self):
        while True:
            if window.update_temp==1:
                window.update_temp=0
                if window.usb_printing==1:
                    time.sleep(1)
                window.Nozz_LCD.display(window.nozz_temp)
                window.Bed_LCD.display(window.bed_temp)

class USBWorker(QThread): #This thread starts when 3DHEX connected successfully to the Printer
    def run(self):
        time.sleep(2)
        self.send_buffer() #Set printer to idle mode temp only report
        while True:
            (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12)) #Read temperature
            if window.A==0: #if in idle mode
               window.update_temp=1 #Send signal to TEMPWorker to update temp
               time.sleep(0.2)
               self.check_idle_commands() #Check if any idle command has triggered
            else:
               window.update_temp=1 #Send signal to TEMPWorker to update temp
               time.sleep(0.2)
               self.usb_printing() #Go into USB Print function
               window.A=0 #After USB printing has completed declare idle mode
               window.B=0
               window.C=0
               time.sleep(8)
               self.send_buffer() #Then send idle mode temp only report to printer
               window.enable_idle_buttons()

    def send_buffer(self):
        window.ser.write(struct.pack("8B4H2B",window.A,window.B,window.C,window.D,window.E,window.F,window.G,window.H,window.I,window.J,window.K,window.L,window.M,window.N))
        (pass_fail,)=struct.unpack("B",window.ser.read(1)) #Wait for arduino to confirm everything is ok
        if pass_fail==1: #pass_fail should be 1, else communication has failed
           print("PASS")
        else:
           print("FAILED")

    def usb_printing(self): #USB Printing function 
        window.A=1 #printing mode
        window.usb_printing=0
        serial_command=0 #reset serial command from arduino
        flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
        flag_file.write(struct.pack('2i', 5, 5)) #Write some trash data in order for Brain.C to know Python is in printing function
        flag_file.close()
        flag_py_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag_py.bin',"wb") #Clear flag file
        flag_py_file.close()
        buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin', "wb") #Reset buffer1 binary file
        buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin', "wb") #Reset buffer2 binary file
        buffer1_file.close()
        buffer2_file.close()
        p1 = subprocess.Popen("Brain.exe") #Start Brain.C Proccess 
        flag_py_buffer=0 #Reset flag_py_buffer
        filecase=1 #Read from buffer1 file
        buffer_file_size=3300 #Declare buffer file size (This is max arduino buffer array size until all RAM is full)
        child_buffer_size=1 #Means Brain.C is still running
        while flag_py_buffer==0:#wait for C to fill binary data to buffer1+buffer2 binary files
            (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12)) #Read arduino temp report
            window.update_temp=1 #Send signal to TEMPWorker to update temp
            flag_py_buffer=os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag_py.bin') #Get size of flag_py_file, if 1 then C has fill binary files
        window.ser.flushInput() #very important without delay to fix a bug
        window.ser.flushOutput()#very important without delay to fix a bug
        self.send_buffer() #Command Printer to go into printig mode window.A=1 
        if buffer_file_size==3300 and child_buffer_size!=0 and serial_command!=-260: #Firt time send binary data to Printer
            if filecase==1: #Read from buffer1 binary file
                filecase=2  #Note to read buffer2 next time
                buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin', "rb")
                (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12)) #This first time read buffer1 contains all the necessary settings for Printer
                window.update_temp=1			 
                window.ser.write(buffer1_file.read(3300)) #Send binary data to Printer
                buffer1_file.close() 
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5)) #Write some trash data to tell C that buffer1 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            if filecase==2:
                filecase=1
                buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin', "rb")
                (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12))
                window.update_temp=1
                window.ser.write(buffer2_file.read(3300))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5)) #Write some trash data to tell C that buffer2 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')
        while serial_command!=-10: #Wait for printer to read commanded temp
            (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12))
            window.update_temp=1
        window.usb_printing=1 
        while buffer_file_size==3300 and child_buffer_size!=0 and serial_command!=-260: #Start binary data streaming to Printer 
            if filecase==1:
                filecase=2
                buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin', "rb")
                (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12))
                window.update_temp=1			 
                window.ser.write(buffer1_file.read(3300))
                buffer1_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            elif filecase==2:
                filecase=1
                buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin', "rb")
                (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12))
                window.update_temp=1
                window.ser.write(buffer2_file.read(3300))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\flag.bin',"wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin')
        child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w') #reset child so Brain.C to terminate
        child_file.close()
        savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w') #reset path
        savepathfile.close()
        buffer1_file.close()
        buffer2_file.close()
        window.update_temp=0
        window.usb_printing=0
        window.ser.flushOutput()
        window.ser.flushInput()

    def check_idle_commands(self): #idle mode commands func

            if window.set_temp==1: #1 Set temp
                window.set_temp=0
                window.B=0
                window.C=1
                window.D=0
                window.E=0
                window.F=0
                window.G=int(window.c22.isChecked())
                window.H=int(window.c23.isChecked())
                window.I=int(window.b35.toPlainText().strip())
                window.J=int(window.b36.toPlainText().strip())
                window.K=int(window.b39.toPlainText().strip())
                window.L=int(window.b40.toPlainText().strip())
                window.M=int(window.b37.toPlainText().strip())
                window.N=int(window.b38.toPlainText().strip())
                time.sleep(0.2)
                self.send_buffer()
                window.enable_idle_buttons()
            if window.set_temp==2: #2 Reset temp
                window.set_temp=0
                window.B=0
                window.C=1
                window.D=0
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
                (serial_command,window.nozz_temp,window.bed_temp,)=struct.unpack("3f",window.ser.read(12)) #wait for arduino to end process
                window.update_temp=1
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

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, *args, obj=None, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.declare_vars()
        self.setStyleSheet("QMenu{color: rgb(255, 255, 255);background-color: rgb(63, 63, 63);} QMenuBar{color: rgb(255, 255, 255);background-color: rgb(63, 63, 63);} QMenu::item:selected{background-color: rgb(83, 83, 83);} QMenuBar::item:selected{background-color: rgb(83, 83, 83);}");
        self.actionPrinter2_2.setVisible(False)
    def declare_vars(self):
        self.set_motor=0
        self.USB_CONNECTED=0
        self.usb_connected = 1
        self.set_temp = 0	
        self.home_axis=0
        self.usb_printing=0
        self.rapid_pos=0
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
        self.update_temp=0
        self.chosenPort = ""
        self.ports = serial.tools.list_ports.comports()
        self.comboBox.addItem("")
        for p in self.ports:
            self.comboBox.addItem(p.device)

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
        self.p4.clicked.connect(self.SD_CARD)
        self.p88.clicked.connect(self.setTEMP)
        self.p89.clicked.connect(self.resetTEMP)
        self.p22.clicked.connect(self.clear_GCODE)
        self.action_Open.triggered.connect(self.openfile)
        #self.p2.clicked.connect(self.start_USB_worker)
        self.comboBox.currentTextChanged.connect(self.selectPort)                                                                 #https://zetcode.com/pyqt/qcheckbox/
        self.c2.stateChanged.connect(self.setXmotor)                                                               
        self.c3.stateChanged.connect(self.setYmotor)                                                             
        self.c4.stateChanged.connect(self.setZmotor)                                                             
        self.c5.stateChanged.connect(self.setEmotor)
        self.c12.stateChanged.connect(self.setHOME_Xbuttons)
        self.c13.stateChanged.connect(self.setHOME_Ybuttons)
        self.c14.stateChanged.connect(self.setHOME_Zbuttons)
        self.Default.triggered.connect(self.Printer_Default)
    
    def Printer_Default(self):
        Default = self.sender()
        print('Name: ', Default.text()+"●")
        self.Default.setText(Default.text()+" ●")

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
                self.temp_worker = TEMPWorker()
                self.usb_thread.start()       #Start usb communication handling thread
                self.temp_worker.start()
                self.enable_rapid_buttons()    #Enable home buttons
                for i in range (2,6): #c0-cmax
                     c = getattr(self, "c{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                     c.setChecked(1) 
                self.USB_CONNECTED=1
                print("Success")		
        except:
            print("failed to open")			

    def USB(self): #call this func whenever USB_calculate button is pressed
        self.A=1 #printing mode
        self.usb_printing=1
        self.start_bar()
        self.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
        self.data = self.GCODE_Panel.toPlainText()
        self.file1.write(self.data)
        self.file1.close()
        self.save_settings()
        self.mirror=5
        self.savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w')
        self.savepathfile.close()
        self.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\child.bin','w')
        self.child_file.write(str(struct.pack("i",self.mirror)))
        self.child_file.close()
        self.disable_idle_buttons()

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
          self.p88.setEnabled(False)
          self.p89.setEnabled(False)

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
                   else:
                       self.E=1
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

    def setTEMP(self):
        if self.A==0:
           self.disable_idle_buttons()
           self.set_temp=1

    def resetTEMP(self):
        if self.A==0:
           self.disable_idle_buttons()
           self.set_temp=2

    def openfile(self):#call this function whenever file->open is pressed
        path = easygui.fileopenbox() #open window to select file and save path
        if path != None:
           self.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.file = open(path) #open this file
           self.data = self.file.read() #read the file
           self.GCODE_Panel.setPlainText(self.data)
           self.file.close() #close the file
           self.file1.write(self.data)
           self.file1.close()

    def save_settings(self):
        bfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\boxes settings.txt','w')
        cfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cboxes settings.txt','w')
        dfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\dboxes settings.txt','w')
        i=0
        for i in range (0,53):
          b = getattr(self, "b{}".format(i)) #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
          text = b.toPlainText().strip() #strip() removes'/n'
          if (text==''): #check if it is aan empty string
              bfile.write("\n")
          else:
              bfile.write(text+"\n")
        i=1
        for i in range (1,29): #c0-cmax
            c = getattr(self, "c{}".format(i))#self.c[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            check = c.isChecked()
            if check==0:
                cfile.write("0\n")
            else:
               	cfile.write("1\n")		
        i=1
        for i in range (1,9): #c0-cmax
            d = getattr(self, "d{}".format(i))
            value = d.value()
            dfile.write(str(value)+"\n")
        bfile.close()
        cfile.close()
        dfile.close()

    def load_settings(self):
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\boxes settings.txt','r') #read general setting file and set them
        boxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cboxes settings.txt','r') #read general setting file and set them
        cboxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\dboxes settings.txt','r') #read general setting file and set them
        dboxes = file.readlines()
        file.close()
        for i in range (0,53): #b0-bmax
           b = getattr(self, "b{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           b.insertPlainText(boxes[i].strip()) #strip() removes'/n'
        for i in range (2,6): #c0-cmax
           c = getattr(self, "c{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           c.setChecked(1)                       #set Enable/Disable cboxes always on at start
        for i in range (7,29): #c0-cmax
           c = getattr(self, "c{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           c.setChecked(int(cboxes[i-1].strip())) #strip() removes'/n'
        for i in range (1,9): #c0-cmax
           d = getattr(self, "d{}".format(i))    #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           d.setValue(float(dboxes[i-1].strip())) #strip() removes'/n'

    def clear_GCODE(self):
        self.data=''
        self.GCODE_Panel.setPlainText('')
        #self.import_GCODE(self.data)

    def SD_CARD(self):
        path = QtWidgets.QFileDialog.getExistingDirectory(self, "Select Directory")
        savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\savepath.txt','w',encoding="utf-8") #encoding is important
        savepathfile.write(path)
        savepathfile.close()
        if(path != ''):
           self.file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.data = self.GCODE_Panel.toPlainText()
           self.file.write(self.data)
           self.file.close()
           #self.start_bar()
           p2 = subprocess.Popen("Brain.exe")

    def start_bar(self):
        self.bar_thread = ProgressBarWorker()
        #self.bar_thread.setDaemon(True) #Terminate at the end,only threading.Thread
        self.bar_thread.progress_value.connect(self.setProgressVal) #connect thread to bar
        self.bar_thread.start()

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


app = QtWidgets.QApplication(sys.argv)
window = MainWindow()
window.assign_buttons()
window.load_settings()
window.start_COMPort_worker()
window.show()
sys.exit(app.exec_())
#app.exec()
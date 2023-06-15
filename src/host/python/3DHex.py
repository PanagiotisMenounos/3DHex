#----------------------------------------------------------------------
#      COPYRIGHT NOTICE FOR 3DHex:
#----------------------------------------------------------------------
#
#      3DHex - 3D Printer Firmware
#
#Copyright (c) 2023 Panagiotis Menounos
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
from popup.pins_library import PinsWindow

from popup.autopid_update import AutoTuneWindow
from ui.mainwindow_ui import Ui_MainWindow
from mainwindow_controller.graphics.progress_bar import ProgressBarWorker
from serial_controller.comport_scanner import COMPortScanner
from printer_controller.fly_commander import FLYWorker
from calibration.auto_bed_leveling import ABL_Interpolation
from serial_controller.usb_connect_printer import UsbConnect
from printer_controller.rapid_controls.axes_controls import RapidAxesController
from printer_controller.rapid_controls.heat_controls import TempConctrols
from mainwindow_controller.printers_config import PrintersConfig

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, *args, obj=None, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.Message_panel.append(">>> 3DHex")
        self.printconfigurations = PrintersConfig(self)
        self.declare_vars()
        self.printconfigurations.load_printers()
        self.setStyleSheet("QMenu{color: rgb(255, 255, 255);background-color: rgb(47, 47, 47);} QMenuBar{color: rgb(255, 255, 255);background-color: rgb(47, 47, 47);} QMenu::item:selected{background-color: rgb(83, 83, 83);} QMenuBar::item:selected{background-color: rgb(83, 83, 83);}")
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
        self.setup_temp_monitor()
        self.usbconnect = UsbConnect(self) #import usb connect
        self.rapidmovecontrol = RapidAxesController(self)
        self.tempidlecontrol = TempConctrols(self)


    def setup_temp_monitor(self):
        length_of_signal = 50
        self.t = np.linspace(0,1,length_of_signal)
        
        self.cosinus_signal = np.zeros(length_of_signal)
        self.sinus_signal = np.zeros(length_of_signal)

        self.MplWidget.canvas.axes.clear()
        self.MplWidget.canvas.axes.plot(self.t, self.cosinus_signal, color="#ff5500")
        self.MplWidget.canvas.axes.plot(self.t, self.sinus_signal, color="#0095ff")
        self.MplWidget.canvas.draw()
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
        self.rapid_X=0
        self.rapid_Y=0
        self.rapid_Z=0
        self.nozz_auto_tune=0
        self.InvertX_tongle=1
        self.InvertY_tongle=1
        self.InvertZ_tongle=1
        self.bl_toggle=0
        self.BL_TOUCH_STATE=0
        self.ABL_INTERPOLATION_TYPE=0
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
        self.U=0
        self.V=0
        self.W=0
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
        self.stepx_direction=1
        self.stepy_direction=1
        self.stepz_direction=1
        self.stepx_pos=0
        self.stepy_pos=0
        self.stepz_pos=0
        self.x_overflow=0
        self.sum_Xpos=0
        self.sum_Ypos=0
        self.sum_Zpos=0
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
    def select_HW_pin(self,button):
        self.pin_button=button
        try:
            self.pin_conf = int(getattr(self, "Pin_Button_{}".format(button)).text())
        except:
            self.pin_conf = 70
        self.pins_window = PinsWindow(self)
        self.pins_window.show()

    def test(self):
        self.window3 = AutoTuneWindow(self)
        self.window3.show()
        print(self.Auto_P)

    def assign_buttons(self):
        self.p1.clicked.connect(self.usbconnect.CONNECT)
        self.p3.clicked.connect(self.USB)
        self.p5.clicked.connect(self.rapidmovecontrol.HOME_X_MIN)
        self.p9.clicked.connect(self.rapidmovecontrol.HOME_Y_MIN)
        self.p13.clicked.connect(self.rapidmovecontrol.HOME_Z_MIN)
        self.p7.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(0,1))
        self.p8.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(0,0))
        self.p11.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(1,1))
        self.p12.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(1,0))
        self.p15.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(2,1))
        self.p16.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(2,0))
        self.p19.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(3,1))
        self.p20.clicked.connect(lambda:self.rapidmovecontrol.rapid_idle_position(3,0))
        self.p21.clicked.connect(self.PAUSE)
        self.p4.clicked.connect(self.SD_CARD)
        self.p88.clicked.connect(self.tempidlecontrol.setNOZZTEMP)
        self.p89.clicked.connect(self.tempidlecontrol.setBEDTEMP)
        self.p22.clicked.connect(self.clear_GCODE)
        self.p23.clicked.connect(self.CANCEL)
        self.p24.clicked.connect(self.setJFAJ)
        self.p25.clicked.connect(self.tempidlecontrol.setFAN1)
        self.p90.clicked.connect(self.nozz_AUTOTUNE)
        self.p91.clicked.connect(self.bed_AUTOTUNE)
        self.p29.clicked.connect(self.View)
        self.p30.clicked.connect(lambda:self.BL_TOUCH_TOGGLE(0))
        self.p31.clicked.connect(lambda:self.BL_TOUCH_TOGGLE(1))
        self.p27.clicked.connect(self.execute_ABL)
        self.action_Open.triggered.connect(self.openfile)
        self.comboBox.currentTextChanged.connect(self.selectPort)#https://zetcode.com/pyqt/qcheckbox/
        self.c2.stateChanged.connect(self.rapidmovecontrol.setXmotor)
        self.c3.stateChanged.connect(self.rapidmovecontrol.setYmotor)
        self.c4.stateChanged.connect(self.rapidmovecontrol.setZmotor)
        self.c5.stateChanged.connect(self.rapidmovecontrol.setEmotor)
        self.c7.stateChanged.connect(self.readInvertX)
        self.c8.stateChanged.connect(self.readInvertY)
        self.c9.stateChanged.connect(self.readInvertZ)
        self.c12.stateChanged.connect(self.setHOME_Xbuttons)
        self.c13.stateChanged.connect(self.setHOME_Ybuttons)
        self.c14.stateChanged.connect(self.setHOME_Zbuttons)
        self.c21.stateChanged.connect(self.UNITS)
        self.c29.stateChanged.connect(self.ABL_include)
        self.actionPrinter0.triggered.connect(lambda:self.printconfigurations.select_printer(0))
        self.actionPrinter1.triggered.connect(lambda:self.printconfigurations.select_printer(1))
        self.actionPrinter2.triggered.connect(lambda:self.printconfigurations.select_printer(2))
        self.actionPrinter3.triggered.connect(lambda:self.printconfigurations.select_printer(3))
        self.actionPrinter4.triggered.connect(lambda:self.printconfigurations.select_printer(4))
        self.actionPrinter5.triggered.connect(lambda:self.printconfigurations.select_printer(5))
        self.actionPrinter6.triggered.connect(lambda:self.printconfigurations.select_printer(6))
        self.actionPrinter7.triggered.connect(lambda:self.printconfigurations.select_printer(7))
        self.actionPrinter8.triggered.connect(lambda:self.printconfigurations.select_printer(8))
        self.actionPrinter9.triggered.connect(lambda:self.printconfigurations.select_printer(9))
        self.actionPrinter10.triggered.connect(lambda:self.printconfigurations.select_printer(10))
        self.actionPrinter11.triggered.connect(lambda:self.printconfigurations.select_printer(11))
        self.actionPrinter12.triggered.connect(lambda:self.printconfigurations.select_printer(12))
        self.actionPrinter13.triggered.connect(lambda:self.printconfigurations.select_printer(13))
        self.actionPrinter14.triggered.connect(lambda:self.printconfigurations.select_printer(14))
        self.actionPrinter15.triggered.connect(lambda:self.printconfigurations.select_printer(15))
        self.actionPrinter16.triggered.connect(lambda:self.printconfigurations.select_printer(16))
        self.actionPrinter17.triggered.connect(lambda:self.printconfigurations.select_printer(17))
        self.actionPrinter18.triggered.connect(lambda:self.printconfigurations.select_printer(18))
        self.actionPrinter19.triggered.connect(lambda:self.printconfigurations.select_printer(19))
        self.actionPrinter20.triggered.connect(lambda:self.printconfigurations.select_printer(20))
        self.actionPrinter21.triggered.connect(lambda:self.printconfigurations.select_printer(21))
        self.actionPrinter22.triggered.connect(lambda:self.printconfigurations.select_printer(22))
        self.actionPrinter23.triggered.connect(lambda:self.printconfigurations.select_printer(23))
        self.actionPrinter24.triggered.connect(lambda:self.printconfigurations.select_printer(24))
        self.Pin_Button_0.clicked.connect(lambda:self.select_HW_pin(0))
        self.Pin_Button_1.clicked.connect(lambda:self.select_HW_pin(1))
        self.Pin_Button_2.clicked.connect(lambda:self.select_HW_pin(2))
        self.Pin_Button_3.clicked.connect(lambda:self.select_HW_pin(3))
        self.Pin_Button_4.clicked.connect(lambda:self.select_HW_pin(4))
        self.Pin_Button_5.clicked.connect(lambda:self.select_HW_pin(5))
        self.Pin_Button_6.clicked.connect(lambda:self.select_HW_pin(6))
        self.Pin_Button_7.clicked.connect(lambda:self.select_HW_pin(7))
        self.Pin_Button_8.clicked.connect(lambda:self.select_HW_pin(8))
        self.Pin_Button_9.clicked.connect(lambda:self.select_HW_pin(9))
        self.Pin_Button_10.clicked.connect(lambda:self.select_HW_pin(10))
        self.Pin_Button_11.clicked.connect(lambda:self.select_HW_pin(11))
        self.Pin_Button_12.clicked.connect(lambda:self.select_HW_pin(12))
        self.Pin_Button_13.clicked.connect(lambda:self.select_HW_pin(13))
        self.Pin_Button_14.clicked.connect(lambda:self.select_HW_pin(14))
        self.Pin_Button_15.clicked.connect(lambda:self.select_HW_pin(15))
        self.Pin_Button_16.clicked.connect(lambda:self.select_HW_pin(16))
        self.Pin_Button_17.clicked.connect(lambda:self.select_HW_pin(17))
        self.Pin_Button_18.clicked.connect(lambda:self.select_HW_pin(18))
        self.Pin_Button_19.clicked.connect(lambda:self.select_HW_pin(19))
        self.Pin_Button_20.clicked.connect(lambda:self.select_HW_pin(20))
        self.Pin_Button_21.clicked.connect(lambda:self.select_HW_pin(21))
        self.Pin_Button_22.clicked.connect(lambda:self.select_HW_pin(22))
        self.Pin_Button_23.clicked.connect(lambda:self.select_HW_pin(23))
        self.Pin_Button_24.clicked.connect(lambda:self.select_HW_pin(24))
        self.Pin_Button_25.clicked.connect(lambda:self.select_HW_pin(25))
        self.Pin_Button_26.clicked.connect(lambda:self.select_HW_pin(26))
        self.Pin_Button_27.clicked.connect(lambda:self.select_HW_pin(27))
        self.Pin_Button_28.clicked.connect(lambda:self.select_HW_pin(28))
        self.Pin_Button_29.clicked.connect(lambda:self.select_HW_pin(29))
        self.Pin_Button_30.clicked.connect(lambda:self.select_HW_pin(30))
        self.Pin_Button_31.clicked.connect(lambda:self.select_HW_pin(31))
        self.Pin_Button_32.clicked.connect(lambda:self.select_HW_pin(32))
        self.Pin_Button_33.clicked.connect(lambda:self.select_HW_pin(33))
        self.Pin_Button_34.clicked.connect(lambda:self.select_HW_pin(34))
        self.Pin_Button_35.clicked.connect(lambda:self.select_HW_pin(35))
        self.Pin_Button_36.clicked.connect(lambda:self.select_HW_pin(36))
        self.Pin_Button_37.clicked.connect(lambda:self.select_HW_pin(37))
        self.Pin_Button_38.clicked.connect(lambda:self.select_HW_pin(38))
        self.Pin_Button_39.clicked.connect(lambda:self.select_HW_pin(39))
        # self.Pin_Button_40.clicked.connect(lambda:self.select_HW_pin(40))
        # self.Pin_Button_41.clicked.connect(lambda:self.select_HW_pin(41))
        # self.Pin_Button_42.clicked.connect(lambda:self.select_HW_pin(42))
        # self.Pin_Button_43.clicked.connect(lambda:self.select_HW_pin(43))
        # self.Pin_Button_44.clicked.connect(lambda:self.select_HW_pin(44))
        # self.Pin_Button_45.clicked.connect(lambda:self.select_HW_pin(45))
        # self.Pin_Button_46.clicked.connect(lambda:self.select_HW_pin(46))
        # self.Pin_Button_47.clicked.connect(lambda:self.select_HW_pin(47))
        self.actionNew.triggered.connect(self.printconfigurations.new_printer)
        self.actionRemove.triggered.connect(self.printconfigurations.remove_printer)
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
                gcode_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
                gcode = self.GCODE_Panel.toPlainText()
                gcode_file.write(gcode)
                gcode_file.close()
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
        self.abl_type = window.comboBox4.currentIndex()
        
        centerX = (self.width/2.0)-self.xtool_offset
        centerY = (self.length/2.0)-self.ytool_offset
        stepx = (self.Xend-self.Xstart)/(self.columns-1)
        stepy = (self.Yend-self.Ystart)/(self.rows-1)
        
        gpsx = self.Xstart
        gpsy = self.Ystart
        with open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w') as ABL_f:
            if self.abl_type == 2: #MESH Interpolation
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
                ABL_f.write('G2929'+'\n')
                ABL_f.write('G92 X'+str(centerX)+' Y'+str(centerY)+'\n')
                ABL_f.write('G1 X0 Y0'+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G1 Z0'+' F'+str(int(self.Z_Feed))+'\n')
                window.m1.setPlainText('MESH')
            elif self.abl_type == 1: #PLANE Interpolation
                ABL_f.write('G2929'+'\n')
                ABL_f.write('G1 Z'+str(safez)+' F'+str(int(self.Z_Feed))+'\n')
                ABL_f.write('G1 X'+str(centerX)+' Y'+str(centerY)+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G92 X'+str(self.width/2.0)+' Y'+str(self.length/2.0)+'\n')
                ABL_f.write('G28 Z'+'\n')
                ABL_f.write('G1 X'+str(self.Xstart)+' Y'+str(self.Ystart)+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G28 Z'+'\n')
                ABL_f.write('G1 X'+str(self.Xend)+' Y'+str(self.Ystart)+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G28 Z'+'\n')
                ABL_f.write('G1 X'+str((self.Xend+self.Xstart)/2.0)+' Y'+str(self.Yend)+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G28 Z'+'\n')
                ABL_f.write('G1 X'+str(self.width/2.0)+' Y'+str(self.length/2.0)+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G2929'+'\n')
                ABL_f.write('G92 X'+str(centerX)+' Y'+str(centerY)+'\n')
                ABL_f.write('G1 X0 Y0'+' F'+str(int(self.XY_Feed))+'\n')
                ABL_f.write('G1 Z0'+' F'+str(int(self.Z_Feed))+'\n')
                window.m1.setPlainText('PLANE')
            elif self.abl_type == 0:
                window.m1.setPlainText('NULL')
        ABL_f.close()
        self.ABL=1
        self.USB()
    def View(self):         
        if pyplot.fignum_exists(self.plot_num)==False:
            bedx = float(window.b53.toPlainText().strip())
            bedy = float(window.b54.toPlainText().strip())
            grid = float(window.b66.toPlainText().strip())
            interpolation = 1
            if grid < 1:
                interpolation = 0
                self.Message_panel.append(">>> Invalid grid value: <0")
            if grid % 1 != 0:
                interpolation = 0
                self.Message_panel.append(">>> Invalid grid value: float")
            if bedx % grid !=0:
                interpolation = 0
                self.Message_panel.append(">>> Invalid grid value: bed x_size modulus grid ")
            if bedy % grid != 0:
                interpolation = 0
                self.Message_panel.append(">>> Invalid grid value: bed y_size modulus grid ")
            if interpolation == 1:
                self.ABL_interpolation_thread=ABL_Interpolation(self)
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
        fig.colorbar(surf, shrink=0.4, aspect=20, pad=0.1)
        fig.tight_layout()
        pyplot.ion()
        pyplot.show()
    def readInvertX(self):
        self.InvertX_tongle=1
    def readInvertY(self):
        self.InvertY_tongle=1
    def readInvertZ(self):
        self.InvertZ_tongle=1
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
        if self.c13.isChecked():   
            self.p9.setEnabled(True)
        if self.c14.isChecked():  
            self.p13.setEnabled(True)
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
        self.p9.setEnabled(False)
        self.p13.setEnabled(False)
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
        else:
            self.p5.setEnabled(False) 
    def setHOME_Ybuttons(self,state):
        if state == Qt.Checked and self.USB_CONNECTED==1:
            self.p9.setEnabled(True)
        else:
            self.p9.setEnabled(False)
    def setHOME_Zbuttons(self,state):
        if state == Qt.Checked and self.USB_CONNECTED==1:
            self.p13.setEnabled(True)
        else:
            self.p13.setEnabled(False)

    def BL_TOUCH_TOGGLE(self,case):
        if self.home_axis==0 and self.rapid_pos==0 and self.A==0:
            self.bl_toggle=1
            self.A=0
            self.B=7
            if case==0: #toggle button
                if self.BL_TOUCH_STATE==0: #UP
                    self.C=1
                    self.BL_TOUCH_STATE=1
                else:
                    self.C=2
                    self.BL_TOUCH_STATE=0
            else: #test button
                self.C=0


    def setJFAJ(self):
        if self.usb_printing==1 and self.USB_CONNECTED==1 and self.froze==0 and self.froze_loop==0:
            self.froze=1
            self.MM=0
            self.Message_panel.append(">>> Set JFAJ")
            self.fly_thread = FLYWorker(self)
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
           #self.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.file = open(path) #open this file
           self.data = self.file.read() #read the file
           self.GCODE_Panel.setPlainText(self.data)
           self.file.close() #close the file
           #self.file1.write(self.data)
           #self.file1.close()
          
           
        else:
           self.Message_panel.append(">>> Aborted")
    def save_settings(self):
        bfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\boxes settings.txt','w')
        cfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cboxes settings.txt','w')
        dfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\dboxes settings.txt','w')
        cbfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cbboxes settings.txt','w')
        pinsfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\pins settings.txt','w')
        b_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\boxes settings.txt','w')
        c_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cboxes settings.txt','w')
        d_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\dboxes settings.txt','w')
        cb_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cbboxes settings.txt','w')
        pins_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\pins settings.txt','w')
        i=0
        for i in range (0,71):
            try:
                b = getattr(self, "b{}".format(i)) #self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                text = b.toPlainText().strip() #strip() removes'/n'
                if (text==''): #check if it is aan empty string
                  bfile.write("\n")
                  b_file.write("\n")
                else:
                  bfile.write(text+"\n")
                  b_file.write(text+"\n")
            except:
                print("b")
        i=1
        for i in range (1,30): #c0-cmax
            try:
                c = getattr(self, "c{}".format(i))#self.c[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                check = c.isChecked()
                if check==0:
                    cfile.write("0\n")
                    c_file.write("0\n")
                else:
                    cfile.write("1\n")
                    c_file.write("1\n")
            except:
                print("c")
        i=1
        for i in range (1,9): #c0-cmax
            try:
                d = getattr(self, "d{}".format(i))
                value = d.value()
                dfile.write(str(value)+"\n")
                d_file.write(str(value)+"\n")
            except:
                print("d")
        i=1
        for i in range (1,4): #c0-cmax
            try:
                cb = getattr(self, "comboBox{}".format(i))
                value = cb.currentIndex()
                cbfile.write(str(value)+"\n")
                cb_file.write(str(value)+"\n")
            except:
                print("cb")
        i=0
        for i in range (0,40):
            try:
                pin = getattr(self, "Pin_Button_{}".format(i))
                value = pin.text()
                if value == 'N':
                    pinsfile.write("100\n")
                    pins_file.write("100\n")
                else:
                    pinsfile.write(str(value)+"\n")
                    pins_file.write(str(value)+"\n")
            except:
                print("pin")
        bfile.close()
        cfile.close()
        dfile.close()
        cbfile.close()
        pinsfile.close()
        b_file.close()
        c_file.close()
        d_file.close()
        cb_file.close()
        pins_file.close()
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
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\pins settings.txt','r') #read general setting file and set them
        pins = file.readlines()
        file.close()
        try:
            file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.printer) + '\\abl_type.txt','r') #read general setting file and set them
            abldata = file.readlines()
            self.ABL_INTERPOLATION_TYPE = int(abldata[0].strip())
            if self.ABL_INTERPOLATION_TYPE==1:
                self.m1.setPlainText('PLANE')
            elif self.ABL_INTERPOLATION_TYPE==2:
                self.m1.setPlainText('MESH')
            elif self.ABL_INTERPOLATION_TYPE==0:
                self.m1.setPlainText('NO DATA')
        except:
            self.m1.setPlainText('NO DATA')
        for i in range (0,71): #b0-bmax
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
        for i in range (0,40): #c0-cmax
            pin = getattr(self, "Pin_Button_{}".format(i))
            val = int(pins[i].strip())
            if val == 100:
                pin.setText('N')
            else:
                pin.setText(str(val))
    def clear_GCODE(self):
        self.data=''
        self.GCODE_Panel.setPlainText('')
        self.Message_panel.append(">>> GCODE cleared")
        #self.import_GCODE(self.data)
    def SD_CARD(self):
        gcode_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
        gcode = self.GCODE_Panel.toPlainText()
        gcode_file.write(gcode)
        gcode_file.close()
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
           p2 = subprocess.Popen("3DBrain.exe")
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
              self.fly_thread = FLYWorker(self)
              self.fly_thread.start()	
           else:
             self.ser.write(struct.pack("B",self.A)) #trash, to resume printing
             self.pause_state=0 #resume
             self.froze=1
             self.froze_loop=0 
             self.Message_panel.append(">>> Resume printing... ")
    def update_nozz_temp(self, new_nozz_temp):
        self.NOZZ_TEMP.setText("{:.2f}".format(round(new_nozz_temp, 2)))
        self.cosinus_signal[49]= float("{:.2f}".format(round(new_nozz_temp, 2)))
    def update_bed_temp(self, new_bed_temp):
        self.BED_TEMP.setText("{:.2f}".format(round(new_bed_temp, 2)))
        self.sinus_signal[49]= float("{:.2f}".format(round(new_bed_temp, 2)))
        self.MplWidget.canvas.axes.clear()
        self.MplWidget.canvas.axes.plot(self.t, self.cosinus_signal,color="#ff5500")
        self.MplWidget.canvas.axes.plot(self.t, self.sinus_signal, color="#0095ff")
        self.MplWidget.canvas.axes.set_ylim(bottom=0)
        self.MplWidget.canvas.draw()
        self.cosinus_signal= np.roll(self.cosinus_signal, -1)
        self.sinus_signal = np.roll(self.sinus_signal, -1)
    def update_xpos(self, x_pos_report):
        self.stepx_pos = x_pos_report - self.x_pos_last    
        if abs(self.stepx_pos) > 15000: #Below algorithm handles 16bit overflow arduino position
            if self.stepx_pos<0:
                self.stepx_pos = (65536-self.x_pos_last) + x_pos_report
            else:
                self.stepx_pos = (x_pos_report -65536) - self.x_pos_last
        if self.rapid_X != 0 and self.stepx_pos!=0 and self.InvertX_tongle==1:
            if self.rapid_X>0 and self.stepx_pos<0:
                window.Message_panel.append(">>> X Position calibrated")
                self.InvertX_tongle=0
                self.stepx_direction=-1
            else:
                if self.rapid_X<0 and self.stepx_pos>0:
                    window.Message_panel.append(">>> X Position calibrated")
                    self.stepx_direction=-1
                    self.InvertX_tongle=0
                else:
                    window.Message_panel.append(">>> X Position calibrated")
                    self.stepx_direction=1
                    self.InvertX_tongle=0
            self.rapid_X=0
        self.stepx_pos=self.stepx_pos*self.stepx_direction
        self.x_pos_last=x_pos_report
        self.sum_Xpos = self.sum_Xpos + self.stepx_pos
        self.x_overflow=self.sum_Xpos/100 #pulses to mm
        self.XPOSITION.setText("{:.3f}".format(round(self.x_overflow, 3)))
    def update_ypos(self, y_pos_report):
        self.stepy_pos = y_pos_report - self.y_pos_last
        if abs(self.stepy_pos) > 15000: #Below algorithm handles 16bit overflow arduino position
            if self.stepy_pos<0:
                self.stepy_pos = (65536-self.y_pos_last) + y_pos_report
            else:
                self.stepy_pos = (y_pos_report -65536) - self.y_pos_last
        if self.rapid_Y != 0 and self.stepy_pos!=0 and self.InvertY_tongle==1:
            if self.rapid_Y>0 and self.stepy_pos<0:
                window.Message_panel.append(">>> Y Position calibrated")
                self.InvertY_tongle=0
                self.stepy_direction=-1
            else:
                if self.rapid_Y<0 and self.stepy_pos>0:
                    window.Message_panel.append(">>> Y Position calibrated")
                    self.stepy_direction=-1
                    self.InvertY_tongle=0
                else:
                    window.Message_panel.append(">>> Y Position calibrated")
                    self.stepy_direction=1
                    self.InvertΥ_tongle=0
            self.rapid_Y=0
        self.stepy_pos=self.stepy_pos*self.stepy_direction
        self.y_pos_last=y_pos_report
        self.sum_Ypos = self.sum_Ypos + self.stepy_pos
        self.y_overflow=self.sum_Ypos/100 #pulses to mm
        self.YPOSITION.setText("{:.3f}".format(round(self.y_overflow, 3)))
    def update_zpos(self, z_pos_report):
        self.stepz_pos = z_pos_report - self.z_pos_last
        if abs(self.stepz_pos) > 15000: #Below algorithm handles 16bit overflow arduino position
            if self.stepz_pos<0:
                self.stepz_pos = (65536-self.z_pos_last) + z_pos_report
            else:
                self.stepz_pos = (z_pos_report -65536) - self.z_pos_last
        if self.rapid_Z != 0 and self.stepz_pos!=0 and self.InvertZ_tongle==1:
            if self.rapid_Z>0 and self.stepz_pos<0:
                window.Message_panel.append(">>> Z Position calibrated")
                self.InvertZ_tongle=0
                self.stepz_direction=-1
            else:
                if self.rapid_Z<0 and self.stepz_pos>0:
                    window.Message_panel.append(">>> Z Position calibrated")
                    self.stepz_direction=-1
                    self.InvertZ_tongle=0
                else:
                    window.Message_panel.append(">>> Z Position calibrated")
                    self.stepz_direction=1
                    self.InvertZ_tongle=0
            self.rapid_Z=0
        self.stepz_pos=self.stepz_pos*self.stepz_direction
        self.z_pos_last=z_pos_report
        self.sum_Zpos = self.sum_Zpos + self.stepz_pos
        self.z_overflow=self.sum_Zpos/100 #pulses to mm
        self.ZPOSITION.setText("{:.3f}".format(round(self.z_overflow, 3)))
    def start_bar(self):
        self.bar_thread = ProgressBarWorker(self)
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
        self.comport_thread = COMPortScanner(self)
        #self.comport_thread.setDaemon(True) #Terminate at the end,only threading.Thread
        self.comport_thread.start()

if __name__ == "__main__":
    QtWidgets.QApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #https://vicrucann.github.io/tutorials/osg-qt-high-dpi/?fbclid=IwAR3lhrM1zYX615yAGoyoJdAYGdKY5W-l5NsQiWu7gEVoQfzsqWML2iPOWBg
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.assign_buttons()
    window.start_COMPort_worker()
    window.show()
    sys.exit(app.exec_())
    #app.exec()
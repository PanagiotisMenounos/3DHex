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



class WidgetManager:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def setup_temp_monitor(self):
        length_of_signal = 50
        self.main_window.t = np.linspace(0, 1, length_of_signal)

        self.main_window.cosinus_signal = np.zeros(length_of_signal)
        self.main_window.sinus_signal = np.zeros(length_of_signal)

        self.main_window.MplWidget.canvas.axes.clear()
        self.main_window.MplWidget.canvas.axes.plot(self.main_window.t, self.main_window.cosinus_signal, color="#ff5500")
        self.main_window.MplWidget.canvas.axes.plot(self.main_window.t, self.main_window.sinus_signal, color="#0095ff")
        self.main_window.MplWidget.canvas.draw()

    def UNITS(self):
        if self.main_window.c21.isChecked() == True:
            self.main_window.c21.setText("[mm/min]")
        else:
            self.main_window.c21.setText("[mm/sec]")
    def ABL_include(self):
        if self.main_window.c29.isChecked() == True:
            self.main_window.p27.setEnabled(False)
        else:
            self.main_window.p27.setEnabled(True)

    def Plot(self,message):
        plt.style.use('dark_background')
        fig = pyplot.figure()
        self.main_window.plot_num=fig.number
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

    def enable_idle_buttons(self): #Enable after idle command
        self.main_window.c2.setEnabled(True)
        self.main_window.c3.setEnabled(True)
        self.main_window.c4.setEnabled(True)
        self.main_window.c12.setEnabled(True)
        self.main_window.c13.setEnabled(True)
        self.main_window.c14.setEnabled(True)
        self.main_window.c5.setEnabled(True)
        self.main_window.p88.setEnabled(True)
        self.main_window.p89.setEnabled(True)

    def disable_idle_buttons(self): #Disable while in idle command
       if self.main_window.USB_CONNECTED==1:
          self.main_window.c2.setEnabled(False)
          self.main_window.c3.setEnabled(False)
          self.main_window.c4.setEnabled(False)
          self.main_window.c12.setEnabled(False)
          self.main_window.c13.setEnabled(False)
          self.main_window.c14.setEnabled(False)
          self.main_window.c5.setEnabled(False)
          #self.main_window.p88.setEnabled(False)
          #self.main_window.p89.setEnabled(False)

    def enable_rapid_buttons(self):
        if self.main_window.c12.isChecked():
            self.main_window.p5.setEnabled(True)
        if self.main_window.c13.isChecked():
            self.main_window.p9.setEnabled(True)
        if self.main_window.c14.isChecked():
            self.main_window.p13.setEnabled(True)
        self.main_window.p7.setEnabled(True)
        self.main_window.p8.setEnabled(True)
        self.main_window.p11.setEnabled(True)
        self.main_window.p12.setEnabled(True)
        self.main_window.p15.setEnabled(True)
        self.main_window.p16.setEnabled(True)
        self.main_window.p19.setEnabled(True)
        self.main_window.p20.setEnabled(True)
    def disable_rapid_buttons(self):
        self.main_window.p5.setEnabled(False)
        self.main_window.p9.setEnabled(False)
        self.main_window.p13.setEnabled(False)
        self.main_window.p7.setEnabled(False)
        self.main_window.p8.setEnabled(False)
        self.main_window.p11.setEnabled(False)
        self.main_window.p12.setEnabled(False)
        self.main_window.p15.setEnabled(False)
        self.main_window.p16.setEnabled(False)
        self.main_window.p19.setEnabled(False)
        self.main_window.p20.setEnabled(False)

    def setHOME_Xbuttons(self,state):
        if state == Qt.Checked and self.main_window.c2.isChecked() and self.main_window.USB_CONNECTED==1:
            self.main_window.p5.setEnabled(True)
        else:
            self.main_window.p5.setEnabled(False)
    def setHOME_Ybuttons(self,state):
        if state == Qt.Checked and self.main_window.USB_CONNECTED==1:
            self.main_window.p9.setEnabled(True)
        else:
            self.main_window.p9.setEnabled(False)
    def setHOME_Zbuttons(self,state):
        if state == Qt.Checked and self.main_window.USB_CONNECTED==1:
            self.main_window.p13.setEnabled(True)
        else:
            self.main_window.p13.setEnabled(False)

    def clear_GCODE(self):
        self.main_window.data=''
        self.main_window.GCODE_Panel.setPlainText('')
        self.main_window.Message_panel.append(">>> GCODE cleared")

    def update_nozz_temp(self, new_nozz_temp):
        self.main_window.NOZZ_TEMP.setText("{:.2f}".format(round(new_nozz_temp, 2)))
        self.main_window.cosinus_signal[49]= float("{:.2f}".format(round(new_nozz_temp, 2)))

    def update_bed_temp(self, new_bed_temp):
        self.main_window.BED_TEMP.setText("{:.2f}".format(round(new_bed_temp, 2)))
        self.main_window.sinus_signal[49]= float("{:.2f}".format(round(new_bed_temp, 2)))
        self.main_window.MplWidget.canvas.axes.clear()
        self.main_window.MplWidget.canvas.axes.plot(self.main_window.t, self.main_window.cosinus_signal,color="#ff5500")
        self.main_window.MplWidget.canvas.axes.plot(self.main_window.t, self.main_window.sinus_signal, color="#0095ff")
        self.main_window.MplWidget.canvas.axes.set_ylim(bottom=0)
        self.main_window.MplWidget.canvas.draw()
        self.main_window.cosinus_signal= np.roll(self.main_window.cosinus_signal, -1)
        self.main_window.sinus_signal = np.roll(self.main_window.sinus_signal, -1)

    def update_xpos(self, x_pos_report):
        self.main_window.stepx_pos = x_pos_report - self.main_window.x_pos_last
        if abs(self.main_window.stepx_pos) > 15000: #Below algorithm handles 16bit overflow arduino position
            if self.main_window.stepx_pos<0:
                self.main_window.stepx_pos = (65536-self.main_window.x_pos_last) + x_pos_report
            else:
                self.main_window.stepx_pos = (x_pos_report -65536) - self.main_window.x_pos_last
        if self.main_window.rapid_X != 0 and self.main_window.stepx_pos!=0 and self.main_window.InvertX_tongle==1:
            if self.main_window.rapid_X>0 and self.main_window.stepx_pos<0:
                self.main_window.Message_panel.append(">>> X Position calibrated")
                self.main_window.InvertX_tongle=0
                self.main_window.stepx_direction=-1
            else:
                if self.main_window.rapid_X<0 and self.main_window.stepx_pos>0:
                    self.main_window.Message_panel.append(">>> X Position calibrated")
                    self.main_window.stepx_direction=-1
                    self.main_window.InvertX_tongle=0
                else:
                    self.main_window.Message_panel.append(">>> X Position calibrated")
                    self.main_window.stepx_direction=1
                    self.main_window.InvertX_tongle=0
            self.main_window.rapid_X=0
        self.main_window.stepx_pos=self.main_window.stepx_pos*self.main_window.stepx_direction
        self.main_window.x_pos_last=x_pos_report
        self.main_window.sum_Xpos = self.main_window.sum_Xpos + self.main_window.stepx_pos
        self.main_window.x_overflow=self.main_window.sum_Xpos/100 #pulses to mm
        self.main_window.XPOSITION.setText("{:.3f}".format(round(self.main_window.x_overflow, 3)))

    def update_ypos(self, y_pos_report):
        self.main_window.stepy_pos = y_pos_report - self.main_window.y_pos_last
        if abs(self.main_window.stepy_pos) > 15000: #Below algorithm handles 16bit overflow arduino position
            if self.main_window.stepy_pos<0:
                self.main_window.stepy_pos = (65536-self.main_window.y_pos_last) + y_pos_report
            else:
                self.main_window.stepy_pos = (y_pos_report -65536) - self.main_window.y_pos_last
        if self.main_window.rapid_Y != 0 and self.main_window.stepy_pos!=0 and self.main_window.InvertY_tongle==1:
            if self.main_window.rapid_Y>0 and self.main_window.stepy_pos<0:
                self.main_window.Message_panel.append(">>> Y Position calibrated")
                self.main_window.InvertY_tongle=0
                self.main_window.stepy_direction=-1
            else:
                if self.main_window.rapid_Y<0 and self.main_window.stepy_pos>0:
                    self.main_window.Message_panel.append(">>> Y Position calibrated")
                    self.main_window.stepy_direction=-1
                    self.main_window.InvertY_tongle=0
                else:
                    self.main_window.Message_panel.append(">>> Y Position calibrated")
                    self.main_window.stepy_direction=1
                    self.main_window.InvertΥ_tongle=0
            self.main_window.rapid_Y=0
        self.main_window.stepy_pos=self.main_window.stepy_pos*self.main_window.stepy_direction
        self.main_window.y_pos_last=y_pos_report
        self.main_window.sum_Ypos = self.main_window.sum_Ypos + self.main_window.stepy_pos
        self.main_window.y_overflow=self.main_window.sum_Ypos/100 #pulses to mm
        self.main_window.YPOSITION.setText("{:.3f}".format(round(self.main_window.y_overflow, 3)))

    def update_zpos(self, z_pos_report):
        self.main_window.stepz_pos = z_pos_report - self.main_window.z_pos_last
        if abs(self.main_window.stepz_pos) > 15000: #Below algorithm handles 16bit overflow arduino position
            if self.main_window.stepz_pos<0:
                self.main_window.stepz_pos = (65536-self.main_window.z_pos_last) + z_pos_report
            else:
                self.main_window.stepz_pos = (z_pos_report -65536) - self.main_window.z_pos_last
        if self.main_window.rapid_Z != 0 and self.main_window.stepz_pos!=0 and self.main_window.InvertZ_tongle==1:
            if self.main_window.rapid_Z>0 and self.main_window.stepz_pos<0:
                self.main_window.Message_panel.append(">>> Z Position calibrated")
                self.main_window.InvertZ_tongle=0
                self.main_window.stepz_direction=-1
            else:
                if self.main_window.rapid_Z<0 and self.main_window.stepz_pos>0:
                    self.main_window.Message_panel.append(">>> Z Position calibrated")
                    self.main_window.stepz_direction=-1
                    self.main_window.InvertZ_tongle=0
                else:
                    self.main_window.Message_panel.append(">>> Z Position calibrated")
                    self.main_window.stepz_direction=1
                    self.main_window.InvertZ_tongle=0
            self.main_window.rapid_Z=0
        self.main_window.stepz_pos=self.main_window.stepz_pos*self.main_window.stepz_direction
        self.main_window.z_pos_last=z_pos_report
        self.main_window.sum_Zpos = self.main_window.sum_Zpos + self.main_window.stepz_pos
        self.main_window.z_overflow=self.main_window.sum_Zpos/100 #pulses to mm
        self.main_window.ZPOSITION.setText("{:.3f}".format(round(self.main_window.z_overflow, 3)))

    def print2user_usb(self,message):
        self.main_window.Message_panel.append(message)
    def print2user_bar(self,message):
        self.main_window.Message_panel.append(message)
    def setProgressVal(self,progress_value):
        self.main_window.progressBar.setValue(progress_value)


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
from PyQt5.QtCore import Qt
from PyQt5 import QtGui
from PyQt5.QtGui import *

#********************LOCAL IMPORTS**************************

from ui.mainwindow_ui import Ui_MainWindow
from app_manager.app_graphics.progress_bar import ProgressBarWorker
from serial_controller.comport_scanner import COMPortScanner
from serial_controller.usb_connect_printer import UsbConnect
from printer_controller.rapid_controls.axes_controls import RapidAxesController
from printer_controller.rapid_controls.heat_controls import TempConctrols
from printer_controller.rapid_controls.bltouch_controls import BlTouch
from app_manager.printers_manager import PrintersConfig
from app_manager.processes_manager import ProcessesManager
from app_manager.popup_manager import PopupManager
from app_manager.widget_manager import WidgetManager
from app_manager.file_manager import FileManager
from app_manager.selection_manager import EventConfig

class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, *args, obj=None, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.Message_panel.append(">>> 3DHex")
        self.bar_thread = ProgressBarWorker(self)
        self.comport_thread = COMPortScanner(self)
        self.filehandler = FileManager(self)
        self.selectionhandler = EventConfig(self)
        self.printconfigurations = PrintersConfig(self)
        self.declare_vars()
        self.printconfigurations.load_printers()
        style_sheet = """
            QMenu {
                color: rgb(255, 255, 255);
                background-color: rgb(47, 47, 47);
            }

            QMenuBar {
                color: rgb(255, 255, 255);
                background-color: rgb(47, 47, 47);
            }

            QMenu::item:selected {
                background-color: rgb(83, 83, 83);
            }

            QMenuBar::item:selected {
                background-color: rgb(83, 83, 83);
            }
        """
        self.setStyleSheet(style_sheet)
        #self.actionPrinter2_2.setVisible(False) #test only
        self.usbconnect = UsbConnect(self) #import usb connect
        self.rapidmovecontrol = RapidAxesController(self)
        self.tempidlecontrol = TempConctrols(self)
        self.processesmanager = ProcessesManager(self)
        self.popuphandler = PopupManager(self)
        self.widgethandler = WidgetManager(self)
        self.bltouch = BlTouch(self)
        self.widgethandler.setup_temp_monitor()
        self.widgethandler.UNITS()
        self.widgethandler.ABL_include()
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

    def assign_buttons(self):
        self.p1.clicked.connect(self.usbconnect.CONNECT)
        self.p3.clicked.connect(self.processesmanager.USB)
        self.p5.clicked.connect(self.rapidmovecontrol.HOME_X_MIN)
        self.p9.clicked.connect(self.rapidmovecontrol.HOME_Y_MIN)
        self.p13.clicked.connect(self.rapidmovecontrol.HOME_Z_MIN)

        button_mapping = {
            self.p7: (0, 1),
            self.p8: (0, 0),
            self.p11: (1, 1),
            self.p12: (1, 0),
            self.p15: (2, 1),
            self.p16: (2, 0),
            self.p19: (3, 1),
            self.p20: (3, 0)
        }
        for button, args in button_mapping.items():
            button.clicked.connect(lambda _, args=args: self.rapidmovecontrol.rapid_idle_position(*args))

        self.p21.clicked.connect(self.processesmanager.PAUSE)
        self.p4.clicked.connect(self.processesmanager.SD_CARD)
        self.p88.clicked.connect(self.tempidlecontrol.setNOZZTEMP)
        self.p89.clicked.connect(self.tempidlecontrol.setBEDTEMP)
        self.p22.clicked.connect(self.widgethandler.clear_GCODE)
        self.p23.clicked.connect(self.processesmanager.CANCEL)
        self.p24.clicked.connect(self.processesmanager.setJFAJ)
        self.p25.clicked.connect(self.tempidlecontrol.setFAN1)
        self.p90.clicked.connect(self.processesmanager.nozz_AUTOTUNE)
        self.p91.clicked.connect(self.processesmanager.bed_AUTOTUNE)
        self.p29.clicked.connect(self.processesmanager.View)
        self.p30.clicked.connect(lambda:self.bltouch.BL_TOUCH_TOGGLE(0))
        self.p31.clicked.connect(lambda:self.bltouch.BL_TOUCH_TOGGLE(1))
        self.p27.clicked.connect(self.processesmanager.execute_ABL)
        self.action_Open.triggered.connect(self.filehandler.openfile)
        self.comboBox.currentTextChanged.connect(self.selectionhandler.selectPort)#https://zetcode.com/pyqt/qcheckbox/
        self.c2.stateChanged.connect(self.rapidmovecontrol.setXmotor)
        self.c3.stateChanged.connect(self.rapidmovecontrol.setYmotor)
        self.c4.stateChanged.connect(self.rapidmovecontrol.setZmotor)
        self.c5.stateChanged.connect(self.rapidmovecontrol.setEmotor)
        self.c7.stateChanged.connect(self.selectionhandler.readInvertX)
        self.c8.stateChanged.connect(self.selectionhandler.readInvertY)
        self.c9.stateChanged.connect(self.selectionhandler.readInvertZ)
        self.c12.stateChanged.connect(self.widgethandler.setHOME_Xbuttons)
        self.c13.stateChanged.connect(self.widgethandler.setHOME_Ybuttons)
        self.c14.stateChanged.connect(self.widgethandler.setHOME_Zbuttons)
        self.c21.stateChanged.connect(self.widgethandler.UNITS)
        self.c29.stateChanged.connect(self.widgethandler.ABL_include)
        for printer in range(25):
            action = getattr(self, f"actionPrinter{printer}")
            action.triggered.connect(lambda _, printer=printer: self.printconfigurations.select_printer(printer))

        for pin in range(40):
            button = getattr(self, f"Pin_Button_{pin}")
            button.clicked.connect(lambda _, pin=pin: self.popuphandler.select_HW_pin(pin))

        self.actionNew.triggered.connect(self.printconfigurations.new_printer)
        self.actionRemove.triggered.connect(self.printconfigurations.remove_printer)

    def closeEvent(self, event): #trigger on closing
        self.filehandler.save_settings() #save settings



if __name__ == "__main__":
    QtWidgets.QApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #https://vicrucann.github.io/tutorials/osg-qt-high-dpi/?fbclid=IwAR3lhrM1zYX615yAGoyoJdAYGdKY5W-l5NsQiWu7gEVoQfzsqWML2iPOWBg
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.assign_buttons()
    window.processesmanager.start_COMPort_worker()
    window.show()
    sys.exit(app.exec_())
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

########################LOCAL IMPORTS######################################

#******************local classes**********************************
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

#******************local functions**********************************
from app_manager.setup.declare_vars import declare_vars
from app_manager.setup.assign_buttons import assign_buttons
from app_manager.setup.setup_style import setup_style_sheet

##########################################################################

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
        declare_vars(self)
        self.printconfigurations.load_printers()
        setup_style_sheet(self)
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
        self.XPOSITION.setFont(QFont("Digital-7", 20))
        self.YPOSITION.setFont(QFont("Digital-7", 20))
        self.ZPOSITION.setFont(QFont("Digital-7", 20))
        self.NOZZ_TEMP.setFont(QFont("Digital-7", 20))
        self.BED_TEMP.setFont(QFont("Digital-7", 20))
        assign_buttons(self)

    def closeEvent(self, event): #trigger on closing
        self.filehandler.save_settings() #save settings


if __name__ == "__main__":
    QtWidgets.QApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #https://vicrucann.github.io/tutorials/osg-qt-high-dpi/?fbclid=IwAR3lhrM1zYX615yAGoyoJdAYGdKY5W-l5NsQiWu7gEVoQfzsqWML2iPOWBg
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.processesmanager.start_COMPort_worker()
    window.show()
    sys.exit(app.exec_())
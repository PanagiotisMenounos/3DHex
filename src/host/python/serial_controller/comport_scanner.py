import serial.tools.list_ports
from PyQt5.QtCore import QThread
import time

class COMPortScanner(QThread):

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def run(self):
        while True:
            prtsNow = serial.tools.list_ports.comports()
            if len(self.main_window.ports) < len(prtsNow):
                for i  in prtsNow:
                    if i not in self.main_window.ports:
                        self.main_window.comboBox.addItem(i.device)
                self.main_window.ports = serial.tools.list_ports.comports()
            elif len(self.main_window.ports) > len(prtsNow):
                for i  in self.main_window.ports:
                    if i not in prtsNow:
                        if i.device == self.main_window.chosenPort:
                            self.main_window.con_state = 0
                            #self.p0.setText("Connect")
                            self.main_window.comboBox.setCurrentIndex(0)
                        self.main_window.comboBox.removeItem(self.main_window.comboBox.findText(i.device))
                self.main_window.ports = serial.tools.list_ports.comports()
            time.sleep(0.5)
import serial.tools.list_ports
from PyQt5.QtCore import QThread
import time

class COMPortScanner(QThread):

    def instance_main(self, mainwindow): #receive main window instance in order to acces mainwindow
        self.window = mainwindow

    def run(self):
        while True:
            prtsNow = serial.tools.list_ports.comports()
            if len(self.window.ports) < len(prtsNow):
                for i  in prtsNow:
                    if i not in self.window.ports:
                        self.window.comboBox.addItem(i.device)
                self.window.ports = serial.tools.list_ports.comports()
            elif len(self.window.ports) > len(prtsNow):
                for i  in self.window.ports:
                    if i not in prtsNow:
                        if i.device == self.window.chosenPort:
                            self.window.con_state = 0
                            #self.p0.setText("Connect")
                            self.window.comboBox.setCurrentIndex(0)
                        self.window.comboBox.removeItem(self.window.comboBox.findText(i.device))
                self.window.ports = serial.tools.list_ports.comports()
            time.sleep(0.5)
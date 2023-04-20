from PyQt5.QtCore import Qt, QThread, pyqtSignal
import time
import os
import struct
import win32file

class FLYWorker(QThread):
    message = pyqtSignal(str) #define signal

    def instance_main(self, mainwindow): #receive main window instance in order to acces mainwindow
        self.window = mainwindow

    def run(self):
        if self.window.MM!=0: #M command
           win32file.WriteFile(self.window.pipe,(struct.pack("if",self.window.MM,self.window.SS))) #send command to C via pipe
        else: # % command
           win32file.WriteFile(self.window.pipe,(struct.pack("i",self.window.MM)))
           self.window.SS=float(self.window.d100.value())
           win32file.WriteFile(self.window.pipe,(struct.pack("f",self.window.SS)))
           self.window.SS=float(self.window.d101.value())
           win32file.WriteFile(self.window.pipe,(struct.pack("f",self.window.SS)))
           self.window.SS=float(self.window.d102.value())
           win32file.WriteFile(self.window.pipe,(struct.pack("f",self.window.SS)))
           self.window.SS=float(self.window.d103.value())
           win32file.WriteFile(self.window.pipe,(struct.pack("f",self.window.SS)))       
        self.window.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\fly.bin','wb')
        self.window.child_file.write(struct.pack("i",self.window.mirror))
        self.window.child_file.close()   
        time.sleep(3)
        self.window.froze=0  
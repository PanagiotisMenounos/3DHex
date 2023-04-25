from PyQt5.QtCore import QThread, pyqtSignal
import time
import os
import struct
import win32file

class FLYWorker(QThread):
    message = pyqtSignal(str) #define signal

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def run(self):
        if self.main_window.MM!=0: #M command
           win32file.WriteFile(self.main_window.pipe,(struct.pack("if",self.main_window.MM,self.main_window.SS))) #send command to C via pipe
        else: # % command
           win32file.WriteFile(self.main_window.pipe,(struct.pack("i",self.main_window.MM)))
           self.main_window.SS=float(self.main_window.d100.value())
           win32file.WriteFile(self.main_window.pipe,(struct.pack("f",self.main_window.SS)))
           self.main_window.SS=float(self.main_window.d101.value())
           win32file.WriteFile(self.main_window.pipe,(struct.pack("f",self.main_window.SS)))
           self.main_window.SS=float(self.main_window.d102.value())
           win32file.WriteFile(self.main_window.pipe,(struct.pack("f",self.main_window.SS)))
           self.main_window.SS=float(self.main_window.d103.value())
           win32file.WriteFile(self.main_window.pipe,(struct.pack("f",self.main_window.SS)))       
        self.main_window.child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\binary files\\fly.bin','wb')
        self.main_window.child_file.write(struct.pack("i",self.main_window.mirror))
        self.main_window.child_file.close()   
        time.sleep(3)
        self.main_window.froze=0  
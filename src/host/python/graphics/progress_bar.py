from PyQt5.QtCore import QThread, pyqtSignal
import struct
import win32pipe, win32file

class ProgressBarWorker(QThread):
  message = pyqtSignal(str) #define signal
  progress_value = pyqtSignal(int) #define signal

  def __init__(self, main_window):
      super().__init__()
      self.main_window = main_window

  def run(self):
        try:
            received_progress=0
            win32pipe.ConnectNamedPipe(self.main_window.pipe, None)
            resp = win32file.ReadFile(self.main_window.pipe,4)
            (received_progress,)=struct.unpack("i",resp[1])
            while received_progress <=100:
                self.progress_value.emit(received_progress) #emit the signal
                resp = win32file.ReadFile(self.main_window.pipe,4)
                (received_progress,)=struct.unpack("i",resp[1])
                if received_progress==100:
                   self.progress_value.emit(received_progress) #emit the signal
                   received_progress=101
        except:
             self.progress_value.emit(0)
             win32file.CloseHandle(self.main_window.pipe)
        finally:
              win32file.CloseHandle(self.main_window.pipe)
              #print("closed pipe")
              self.message.emit(">>> Completed") #emit the signal
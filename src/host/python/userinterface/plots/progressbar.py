from PyQt5.QtCore import QThread, pyqtSignal
import struct
import win32pipe, win32file


class ProgressBarWorker(QThread):

    message = pyqtSignal(str) #define signal
    progress_value = pyqtSignal(int) #define signal

    def instance_main(self, mainwindow): #receive main window instance in order to acces PIPELINE signal slots
        self.window = mainwindow

    def run(self):
        try:
            received_progress=0
            win32pipe.ConnectNamedPipe(self.window.pipe, None)
            resp = win32file.ReadFile(self.window.pipe,4)
            (received_progress,)=struct.unpack("i",resp[1])

            while received_progress <=100:
                self.progress_value.emit(received_progress) #emit the signal
                resp = win32file.ReadFile(self.window.pipe,4)
                (received_progress,)=struct.unpack("i",resp[1])
                if received_progress==100:
                   self.progress_value.emit(received_progress) #emit the signal
                   received_progress=101
        except:
            self.progress_value.emit(0)
            win32file.CloseHandle(self.window.pipe)
        finally:
              win32file.CloseHandle(self.window.pipe)
              self.message.emit(">>> Completed") #emit the signal
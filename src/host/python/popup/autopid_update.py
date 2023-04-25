from PyQt5 import QtWidgets

from ui.auto_pid_ui import Ui_AutoTune

class AutoTuneWindow(QtWidgets.QMainWindow, Ui_AutoTune):
    def __init__(self, mainwindow, *args, obj=None, **kwargs):
        super(AutoTuneWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.main_window = mainwindow
        self.ok_autotune.clicked.connect(self.OK_Printer)
        self.cancel_autotune.clicked.connect(self.CANCEL_Printer)

    def OK_Printer(self):
        if self.main_window.C == 1:
            self.main_window.b41.clear()
            self.main_window.b41.insertPlainText(str(self.main_window.Auto_P))
            self.main_window.b43.clear()
            self.main_window.b43.insertPlainText(str(self.main_window.Auto_I))
            self.main_window.b45.clear()
            self.main_window.b45.insertPlainText(str(self.main_window.Auto_D))
        else:
            self.main_window.b42.clear()
            self.main_window.b42.insertPlainText(str(self.main_window.Auto_P))
            self.main_window.b44.clear()
            self.main_window.b44.insertPlainText(str(self.main_window.Auto_I))
            self.main_window.b46.clear()
            self.main_window.b46.insertPlainText(str(self.main_window.Auto_D))
        self.main_window.Message_panel.append(">>> PID updated")
        self.close()


    def CANCEL_Printer(self):
        self.close()
        self.main_window.Message_panel.append(">>> Aborted")
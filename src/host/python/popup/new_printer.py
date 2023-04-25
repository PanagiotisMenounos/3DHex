import os
from PyQt5 import QtWidgets

from ui.new_printer_ui import Ui_New_Printer

class PrinterWindow(QtWidgets.QMainWindow, Ui_New_Printer):

    def __init__(self, mainwindow, *args, obj=None, **kwargs):
        super(PrinterWindow, self).__init__(*args, **kwargs)
        self.setupUi(self)  # import Qtdesigner
        self.main_window = mainwindow
        self.ok_printer.clicked.connect(self.OK_Printer)
        self.cancel_printer.clicked.connect(self.CANCEL_Printer)
        self.main_window.Message_panel.append(">>> Add new settings")

    def OK_Printer(self):
        text = self.input_name.toPlainText().replace("\n", "")
        if text == '':
            self.main_window.Message_panel.append(">>> Aborted")
            self.close()
        else:
            # print(text)
            self.main_window.freeroom = 0
            self.main_window.added = 0
            printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'r')
            printers = printer_file.readlines()
            printer_file.close()
            printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'w')
            self.main_window.printers = len(printers)
            # print(self.main_window.printer)
            for temp in printers:
                printer = temp.split(',')
                printer[2] = printer[2].replace("\n", "")
                if int(printer[2]) == 0:
                    self.main_window.freeroom = 1
            if self.main_window.freeroom == 1:
                for temp in printers:
                    printer = temp.split(',')
                    printer[2] = printer[2].replace("\n", "")
                    if int(printer[2]) == 0 and self.main_window.added == 0:
                        printer_file.write("1," + text + ",1\n")
                        self.main_window.Message_panel.append(">>> Added: " + text)
                        self.main_window.added = 1
                    else:
                        printer_file.write("0," + printer[1] + "," + printer[2] + "\n")
            else:
                self.main_window.Message_panel.append(">>> Cannot add more than " + str(self.main_window.printers) + " configurations")
                for temp in printers:
                    printer = temp.split(',')
                    printer[2] = printer[2].replace("\n", "")
                    if self.main_window.added == 0:
                        printer_file.write("1,Default,1\n")
                        self.main_window.added = 1
                    else:
                        printer_file.write("0," + printer[1] + "," + printer[2] + "\n")
            self.main_window.freeroom = 0
            self.main_window.added = 0
            printer_file.close()
            self.main_window.save_settings()
            self.main_window.load_printers()
            self.close()

    def CANCEL_Printer(self):
        self.close()
        self.main_window.Message_panel.append(">>> Aborted")
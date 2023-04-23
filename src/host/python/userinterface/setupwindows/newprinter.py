from PyQt5 import QtWidgets
import os

from userinterface.windows.printer_name import Ui_New_Printer
class PrinterWindow(QtWidgets.QMainWindow, Ui_New_Printer):

    def instance_main(self, mainwindow): #receive main window instance in order to acces mainwindow
        self.window = mainwindow
    def initiate(self, *args, obj=None, **kwargs):
        super(PrinterWindow, self).__init__(*args, **kwargs)
        self.setupUi(self)  # import Qtdesigner
        self.ok_printer.clicked.connect(self.OK_Printer)
        self.cancel_printer.clicked.connect(self.CANCEL_Printer)
        self.window.Message_panel.append(">>> Add new settings")

    def OK_Printer(self):
        text = self.input_name.toPlainText().replace("\n", "")
        if text == '':
            self.window.Message_panel.append(">>> Aborted")
            self.close()
        else:
            # print(text)
            self.window.freeroom = 0
            self.window.added = 0
            printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'r')
            printers = printer_file.readlines()
            printer_file.close()
            printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'w')
            self.window.printers = len(printers)
            # print(self.window.printer)
            for temp in printers:
                printer = temp.split(',')
                printer[2] = printer[2].replace("\n", "")
                if int(printer[2]) == 0:
                    self.window.freeroom = 1
            if self.window.freeroom == 1:
                for temp in printers:
                    printer = temp.split(',')
                    printer[2] = printer[2].replace("\n", "")
                    if int(printer[2]) == 0 and self.window.added == 0:
                        printer_file.write("1," + text + ",1\n")
                        self.window.Message_panel.append(">>> Added: " + text)
                        self.window.added = 1
                    else:
                        printer_file.write("0," + printer[1] + "," + printer[2] + "\n")
            else:
                self.window.Message_panel.append(">>> Cannot add more than " + str(self.window.printers) + " configurations")
                for temp in printers:
                    printer = temp.split(',')
                    printer[2] = printer[2].replace("\n", "")
                    if self.window.added == 0:
                        printer_file.write("1,Default,1\n")
                        self.window.added = 1
                    else:
                        printer_file.write("0," + printer[1] + "," + printer[2] + "\n")
            self.window.freeroom = 0
            self.window.added = 0
            printer_file.close()
            self.window.save_settings()
            self.window.load_printers()
            self.close()

    def CANCEL_Printer(self):
        self.close()
        self.window.Message_panel.append(">>> Aborted")
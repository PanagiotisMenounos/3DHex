import os
from popup.new_printer import PrinterWindow

class PrintersConfig:
    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def new_printer(self):
        self.window2 = PrinterWindow(self.main_window)
        self.window2.show()

    def select_printer(self, printer_selection):
        self.main_window.filehandler.save_settings()
        printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'r')
        printers = printer_file.readlines()
        printer_file.close()
        printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'w')
        total_printers = len(printers)
        p = 0
        for temp in printers:
            printer = temp.split(',')
            if p < total_printers:
                if p == printer_selection:
                    printer_file.write("1," + printer[1] + "," + printer[2])
                else:
                    printer_file.write("0," + printer[1] + "," + printer[2])
            p = p + 1
        printer_file.close()
        self.load_printers()

    def remove_printer(self):
        printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'r')
        printers = printer_file.readlines()
        printer_file.close()
        printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'w')
        total_printers = len(printers)
        p = 0
        for temp in printers:
            printer = temp.split(',')
            if p == 0:
                printer_file.write("1," + printer[1].replace("\n", "") + ",1\n")
            else:
                if p == self.main_window.printer:
                    printer_file.write("0," + printer[1].replace("\n", "") + ",0\n")
                    self.main_window.Message_panel.append(">>> Removed: " + str(printer[1].replace("\n", "")))
                else:
                    printer_file.write("0," + printer[1].replace("\n", "") + "," + printer[2])
            p = p + 1

        printer_file.close()
        self.load_printers()

    def load_printers(self):
        printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'r')
        printers = printer_file.readlines()
        total_printers = len(printers)
        p = 0
        for temp in printers:
            printer = temp.split(',')
            if p < total_printers:
                if int(printer[0]) == 1:
                    self.main_window.printer = p
                    self.main_window.selected_printer = p
                    action_printer = getattr(self.main_window, "actionPrinter{}".format(p))
                    self.main_window.Message_panel.append(">>> Loaded: " + str(printer[1].replace("\n", "")))
                    action_printer.setText(str("● " + printer[1].replace("\n", "")))
                else:
                    action_printer = getattr(self.main_window, "actionPrinter{}".format(p))
                    action_printer.setText(str("   " + printer[1].replace("\n", "")))
                if int(printer[2]) == 1:
                    action_printer = getattr(self.main_window, "actionPrinter{}".format(p))
                    action_printer.setVisible(True)
                else:
                    action_printer = getattr(self.main_window, "actionPrinter{}".format(p))
                    action_printer.setVisible(False)
            p = p + 1
        printer_file.close()
        self.main_window.filehandler.load_settings()
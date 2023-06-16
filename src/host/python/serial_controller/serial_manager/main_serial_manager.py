from PyQt5.QtCore import QThread, pyqtSignal
import time
import struct

from serial_controller.serial_manager.serial_decoder import SerialDecoder
from serial_controller.serial_manager.serial_printing import SerialPrint
from serial_controller.serial_manager.buffer_sender import BufferSender
from printer_controller.idle_commander import IdleCommando

class USBWorker(QThread):  # This thread starts when 3DHEX connects successfully to the Printer
    message = pyqtSignal(str)  # define signal
    new_nozz_temp = pyqtSignal(float)
    new_bed_temp = pyqtSignal(float)
    autotune_p = pyqtSignal(float)
    x_pos_report = pyqtSignal(float)
    y_pos_report = pyqtSignal(float)
    z_pos_report = pyqtSignal(float)

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window
        self.decoder = SerialDecoder(self, self.main_window)
        self.usbprinter = SerialPrint(self, self.main_window)
        self.serialsender = BufferSender(self.main_window)
        self.idlechecker = IdleCommando(self, self.main_window)

    def run(self):
        time.sleep(2)
        self.serialsender.send_buffer()  # Set printer to idle mode temp only report
        self.message.emit(">>> Mode: Idle")  # emit the signal
        try:
            while self.main_window.USB_CONNECTED == 1:
                (self.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS, self.main_window.Y_POS,
                 self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(18))  # Read temperature
                # print("XPOS="+str(self.main_window.X_POS))
                if self.main_window.A == 0:  # if in idle mode
                    if self.serial_command == -200:  # -200 autotune read settings
                        self.main_window.Auto_P = round(self.main_window.nozz_temp, 2)
                        self.main_window.Auto_I = round(self.main_window.bed_temp, 2)
                        (self.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS, self.main_window.Y_POS,
                         self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(18))  # Read temperature
                        self.main_window.Auto_D = round(self.main_window.nozz_temp, 2)
                        self.message.emit(">>> AUTOTUNE RESULTS")  # emit the signal
                        self.message.emit(">>> P=" + str(self.main_window.Auto_P))
                        self.message.emit(">>> I=" + str(self.main_window.Auto_I))
                        self.message.emit(">>> D=" + str(self.main_window.Auto_D))
                        self.autotune_p.emit(self.main_window.Auto_I)
                        self.serial_command = -243
                    elif self.serial_command == -301:  # temporary
                        self.message.emit(">>> ZTRACK=" + str(int(self.main_window.nozz_temp)))
                        self.serial_command = -243
                    else:  # just update temp
                        self.new_nozz_temp.emit(self.main_window.nozz_temp)  # emit the signal
                        self.new_bed_temp.emit(self.main_window.bed_temp)  # emit the signal
                        self.main_window.X_POS = self.main_window.InvertX * self.main_window.X_POS
                        self.x_pos_report.emit(self.main_window.X_POS)
                        self.y_pos_report.emit(self.main_window.Y_POS)
                        self.z_pos_report.emit(self.main_window.Z_POS)
                    time.sleep(0.5)
                    self.idlechecker.check_idle_commands()  # Check if any idle command has triggered
                else:
                    self.message.emit(">>> Mode: USB printing")  # emit the signal
                    # self.main_window.Message_panel.append(">>> Start usb printing")
                    time.sleep(0.5)
                    self.usbprinter.usb_printing()  # Go into USB Print function
                    time.sleep(5)
                    self.main_window.A = 0  # After USB printing has completed declare idle mode
                    self.main_window.B = 0
                    self.main_window.C = 0
                    self.main_window.ser.flushInput()  # very important without delay to fix a bug
                    self.main_window.ser.flushOutput()  # very important without delay to fix a bug
                    self.serialsender.send_buffer()  # Then send idle mode temp only report to printer
                    self.main_window.widgethandler.enable_idle_buttons()
        except:
            self.main_window.USB_CONNECTED = 0

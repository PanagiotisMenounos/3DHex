import shutil
import os
import struct

class SerialDecoder:

    def __init__(self, usbworker, main_window):
        super().__init__()
        self.main_window = main_window
        self.serial_manager = usbworker

    def packet_decode(self):
        (self.serial_manager.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS,
         self.main_window.Y_POS,
         self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(
            18))  # This first time read buffer1 contains all the necessary settings for Printer
        while int(
                self.serial_manager.serial_command) != -253 and self.serial_manager.child_buffer_size != 0 and self.main_window.usb_printing == 1 and self.serial_manager.serial_command != -260:
            if int(self.serial_manager.serial_command) == -300:  # 300-> start of ABL
                self.serial_manager.abl_z_file = open(
                    os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(
                        self.main_window.printer) + '\\abl_z.txt',
                    "w")
            if int(self.serial_manager.serial_command) == -301:  # -301-> TRACK_Z
                self.serial_manager.trackZ = self.serial_manager.trackZ + self.main_window.nozz_temp
                if self.main_window.ABL_Sample == 0:
                    if self.main_window.min == 0:
                        self.main_window.min = self.main_window.nozz_temp
                    if self.main_window.nozz_temp < self.main_window.min:
                        self.main_window.min = self.main_window.nozz_temp
            if int(self.serial_manager.serial_command) == -303:  # -303-> end of iterations
                self.serial_manager.AVG_traxkZ = self.serial_manager.trackZ / 2
                self.serial_manager.trackZ = 0
                if self.main_window.ABL_Sample == 0:
                    self.main_window.ABL_Z_CENTER = self.main_window.min
                    self.serial_manager.AVG_traxkZ = self.main_window.min
                    self.main_window.ABL_Sample = 1
                self.serial_manager.AVG_traxkZ = (self.main_window.ABL_Z_CENTER - self.serial_manager.AVG_traxkZ) / self.main_window.STPZ
                self.serial_manager.message.emit(
                    ">>> AVG:" + str("{:.4f}".format(round(float(self.serial_manager.AVG_traxkZ), 4))) + "mm")  # emit the signal
                self.serial_manager.abl_z_file.write(str("{:.4f}".format(round(float(self.serial_manager.AVG_traxkZ), 4))) + "\n")
            if int(self.serial_manager.serial_command) == -302:  # 302-> end of ABL
                self.main_window.ABL_Sample = 0
                self.serial_manager.AVG_traxkZ = 0
                self.serial_manager.trackZ = 0
                self.serial_manager.abl_z_file.close()
                self.main_window.ABL = 0
                shutil.copy(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\abl_x.txt',
                            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(
                                self.main_window.printer) + '\\abl_x.txt')
                shutil.copy(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\abl_y.txt',
                            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(
                                self.main_window.printer) + '\\abl_y.txt')
                self.serial_manager.abl_type_file = open(
                    os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(
                        self.main_window.printer) + '\\abl_type.txt',
                    "w")
                self.serial_manager.abl_type_file.write(str(self.main_window.abl_type) + "\n")
                self.serial_manager.abl_type_file.close()
                if self.main_window.abl_type == 1:
                    self.main_window.ABL_INTERPOLATION_TYPE = self.main_window.abl_type
                elif self.main_window.abl_type == 2:
                    self.main_window.ABL_INTERPOLATION_TYPE = self.main_window.abl_type
                elif self.main_window.abl_type == 0:
                    self.main_window.ABL_INTERPOLATION_TYPE = self.main_window.abl_type
                # self.ABL_interpolation_thread=ABL_Interpolation()
                # self.ABL_interpolation_thread.start()
            if int(self.serial_manager.serial_command) == -243:  # temp report + xyz pos
                self.serial_manager.new_nozz_temp.emit(self.main_window.nozz_temp)  # emit the signal
                self.serial_manager.new_bed_temp.emit(self.main_window.bed_temp)  # emit the signal
                self.serial_manager.x_pos_report.emit(self.main_window.X_POS)
                self.serial_manager.y_pos_report.emit(self.main_window.Y_POS)
                self.serial_manager.z_pos_report.emit(self.main_window.Z_POS)
            (self.serial_manager.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS,
             self.main_window.Y_POS,
             self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(
                18))  # This first time read buffer1 contains all the necessary settings for Printer
            self.serial_manager.child_buffer_size = os.path.getsize(
                os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin')
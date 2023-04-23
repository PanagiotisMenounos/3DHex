import shutil
from PyQt5.QtCore import QThread, pyqtSignal
import time
import os
import subprocess
import struct

class USBWorker(QThread):  # This thread starts when 3DHEX connects successfully to the Printer
    message = pyqtSignal(str)  # define signal
    new_nozz_temp = pyqtSignal(float)
    new_bed_temp = pyqtSignal(float)
    autotune_p = pyqtSignal(float)
    x_pos_report = pyqtSignal(float)
    y_pos_report = pyqtSignal(float)
    z_pos_report = pyqtSignal(float)

    def instance_main(self, mainwindow):
        self.window = mainwindow

    def run(self):
        time.sleep(2)
        self.send_buffer()  # Set printer to idle mode temp only report
        self.message.emit(">>> Mode: Idle")  # emit the signal
        try:
            while self.window.USB_CONNECTED == 1:
                (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
                 self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(18))  # Read temperature
                # print("XPOS="+str(self.window.X_POS))
                if self.window.A == 0:  # if in idle mode
                    if self.serial_command == -200:  # -200 autotune read settings
                        self.window.Auto_P = round(self.window.nozz_temp, 2)
                        self.window.Auto_I = round(self.window.bed_temp, 2)
                        (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
                         self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(18))  # Read temperature
                        self.window.Auto_D = round(self.window.nozz_temp, 2)
                        self.message.emit(">>> AUTOTUNE RESULTS")  # emit the signal
                        self.message.emit(">>> P=" + str(self.window.Auto_P))
                        self.message.emit(">>> I=" + str(self.window.Auto_I))
                        self.message.emit(">>> D=" + str(self.window.Auto_D))
                        self.autotune_p.emit(self.window.Auto_I)
                        self.serial_command = -243
                    elif self.serial_command == -301:  # temporary
                        self.message.emit(">>> ZTRACK=" + str(int(self.window.nozz_temp)))
                        self.serial_command = -243
                    else:  # just update temp
                        self.new_nozz_temp.emit(self.window.nozz_temp)  # emit the signal
                        self.new_bed_temp.emit(self.window.bed_temp)  # emit the signal
                        self.window.X_POS = self.window.InvertX * self.window.X_POS
                        self.x_pos_report.emit(self.window.X_POS)
                        self.y_pos_report.emit(self.window.Y_POS)
                        self.z_pos_report.emit(self.window.Z_POS)
                    time.sleep(0.5)
                    self.check_idle_commands()  # Check if any idle command has triggered
                else:
                    self.message.emit(">>> Mode: USB printing")  # emit the signal
                    # self.window.Message_panel.append(">>> Start usb printing")
                    time.sleep(0.5)
                    self.usb_printing()  # Go into USB Print function
                    time.sleep(5)
                    self.window.A = 0  # After USB printing has completed declare idle mode
                    self.window.B = 0
                    self.window.C = 0
                    self.window.ser.flushInput()  # very important without delay to fix a bug
                    self.window.ser.flushOutput()  # very important without delay to fix a bug
                    self.send_buffer()  # Then send idle mode temp only report to printer
                    self.window.enable_idle_buttons()
        except:
            self.window.USB_CONNECTED = 0

    def send_buffer(self):
        go = 3
        pin = self.window.Pin_Button_0.text()
        if pin == 'N':
            X_ENABLE_PIN = 255
        else:
            X_ENABLE_PIN = int(pin)
        pin = self.window.Pin_Button_1.text()
        if pin == 'N':
            X_STEP_PIN = 255
        else:
            X_STEP_PIN = int(pin)
        pin = self.window.Pin_Button_2.text()
        if pin == 'N':
            X_DIR_PIN = 255
        else:
            X_DIR_PIN = int(pin)
        pin = self.window.Pin_Button_3.text()
        if pin == 'N':
            X_HOME_PIN = 255
        else:
            X_HOME_PIN = int(pin)

        pin = self.window.Pin_Button_4.text()
        if pin == 'N':
            Y_ENABLE_PIN = 255
        else:
            Y_ENABLE_PIN = int(pin)
        pin = self.window.Pin_Button_5.text()
        if pin == 'N':
            Y_STEP_PIN = 255
        else:
            Y_STEP_PIN = int(pin)
        pin = self.window.Pin_Button_6.text()
        if pin == 'N':
            Y_DIR_PIN = 255
        else:
            Y_DIR_PIN = int(pin)
        pin = self.window.Pin_Button_7.text()
        if pin == 'N':
            Y_HOME_PIN = 255
        else:
            Y_HOME_PIN = int(pin)

        pin = self.window.Pin_Button_8.text()
        if pin == 'N':
            Z_ENABLE_PIN = 255
        else:
            Z_ENABLE_PIN = int(pin)
        pin = self.window.Pin_Button_9.text()
        if pin == 'N':
            Z_STEP_PIN = 255
        else:
            Z_STEP_PIN = int(pin)
        pin = self.window.Pin_Button_10.text()
        if pin == 'N':
            Z_DIR_PIN = 255
        else:
            Z_DIR_PIN = int(pin)
        pin = self.window.Pin_Button_11.text()
        if pin == 'N':
            Z_HOME_PIN = 255
        else:
            Z_HOME_PIN = int(pin)

        pin = self.window.Pin_Button_12.text()
        if pin == 'N':
            Z1_ENABLE_PIN = 255
        else:
            Z1_ENABLE_PIN = int(pin)
        pin = self.window.Pin_Button_13.text()
        if pin == 'N':
            Z1_STEP_PIN = 255
        else:
            Z1_STEP_PIN = int(pin)
        pin = self.window.Pin_Button_14.text()
        if pin == 'N':
            Z1_DIR_PIN = 255
        else:
            Z1_DIR_PIN = int(pin)
        pin = self.window.Pin_Button_15.text()
        if pin == 'N':
            Z1_HOME_PIN = 255
        else:
            Z1_HOME_PIN = int(pin)

        pin = self.window.Pin_Button_16.text()
        if pin == 'N':
            E_ENABLE_PIN = 255
        else:
            E_ENABLE_PIN = int(pin)
        pin = self.window.Pin_Button_17.text()
        if pin == 'N':
            E_STEP_PIN = 255
        else:
            E_STEP_PIN = int(pin)
        pin = self.window.Pin_Button_18.text()
        if pin == 'N':
            E_DIR_PIN = 255
        else:
            E_DIR_PIN = int(pin)
        pin = self.window.Pin_Button_19.text()
        if pin == 'N':
            E_HOME_PIN = 255
        else:
            E_HOME_PIN = int(pin)

        pin = self.window.Pin_Button_20.text()
        if pin == 'N':
            N_HEATER_PIN = 255
        else:
            N_HEATER_PIN = int(pin)
        pin = self.window.Pin_Button_21.text()
        if pin == 'N':
            N_SENSOR_PIN = 255
        else:
            N_SENSOR_PIN = int(pin)
        pin = self.window.Pin_Button_22.text()
        if pin == 'N':
            N_FAN_PIN = 255
        else:
            N_FAN_PIN = int(pin)
        pin = self.window.Pin_Button_23.text()
        if pin == 'N':
            B_HEATER_PIN = 255
        else:
            B_HEATER_PIN = int(pin)
        pin = self.window.Pin_Button_24.text()
        if pin == 'N':
            B_SENSOR_PIN = 255
        else:
            B_SENSOR_PIN = int(pin)
        pin = self.window.Pin_Button_25.text()
        if pin == 'N':
            FAN_PIN = 255
        else:
            FAN_PIN = int(pin)
        pin = self.window.Pin_Button_26.text()
        if pin == 'N':
            BL_PIN = 255
        else:
            BL_PIN = int(pin)
        pin = self.window.Pin_Button_27.text()
        if pin == 'N':
            SERVO1_PIN = 255
        else:
            SERVO1_PIN = int(pin)
        pin = self.window.Pin_Button_28.text()
        if pin == 'N':
            SERVO2_PIN = 255
        else:
            SERVO2_PIN = int(pin)
        pin = self.window.Pin_Button_29.text()
        if pin == 'N':
            RS_PIN = 255
        else:
            RS_PIN = int(pin)
        pin = self.window.Pin_Button_30.text()
        if pin == 'N':
            LCD_ENABLE_PIN = 255
        else:
            LCD_ENABLE_PIN = int(pin)
        pin = self.window.Pin_Button_31.text()
        if pin == 'N':
            D4_PIN = 255
        else:
            D4_PIN = int(pin)
        pin = self.window.Pin_Button_32.text()
        if pin == 'N':
            D5_PIN = 255
        else:
            D5_PIN = int(pin)
        pin = self.window.Pin_Button_33.text()
        if pin == 'N':
            D6_PIN = 255
        else:
            D6_PIN = int(pin)
        pin = self.window.Pin_Button_34.text()
        if pin == 'N':
            D7_PIN = 255
        else:
            D7_PIN = int(pin)
        pin = self.window.Pin_Button_35.text()
        if pin == 'N':
            BTEN1_PIN = 255
        else:
            BTEN1 = int(pin)
        pin = self.window.Pin_Button_36.text()
        if pin == 'N':
            BTEN2_PIN = 255
        else:
            BTEN2_PIN = int(pin)
        pin = self.window.Pin_Button_37.text()
        if pin == 'N':
            BTENC_PIN = 255
        else:
            BTENC_PIN = int(pin)
        pin = self.window.Pin_Button_38.text()
        if pin == 'N':
            SD_CS_PIN = 255
        else:
            SD_CS_PIN = int(pin)
        pin = self.window.Pin_Button_39.text()
        if pin == 'N':
            SD_DET_PIN = 255
        else:
            SD_DET_PIN = int(pin)
        self.window.ser.write(struct.pack("4B", 0, 0, 0, 5))
        (pass_fail,) = struct.unpack("B", self.window.ser.read(1))  # Wait for arduino to confirm everything is ok
        self.window.ser.write(
            struct.pack("2b26h8B4H2B9H", go, go, X_ENABLE_PIN, X_STEP_PIN, X_DIR_PIN, X_HOME_PIN, Y_ENABLE_PIN,
                        Y_STEP_PIN, Y_DIR_PIN, Y_HOME_PIN, Z_ENABLE_PIN, Z_STEP_PIN, Z_DIR_PIN, Z_HOME_PIN,
                        Z1_ENABLE_PIN, Z1_STEP_PIN, Z1_DIR_PIN, Z1_HOME_PIN, E_ENABLE_PIN, E_STEP_PIN, E_DIR_PIN,
                        E_HOME_PIN, N_HEATER_PIN, N_SENSOR_PIN, N_FAN_PIN, B_HEATER_PIN, B_SENSOR_PIN, FAN_PIN,
                        self.window.A, self.window.B, self.window.C, self.window.D, self.window.E, self.window.F, self.window.G, self.window.H, self.window.I,
                        self.window.J, self.window.K, self.window.L, self.window.M, self.window.N, self.window.O, self.window.P, self.window.Q, self.window.R,
                        self.window.S, self.window.T, self.window.U, self.window.V, self.window.W))
        (pass_fail,) = struct.unpack("B", self.window.ser.read(1))  # Wait for arduino to confirm everything is ok
        # if pass_fail==1: #pass_fail should be 1, else communication has failed
        # print("PASS")
        # else:
        # print("FAILED")

    def packet_decode(self):
        (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
         self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(
            18))  # This first time read buffer1 contains all the necessary settings for Printer
        while int(
                self.serial_command) != -253 and self.child_buffer_size != 0 and self.window.usb_printing == 1 and self.serial_command != -260:
            if int(self.serial_command) == -300:  # 300-> start of ABL
                self.abl_z_file = open(
                    os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.window.printer) + '\\abl_z.txt',
                    "w")
            if int(self.serial_command) == -301:  # -301-> TRACK_Z
                self.trackZ = self.trackZ + self.window.nozz_temp
                if self.window.ABL_Sample == 0:
                    if self.window.min == 0:
                        self.window.min = self.window.nozz_temp
                    if self.window.nozz_temp < self.window.min:
                        self.window.min = self.window.nozz_temp
            if int(self.serial_command) == -303:  # -303-> end of iterations
                self.AVG_traxkZ = self.trackZ / 2
                self.trackZ = 0
                if self.window.ABL_Sample == 0:
                    self.window.ABL_Z_CENTER = self.window.min
                    self.AVG_traxkZ = self.window.min
                    self.window.ABL_Sample = 1
                self.AVG_traxkZ = (self.window.ABL_Z_CENTER - self.AVG_traxkZ) / self.window.STPZ
                self.message.emit(
                    ">>> AVG:" + str("{:.4f}".format(round(float(self.AVG_traxkZ), 4))) + "mm")  # emit the signal
                self.abl_z_file.write(str("{:.4f}".format(round(float(self.AVG_traxkZ), 4))) + "\n")
            if int(self.serial_command) == -302:  # 302-> end of ABL
                self.window.ABL_Sample = 0
                self.AVG_traxkZ = 0
                self.trackZ = 0
                self.abl_z_file.close()
                self.window.ABL = 0
                shutil.copy(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\abl_x.txt',
                            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(
                                self.window.printer) + '\\abl_x.txt')
                shutil.copy(os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\abl_y.txt',
                            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(
                                self.window.printer) + '\\abl_y.txt')
                self.abl_type_file = open(
                    os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.window.printer) + '\\abl_type.txt',
                    "w")
                self.abl_type_file.write(str(self.window.abl_type) + "\n")
                self.abl_type_file.close()
                if self.window.abl_type == 1:
                    self.window.ABL_INTERPOLATION_TYPE = self.window.abl_type
                elif self.window.abl_type == 2:
                    self.window.ABL_INTERPOLATION_TYPE = self.window.abl_type
                elif self.window.abl_type == 0:
                    self.window.ABL_INTERPOLATION_TYPE = self.window.abl_type
                # self.ABL_interpolation_thread=ABL_Interpolation()
                # self.ABL_interpolation_thread.start()
            if int(self.serial_command) == -243:  # temp report + xyz pos
                self.new_nozz_temp.emit(self.window.nozz_temp)  # emit the signal
                self.new_bed_temp.emit(self.window.bed_temp)  # emit the signal
                self.x_pos_report.emit(self.window.X_POS)
                self.y_pos_report.emit(self.window.Y_POS)
                self.z_pos_report.emit(self.window.Z_POS)
            (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
             self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(
                18))  # This first time read buffer1 contains all the necessary settings for Printer
            self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin')

    def usb_printing(self):  # USB Printing function
        self.window.usb_printing = 1
        self.trackZ = 0
        self.window.A = 1  # printing mode
        self.serial_command = 0  # reset serial command from arduino
        flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
        flag_file.write(struct.pack('2i', 5,
                                    5))  # Write some trash data in order for 3DHex.C to know Python is in printing function
        flag_file.close()
        flag_py_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag_py.bin', "wb")  # Clear flag file
        flag_py_file.close()
        buffer1_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin',
                            "wb")  # Reset buffer1 binary file
        buffer2_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin',
                            "wb")  # Reset buffer2 binary file
        buffer1_file.close()
        buffer2_file.close()
        self.message.emit(">>> GCODE Post processing..this may take a while..")  # emit the signal
        p1 = subprocess.Popen("3DBrain.exe")  # Start 3DHex.C Proccess
        flag_py_buffer = 0  # Reset flag_py_buffer
        filecase = 1  # Read from buffer1 file
        buffer_file_size = 3000  # Declare buffer file size (This is max arduino buffer array size until all RAM is full)
        self.child_buffer_size = 1  # Means 3DHex.C is still running
        while flag_py_buffer == 0 and self.window.usb_printing == 1:  # wait for C to fill binary data to buffer1+buffer2 binary files
            (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
             self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(18))  # Read arduino temp report
            self.new_nozz_temp.emit(self.window.nozz_temp)  # emit the signal
            self.new_bed_temp.emit(self.window.bed_temp)  # emit the signal
            self.x_pos_report.emit(self.window.X_POS)
            self.y_pos_report.emit(self.window.Y_POS)
            self.z_pos_report.emit(self.window.Z_POS)
            flag_py_buffer = os.path.getsize(os.getenv(
                'LOCALAPPDATA') + '\\3DHex2\\binary files\\flag_py.bin')  # Get size of flag_py_file, if 1 then C has fill binary files
        self.window.ser.flushInput()  # very important without delay to fix a bug
        self.window.ser.flushOutput()  # very important without delay to fix a bug
        self.send_buffer()  # Command Printer to go into printig mode self.window.A=1
        self.message.emit(">>> Post processing successfully completed")  # emit the signal
        self.message.emit(">>> Printing...")  # emit the signal
        if buffer_file_size == 3000 and self.child_buffer_size != 0 and self.serial_command != -260:  # Firt time send binary data to Printer
            if filecase == 1:  # Read from buffer1 binary file
                filecase = 2  # Note to read buffer2 next time
                buffer1_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin', "rb")
                self.packet_decode()
                self.window.ser.write(buffer1_file.read(3000))  # Send binary data to Printer
                buffer1_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5,
                                            5))  # Write some trash data to tell C that buffer1 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            if filecase == 2:
                filecase = 1
                buffer2_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin', "rb")
                self.packet_decode()
                self.window.ser.write(buffer2_file.read(3000))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5,
                                            5))  # Write some trash data to tell C that buffer2 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin')
        while self.serial_command != -10 and self.child_buffer_size != 0 and self.window.usb_printing == 1:  # Wait for printer to read commanded temp
            (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
             self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(18))  # Read arduino temp report
            self.new_nozz_temp.emit(self.window.nozz_temp)  # emit the signal
            self.new_bed_temp.emit(self.window.bed_temp)  # emit the signal
            self.x_pos_report.emit(self.window.X_POS)
            self.y_pos_report.emit(self.window.Y_POS)
            self.z_pos_report.emit(self.window.Z_POS)
        while buffer_file_size == 3000 and self.child_buffer_size != 0 and self.serial_command != -260 and self.window.usb_printing == 1:  # Start binary data streaming to Printer
            if filecase == 1:
                filecase = 2
                buffer1_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin', "rb")
                self.packet_decode()
                self.window.ser.write(buffer1_file.read(3000))
                buffer1_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            elif filecase == 2:
                filecase = 1
                buffer2_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin', "rb")
                self.packet_decode()
                self.window.ser.write(buffer2_file.read(3000))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            self.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin')
        self.child_buffer_size = 1  # catch last packet after C terminates, if MCU does ot send -260 this will stuck in loop
        self.packet_decode()
        child_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin',
                          'w')  # reset child so 3DHex.C to terminate
        child_file.close()
        savepathfile = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\support files\\savepath.txt', 'w')  # reset path
        savepathfile.close()
        buffer1_file.close()
        buffer2_file.close()
        self.window.usb_printing = 0

    def check_idle_commands(self):  # idle mode commands func
        if self.window.set_temp == 1:  # 1 Set NOZZLE temp
            self.window.set_temp = 0
            self.window.B = 0
            self.window.C = 0
            self.window.D = 0
            self.window.E = int(self.window.c26.isChecked())
            self.window.G = int(self.window.c22.isChecked())
            self.window.I = int(self.window.b35.toPlainText().strip())
            self.window.K = int(self.window.b39.toPlainText().strip())
            self.window.M = int(self.window.b37.toPlainText().strip())
            self.window.O = int(float(self.window.b41.toPlainText().strip()) * 10.0)
            self.window.P = int(float(self.window.b43.toPlainText().strip()) * 10.0)
            self.window.Q = int(float(self.window.b45.toPlainText().strip()) * 10.0)
            print(self.window.O)
            time.sleep(0.2)
            self.send_buffer()
            self.window.enable_idle_buttons()
        if self.window.set_temp == 2:  # 2 SET bed temp
            self.window.set_temp = 0
            self.window.B = 0
            self.window.C = 1
            self.window.D = 0
            self.window.E = int(self.window.c27.isChecked())
            self.window.G = int(self.window.c23.isChecked())
            self.window.I = int(self.window.b36.toPlainText().strip())
            self.window.K = int(self.window.b40.toPlainText().strip())
            self.window.M = int(self.window.b38.toPlainText().strip())
            self.window.O = int(float(self.window.b42.toPlainText().strip()) * 10.0)
            self.window.P = int(float(self.window.b44.toPlainText().strip()) * 10.0)
            self.window.Q = int(float(self.window.b46.toPlainText().strip()) * 10.0)
            time.sleep(0.2)
            self.send_buffer()
            self.window.enable_idle_buttons()
        if self.window.set_fan == 1:
            self.window.set_fan = 0
            self.window.B = 4
            self.window.J = int(self.window.d104.value())
            # print(self.window.J)
            self.send_buffer()
        if self.window.set_motor == 1:  # Enable/Disable motor
            self.window.set_motor = 0
            self.window.B = 1
            self.window.E = 0
            self.window.F = 0
            self.window.G = 0
            self.window.H = 0
            self.window.I = 0
            self.window.J = 0
            self.window.K = 0
            self.window.L = 0
            self.window.M = 0
            self.window.N = 0
            time.sleep(0.2)
            self.send_buffer()
            self.window.enable_idle_buttons()

        if self.window.home_axis != 0:  # Homing
            self.window.A = 0
            self.window.B = 2
            if self.window.home_axis == 1:
                self.window.C = int(self.window.c12.isChecked())  # HOME_X_ENABLE
                self.window.F = int(self.window.c17.isChecked())  # HOME_X_STATE
                self.window.O = int(1000000 / (int(self.window.b1.toPlainText().strip()) * self.window.d5.value() * 2))  # DURATION
                self.window.R = 1
                self.window.S = 0
                self.window.T = 0
            if self.window.home_axis == 2:
                self.window.D = int(self.window.c13.isChecked())  # HOME_Y_ENABLE
                self.window.G = int(self.window.c18.isChecked())  # HOME_Y_STATE
                self.window.P = int(1000000 / (int(self.window.b2.toPlainText().strip()) * self.window.d6.value() * 2))  # DURATION
                self.window.S = 1
                self.window.R = 0
                self.window.T = 0
            if self.window.home_axis == 3:
                self.window.E = int(self.window.c14.isChecked())  # HOME_Z_ENABLE
                self.window.K = int(self.window.c19.isChecked())  # HOME_Z_STATE
                self.window.Q = int(1000000 / (int(self.window.b3.toPlainText().strip()) * self.window.d7.value() * 2))  # DURATION
                self.window.T = 1
                self.window.R = 0
                self.window.S = 0
            time.sleep(0.2)
            self.send_buffer()
            if self.window.home_axis == 1:
                if self.window.L == 1:
                    self.window.p5.setEnabled(True)
            if self.window.home_axis == 2:
                if self.window.L == 1:
                    self.window.p9.setEnabled(True)
            if self.window.home_axis == 3:
                if self.window.L == 1:
                    self.window.p13.setEnabled(True)
            self.window.enable_idle_buttons()
            self.window.home_axis = 0

        if self.window.rapid_pos == 1:  # Rapid positioning command
            self.send_buffer()
            (self.serial_command, self.window.nozz_temp, self.window.bed_temp, self.window.X_POS, self.window.Y_POS,
             self.window.Z_POS,) = struct.unpack("3f3H", self.window.ser.read(18))  # wait for arduino to end process
            self.new_nozz_temp.emit(self.window.nozz_temp)  # emit the signal
            self.new_bed_temp.emit(self.window.bed_temp)  # emit the signal
            if self.window.C == 0:
                if self.window.F == 1:
                    self.window.p7.setEnabled(True)
                else:
                    self.window.p8.setEnabled(True)
            if self.window.C == 1:
                if self.window.F == 1:
                    self.window.p11.setEnabled(True)
                else:
                    self.window.p12.setEnabled(True)
            if self.window.C == 2:
                if self.window.F == 1:
                    self.window.p15.setEnabled(True)
                else:
                    self.window.p16.setEnabled(True)
            if self.window.C == 3:
                if self.window.F == 1:
                    self.window.p19.setEnabled(True)
                else:
                    self.window.p20.setEnabled(True)
            self.window.enable_idle_buttons()
            self.window.rapid_pos = 0

        if self.window.nozz_auto_tune == 1:
            self.window.nozz_auto_tune = 0
            if self.window.C == 1:
                self.message.emit(">>> NOZZLE AUTOTUNE")  # emit the signal
                print(self.window.B)
            else:
                self.message.emit(">>> BED AUTOTUNE")
            self.send_buffer()

        if self.window.bl_toggle == 1:  # Rapid positioning command
            self.window.bl_toggle = 0
            self.message.emit(">>> BL_TOUCH TOGGLE")
            self.send_buffer()
import time
import struct

from serial_controller.serial_manager.buffer_sender import BufferSender

class IdleCommando: #This class is imported from serial_manager package-thread
    
    def __init__(self, usbworker, main_window):
        super().__init__()
        self.main_window = main_window
        self.serial_manager = usbworker
        self.serialsender = BufferSender(self.main_window)
    
    def check_idle_commands(self):  # idle mode commands func
        if self.main_window.set_temp == 1:  # 1 Set NOZZLE temp
            self.main_window.set_temp = 0
            self.main_window.B = 0
            self.main_window.C = 0
            self.main_window.D = 0
            self.main_window.E = int(self.main_window.c26.isChecked())
            self.main_window.G = int(self.main_window.c22.isChecked())
            self.main_window.I = int(self.main_window.b35.toPlainText().strip())
            self.main_window.K = int(self.main_window.b39.toPlainText().strip())
            self.main_window.M = int(self.main_window.b37.toPlainText().strip())
            self.main_window.O = int(float(self.main_window.b41.toPlainText().strip()) * 10.0)
            self.main_window.P = int(float(self.main_window.b43.toPlainText().strip()) * 10.0)
            self.main_window.Q = int(float(self.main_window.b45.toPlainText().strip()) * 10.0)
            print(self.main_window.O)
            time.sleep(0.2)
            self.serialsender.send_buffer()
            self.main_window.widgethandler.enable_idle_buttons()
        if self.main_window.set_temp == 2:  # 2 SET bed temp
            self.main_window.set_temp = 0
            self.main_window.B = 0
            self.main_window.C = 1
            self.main_window.D = 0
            self.main_window.E = int(self.main_window.c27.isChecked())
            self.main_window.G = int(self.main_window.c23.isChecked())
            self.main_window.I = int(self.main_window.b36.toPlainText().strip())
            self.main_window.K = int(self.main_window.b40.toPlainText().strip())
            self.main_window.M = int(self.main_window.b38.toPlainText().strip())
            self.main_window.O = int(float(self.main_window.b42.toPlainText().strip()) * 10.0)
            self.main_window.P = int(float(self.main_window.b44.toPlainText().strip()) * 10.0)
            self.main_window.Q = int(float(self.main_window.b46.toPlainText().strip()) * 10.0)
            time.sleep(0.2)
            self.serialsender.send_buffer()
            self.main_window.widgethandler.enable_idle_buttons()
        if self.main_window.set_fan == 1:
            self.main_window.set_fan = 0
            self.main_window.B = 4
            self.main_window.J = int(self.main_window.d104.value())
            # print(self.main_window.J)
            self.serialsender.send_buffer()
        if self.main_window.set_motor == 1:  # Enable/Disable motor
            self.main_window.set_motor = 0
            self.main_window.B = 1
            self.main_window.E = 0
            self.main_window.F = 0
            self.main_window.G = 0
            self.main_window.H = 0
            self.main_window.I = 0
            self.main_window.J = 0
            self.main_window.K = 0
            self.main_window.L = 0
            self.main_window.M = 0
            self.main_window.N = 0
            time.sleep(0.2)
            self.serialsender.send_buffer()
            self.main_window.widgethandler.enable_idle_buttons()

        if self.main_window.home_axis != 0:  # Homing
            self.main_window.A = 0
            self.main_window.B = 2
            if self.main_window.home_axis == 1:
                self.main_window.C = int(self.main_window.c12.isChecked())  # HOME_X_ENABLE
                self.main_window.F = int(self.main_window.c17.isChecked())  # HOME_X_STATE
                self.main_window.O = int(1000000 / (int(self.main_window.b1.toPlainText().strip()) * self.main_window.d5.value() * 2))  # DURATION
                self.main_window.R = 1
                self.main_window.S = 0
                self.main_window.T = 0
            if self.main_window.home_axis == 2:
                self.main_window.D = int(self.main_window.c13.isChecked())  # HOME_Y_ENABLE
                self.main_window.G = int(self.main_window.c18.isChecked())  # HOME_Y_STATE
                self.main_window.P = int(1000000 / (int(self.main_window.b2.toPlainText().strip()) * self.main_window.d6.value() * 2))  # DURATION
                self.main_window.S = 1
                self.main_window.R = 0
                self.main_window.T = 0
            if self.main_window.home_axis == 3:
                self.main_window.E = int(self.main_window.c14.isChecked())  # HOME_Z_ENABLE
                self.main_window.K = int(self.main_window.c19.isChecked())  # HOME_Z_STATE
                self.main_window.Q = int(1000000 / (int(self.main_window.b3.toPlainText().strip()) * self.main_window.d7.value() * 2))  # DURATION
                self.main_window.T = 1
                self.main_window.R = 0
                self.main_window.S = 0
            time.sleep(0.2)
            self.serialsender.send_buffer()
            if self.main_window.home_axis == 1:
                if self.main_window.L == 1:
                    self.main_window.p5.setEnabled(True)
            if self.main_window.home_axis == 2:
                if self.main_window.L == 1:
                    self.main_window.p9.setEnabled(True)
            if self.main_window.home_axis == 3:
                if self.main_window.L == 1:
                    self.main_window.p13.setEnabled(True)
            self.main_window.widgethandler.enable_idle_buttons()
            self.main_window.home_axis = 0

        if self.main_window.rapid_pos == 1:  # Rapid positioning command
            self.serialsender.send_buffer()
            (self.serial_manager.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS, self.main_window.Y_POS,
             self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(18))  # wait for arduino to end process
            self.serial_manager.new_nozz_temp.emit(self.main_window.nozz_temp)  # emit the signal
            self.serial_manager.new_bed_temp.emit(self.main_window.bed_temp)  # emit the signal
            if self.main_window.C == 0:
                if self.main_window.F == 1:
                    self.main_window.p7.setEnabled(True)
                else:
                    self.main_window.p8.setEnabled(True)
            if self.main_window.C == 1:
                if self.main_window.F == 1:
                    self.main_window.p11.setEnabled(True)
                else:
                    self.main_window.p12.setEnabled(True)
            if self.main_window.C == 2:
                if self.main_window.F == 1:
                    self.main_window.p15.setEnabled(True)
                else:
                    self.main_window.p16.setEnabled(True)
            if self.main_window.C == 3:
                if self.main_window.F == 1:
                    self.main_window.p19.setEnabled(True)
                else:
                    self.main_window.p20.setEnabled(True)
            self.main_window.widgethandler.enable_idle_buttons()
            self.main_window.rapid_pos = 0

        if self.main_window.nozz_auto_tune == 1:
            self.main_window.nozz_auto_tune = 0
            if self.main_window.C == 1:
                self.serial_manager.message.emit(">>> NOZZLE AUTOTUNE")  # emit the signal 
                print(self.main_window.B)
            else:
                self.serial_manager.message.emit(">>> BED AUTOTUNE")
            self.serialsender.send_buffer()

        if self.main_window.bl_toggle == 1:  # Rapid positioning command
            self.main_window.bl_toggle = 0
            self.serial_manager.message.emit(">>> BL_TOUCH TOGGLE")
            self.serialsender.send_buffer()
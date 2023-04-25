import struct

class BufferSender:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def send_buffer(self):
        go = 3
        pin = self.main_window.Pin_Button_0.text()
        if pin == 'N':
            X_ENABLE_PIN = 255
        else:
            X_ENABLE_PIN = int(pin)
        pin = self.main_window.Pin_Button_1.text()
        if pin == 'N':
            X_STEP_PIN = 255
        else:
            X_STEP_PIN = int(pin)
        pin = self.main_window.Pin_Button_2.text()
        if pin == 'N':
            X_DIR_PIN = 255
        else:
            X_DIR_PIN = int(pin)
        pin = self.main_window.Pin_Button_3.text()
        if pin == 'N':
            X_HOME_PIN = 255
        else:
            X_HOME_PIN = int(pin)

        pin = self.main_window.Pin_Button_4.text()
        if pin == 'N':
            Y_ENABLE_PIN = 255
        else:
            Y_ENABLE_PIN = int(pin)
        pin = self.main_window.Pin_Button_5.text()
        if pin == 'N':
            Y_STEP_PIN = 255
        else:
            Y_STEP_PIN = int(pin)
        pin = self.main_window.Pin_Button_6.text()
        if pin == 'N':
            Y_DIR_PIN = 255
        else:
            Y_DIR_PIN = int(pin)
        pin = self.main_window.Pin_Button_7.text()
        if pin == 'N':
            Y_HOME_PIN = 255
        else:
            Y_HOME_PIN = int(pin)

        pin = self.main_window.Pin_Button_8.text()
        if pin == 'N':
            Z_ENABLE_PIN = 255
        else:
            Z_ENABLE_PIN = int(pin)
        pin = self.main_window.Pin_Button_9.text()
        if pin == 'N':
            Z_STEP_PIN = 255
        else:
            Z_STEP_PIN = int(pin)
        pin = self.main_window.Pin_Button_10.text()
        if pin == 'N':
            Z_DIR_PIN = 255
        else:
            Z_DIR_PIN = int(pin)
        pin = self.main_window.Pin_Button_11.text()
        if pin == 'N':
            Z_HOME_PIN = 255
        else:
            Z_HOME_PIN = int(pin)

        pin = self.main_window.Pin_Button_12.text()
        if pin == 'N':
            Z1_ENABLE_PIN = 255
        else:
            Z1_ENABLE_PIN = int(pin)
        pin = self.main_window.Pin_Button_13.text()
        if pin == 'N':
            Z1_STEP_PIN = 255
        else:
            Z1_STEP_PIN = int(pin)
        pin = self.main_window.Pin_Button_14.text()
        if pin == 'N':
            Z1_DIR_PIN = 255
        else:
            Z1_DIR_PIN = int(pin)
        pin = self.main_window.Pin_Button_15.text()
        if pin == 'N':
            Z1_HOME_PIN = 255
        else:
            Z1_HOME_PIN = int(pin)

        pin = self.main_window.Pin_Button_16.text()
        if pin == 'N':
            E_ENABLE_PIN = 255
        else:
            E_ENABLE_PIN = int(pin)
        pin = self.main_window.Pin_Button_17.text()
        if pin == 'N':
            E_STEP_PIN = 255
        else:
            E_STEP_PIN = int(pin)
        pin = self.main_window.Pin_Button_18.text()
        if pin == 'N':
            E_DIR_PIN = 255
        else:
            E_DIR_PIN = int(pin)
        pin = self.main_window.Pin_Button_19.text()
        if pin == 'N':
            E_HOME_PIN = 255
        else:
            E_HOME_PIN = int(pin)

        pin = self.main_window.Pin_Button_20.text()
        if pin == 'N':
            N_HEATER_PIN = 255
        else:
            N_HEATER_PIN = int(pin)
        pin = self.main_window.Pin_Button_21.text()
        if pin == 'N':
            N_SENSOR_PIN = 255
        else:
            N_SENSOR_PIN = int(pin)
        pin = self.main_window.Pin_Button_22.text()
        if pin == 'N':
            N_FAN_PIN = 255
        else:
            N_FAN_PIN = int(pin)
        pin = self.main_window.Pin_Button_23.text()
        if pin == 'N':
            B_HEATER_PIN = 255
        else:
            B_HEATER_PIN = int(pin)
        pin = self.main_window.Pin_Button_24.text()
        if pin == 'N':
            B_SENSOR_PIN = 255
        else:
            B_SENSOR_PIN = int(pin)
        pin = self.main_window.Pin_Button_25.text()
        if pin == 'N':
            FAN_PIN = 255
        else:
            FAN_PIN = int(pin)
        pin = self.main_window.Pin_Button_26.text()
        if pin == 'N':
            BL_PIN = 255
        else:
            BL_PIN = int(pin)
        pin = self.main_window.Pin_Button_27.text()
        if pin == 'N':
            SERVO1_PIN = 255
        else:
            SERVO1_PIN = int(pin)
        pin = self.main_window.Pin_Button_28.text()
        if pin == 'N':
            SERVO2_PIN = 255
        else:
            SERVO2_PIN = int(pin)
        pin = self.main_window.Pin_Button_29.text()
        if pin == 'N':
            RS_PIN = 255
        else:
            RS_PIN = int(pin)
        pin = self.main_window.Pin_Button_30.text()
        if pin == 'N':
            LCD_ENABLE_PIN = 255
        else:
            LCD_ENABLE_PIN = int(pin)
        pin = self.main_window.Pin_Button_31.text()
        if pin == 'N':
            D4_PIN = 255
        else:
            D4_PIN = int(pin)
        pin = self.main_window.Pin_Button_32.text()
        if pin == 'N':
            D5_PIN = 255
        else:
            D5_PIN = int(pin)
        pin = self.main_window.Pin_Button_33.text()
        if pin == 'N':
            D6_PIN = 255
        else:
            D6_PIN = int(pin)
        pin = self.main_window.Pin_Button_34.text()
        if pin == 'N':
            D7_PIN = 255
        else:
            D7_PIN = int(pin)
        pin = self.main_window.Pin_Button_35.text()
        if pin == 'N':
            BTEN1_PIN = 255
        else:
            BTEN1 = int(pin)
        pin = self.main_window.Pin_Button_36.text()
        if pin == 'N':
            BTEN2_PIN = 255
        else:
            BTEN2_PIN = int(pin)
        pin = self.main_window.Pin_Button_37.text()
        if pin == 'N':
            BTENC_PIN = 255
        else:
            BTENC_PIN = int(pin)
        pin = self.main_window.Pin_Button_38.text()
        if pin == 'N':
            SD_CS_PIN = 255
        else:
            SD_CS_PIN = int(pin)
        pin = self.main_window.Pin_Button_39.text()
        if pin == 'N':
            SD_DET_PIN = 255
        else:
            SD_DET_PIN = int(pin)
        self.main_window.ser.write(struct.pack("4B", 0, 0, 0, 5))
        (pass_fail,) = struct.unpack("B", self.main_window.ser.read(1))  # Wait for arduino to confirm everything is ok
        self.main_window.ser.write(
            struct.pack("2b26h8B4H2B9H", go, go, X_ENABLE_PIN, X_STEP_PIN, X_DIR_PIN, X_HOME_PIN, Y_ENABLE_PIN,
                        Y_STEP_PIN, Y_DIR_PIN, Y_HOME_PIN, Z_ENABLE_PIN, Z_STEP_PIN, Z_DIR_PIN, Z_HOME_PIN,
                        Z1_ENABLE_PIN, Z1_STEP_PIN, Z1_DIR_PIN, Z1_HOME_PIN, E_ENABLE_PIN, E_STEP_PIN, E_DIR_PIN,
                        E_HOME_PIN, N_HEATER_PIN, N_SENSOR_PIN, N_FAN_PIN, B_HEATER_PIN, B_SENSOR_PIN, FAN_PIN,
                        self.main_window.A, self.main_window.B, self.main_window.C, self.main_window.D, self.main_window.E,
                        self.main_window.F, self.main_window.G, self.main_window.H, self.main_window.I,
                        self.main_window.J, self.main_window.K, self.main_window.L, self.main_window.M, self.main_window.N,
                        self.main_window.O, self.main_window.P, self.main_window.Q, self.main_window.R,
                        self.main_window.S, self.main_window.T, self.main_window.U, self.main_window.V, self.main_window.W))
        (pass_fail,) = struct.unpack("B", self.main_window.ser.read(1))  # Wait for arduino to confirm everything is ok
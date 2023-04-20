import serial.tools.list_ports

#********************LOCAL IMPORTS**************************
from usb.usbhandler import USBWorker

def CONNECT(self):
    try:
        if self.USB_CONNECTED == 0:
            self.save_settings()
            COM_PORT = self.chosenPort
            BAUD_RATE = int(self.b48.toPlainText().strip())
            self.ser = serial.Serial(COM_PORT, BAUD_RATE)  # first time communicate with Printer
            self.ser.flushOutput()
            self.ser.flushInput()
            self.A = 0  # A=0 => Idle mode
            self.B = 0  # B=0 => Temperature command
            self.C = 0  # C=0 => Report only temperature
            self.stepx_pos = 0
            self.x_pos_last = 0
            self.sum_Xpos = 0
            self.x_overflow = 0
            self.stepy_pos = 0
            self.y_pos_last = 0
            self.sum_Ypos = 0
            self.y_overflow = 0
            self.stepz_pos = 0
            self.z_pos_last = 0
            self.sum_Zpos = 0
            self.z_overflow = 0
            self.usb_thread = USBWorker()
            self.usb_thread.instance_main(self)
            self.usb_thread.message.connect(self.print2user_usb)  # connect thread to message window
            self.usb_thread.new_nozz_temp.connect(self.update_nozz_temp)  # connect thread to message window
            self.usb_thread.new_bed_temp.connect(self.update_bed_temp)  # connect thread to message window
            self.usb_thread.x_pos_report.connect(self.update_xpos)
            self.usb_thread.y_pos_report.connect(self.update_ypos)
            self.usb_thread.z_pos_report.connect(self.update_zpos)
            self.usb_thread.autotune_p.connect(self.test)  # connect thread to autotune window
            self.usb_thread.start()  # Start usb communication handling thread
            self.enable_rapid_buttons()  # Enable home buttons
            for i in range(2, 6):  # c0-cmax
                c = getattr(self, "c{}".format(
                    i))  # self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                c.setChecked(1)
            self.USB_CONNECTED = 1
            self.Message_panel.append(">>> Successfully connected to: " + str(COM_PORT))
            self.p1.setStyleSheet('QPushButton {background-color:rgb(47, 47, 47);color: rgb(0,255,0);border-style: solid;border-radius:5px;border-width:2px; border-color:rgb(0,255,0);}\
            QPushButton:hover {border-width:3px; border-color:rgb(255, 115, 30);}\
            QPushButton:pressed {background-color:rgb(255, 115, 30);border-color: rgb(255, 195, 110);border-width: 4px;}\
            QPushButton:disabled{background-color: rgb(255, 149, 62);color: rgb(83, 83, 83);border-color:rgb(83, 83, 83);}')
        else:
            if self.usb_printing == 0:  # disconnect only allowed when on idle
                self.p1.setStyleSheet('QPushButton {background-color:rgb(47, 47, 47);color: rgb(255,255,255);border-style: solid;border-radius:5px;border-width:2px; border-color:rgb(255,85,0);}\
                QPushButton:hover {border-width:3px; border-color:rgb(255, 115, 30);}\
                QPushButton:pressed {background-color:rgb(255, 115, 30);border-color: rgb(255, 195, 110);border-width: 4px;}\
                QPushButton:disabled{background-color: rgb(255, 149, 62);color: rgb(83, 83, 83);border-color:rgb(83, 83, 83);}')
                self.USB_CONNECTED = 0
                self.ser.close()
                self.disable_rapid_buttons()
                self.Message_panel.append(">>> Printer disconnected")
                self.XPOSITION.setText("{:.3f}".format(round(0, 3)))
                self.YPOSITION.setText("{:.3f}".format(round(0, 3)))
                self.ZPOSITION.setText("{:.3f}".format(round(0, 3)))
                self.NOZZ_TEMP.setText("{:.2f}".format(round(0, 2)))
                self.BED_TEMP.setText("{:.2f}".format(round(0, 2)))
    except:
        self.Message_panel.append(">>> Failed to connect")
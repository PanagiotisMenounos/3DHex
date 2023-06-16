import serial.tools.list_ports

from serial_controller.serial_manager.main_serial_manager import USBWorker

class UsbConnect:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

        self.usb_thread = USBWorker(self.main_window)

    def CONNECT(self):
        try:
            if self.main_window.USB_CONNECTED==0:            
                self.main_window.filehandler.save_settings()
                COM_PORT = self.main_window.chosenPort
                BAUD_RATE = int(self.main_window.b48.toPlainText().strip())
                self.main_window.ser=serial.Serial(COM_PORT,BAUD_RATE) #first time communicate with Printer
                self.main_window.ser.flushOutput()
                self.main_window.ser.flushInput() 
                self.main_window.A=0 #A=0 => Idle mode
                self.main_window.B=0 #B=0 => Temperature command
                self.main_window.C=0 #C=0 => Report only temperature
                self.main_window.stepx_pos=0
                self.main_window.x_pos_last=0
                self.main_window.sum_Xpos =0
                self.main_window.x_overflow=0    
                self.main_window.stepy_pos=0
                self.main_window.y_pos_last=0
                self.main_window.sum_Ypos =0
                self.main_window.y_overflow=0   
                self.main_window.stepz_pos=0
                self.main_window.z_pos_last=0
                self.main_window.sum_Zpos =0
                self.main_window.z_overflow=0                 
                self.main_window.usb_thread = USBWorker(self.main_window)
                self.main_window.usb_thread.message.connect(self.main_window.print2user_usb) #connect thread to message window
                self.main_window.usb_thread.new_nozz_temp.connect(self.main_window.widgethandler.update_nozz_temp) #connect thread to message window
                self.main_window.usb_thread.new_bed_temp.connect(self.main_window.widgethandler.update_bed_temp) #connect thread to message window
                self.main_window.usb_thread.x_pos_report.connect(self.main_window.widgethandler.update_xpos)
                self.main_window.usb_thread.y_pos_report.connect(self.main_window.widgethandler.update_ypos)
                self.main_window.usb_thread.z_pos_report.connect(self.main_window.widgethandler.update_zpos)
                self.main_window.usb_thread.autotune_p.connect(self.main_window.popuphandler.test) #connect thread to autotune window
                self.main_window.usb_thread.start()       #Start usb communication handling thread
                self.main_window.widgethandler.enable_rapid_buttons()    #Enable home buttons
                for i in range (2,6): #c0-cmax
                     c = getattr(self.main_window, "c{}".format(i))    #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                     c.setChecked(1) 
                self.main_window.USB_CONNECTED=1
                self.main_window.Message_panel.append(">>> Successfully connected to: " + str(COM_PORT))
                self.main_window.p1.setStyleSheet('QPushButton {background-color:rgb(47, 47, 47);color: rgb(0,255,0);border-style: solid;border-radius:5px;border-width:2px; border-color:rgb(0,255,0);}\
                QPushButton:hover {border-width:3px; border-color:rgb(255, 115, 30);}\
                QPushButton:pressed {background-color:rgb(255, 115, 30);border-color: rgb(255, 195, 110);border-width: 4px;}\
                QPushButton:disabled{background-color: rgb(255, 149, 62);color: rgb(83, 83, 83);border-color:rgb(83, 83, 83);}')
            else:
                if self.main_window.usb_printing==0: #disconnect only allowed when on idle
                    self.main_window.p1.setStyleSheet('QPushButton {background-color:rgb(47, 47, 47);color: rgb(255,255,255);border-style: solid;border-radius:5px;border-width:2px; border-color:rgb(255,85,0);}\
                    QPushButton:hover {border-width:3px; border-color:rgb(255, 115, 30);}\
                    QPushButton:pressed {background-color:rgb(255, 115, 30);border-color: rgb(255, 195, 110);border-width: 4px;}\
                    QPushButton:disabled{background-color: rgb(255, 149, 62);color: rgb(83, 83, 83);border-color:rgb(83, 83, 83);}')
                    self.main_window.USB_CONNECTED=0
                    self.main_window.ser.close()
                    self.main_window.widgethandler.disable_rapid_buttons()
                    self.main_window.Message_panel.append(">>> Printer disconnected")
                    self.main_window.XPOSITION.setText("{:.3f}".format(round(0, 3)))
                    self.main_window.YPOSITION.setText("{:.3f}".format(round(0, 3)))
                    self.main_window.ZPOSITION.setText("{:.3f}".format(round(0, 3)))
                    self.main_window.NOZZ_TEMP.setText("{:.2f}".format(round(0, 2)))
                    self.main_window.BED_TEMP.setText("{:.2f}".format(round(0, 2)))
        except:
            self.main_window.Message_panel.append(">>> Failed to connect")	
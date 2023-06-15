from printer_controller.fly_commander import FLYWorker

class TempConctrols:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def setNOZZTEMP(self):
        if self.main_window.A==0:
           self.main_window.disable_idle_buttons()
           self.main_window.set_temp=1
        if self.main_window.usb_printing==1 and self.main_window.USB_CONNECTED==1 and self.main_window.froze==0 and self.main_window.froze_loop==0:
            self.main_window.froze=1
            self.main_window.MM=104
            self.main_window.SS=float(self.main_window.b35.toPlainText().strip())
            self.main_window.Message_panel.append(">>> M104 S"+str(self.main_window.SS)+": Set NOZZLE temp ")
            self.main_window.fly_thread = FLYWorker(self.main_window)
            self.main_window.fly_thread.start()

    def setBEDTEMP(self):
        if self.main_window.A==0:
            self.main_window.disable_idle_buttons()
            self.main_window.set_temp=2
        if self.main_window.usb_printing==1 and self.main_window.USB_CONNECTED==1 and self.main_window.froze==0 and self.main_window.froze_loop==0:
            self.main_window.froze=1
            self.main_window.MM=140
            self.main_window.SS=float(self.main_window.b36.toPlainText().strip())
            self.main_window.Message_panel.append(">>> M140 S"+str(self.main_window.SS)+": Set BED temp ")
            self.main_window.fly_thread = FLYWorker(self.main_window)
            self.main_window.fly_thread.start()

    def setFAN1(self):
        if self.main_window.A==0:
            self.main_window.set_fan=1
        if self.main_window.usb_printing==1 and self.main_window.USB_CONNECTED==1 and self.main_window.froze==0 and self.main_window.froze_loop==0:
            self.main_window.froze=1
            self.main_window.MM=106
            self.main_window.SS=float(self.main_window.d104.value())
            self.main_window.Message_panel.append(">>> M106: Set FAN1 ")
            self.main_window.fly_thread = FLYWorker(self.main_window)
            self.main_window.fly_thread.start()
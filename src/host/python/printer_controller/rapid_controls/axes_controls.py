from PyQt5.QtCore import Qt

class RapidAxesController:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def setXmotor(self,state):
        self.main_window.C=0
        if state == Qt.Checked:
            self.main_window.D=1
            if self.main_window.USB_CONNECTED==1:
                if self.main_window.c12.isChecked():
                    self.main_window.p5.setEnabled(True)
                self.main_window.p7.setEnabled(True)
                self.main_window.p8.setEnabled(True)
        else:
            self.main_window.D=0
            if self.main_window.USB_CONNECTED==1:
                self.main_window.p5.setEnabled(False)
                self.main_window.p7.setEnabled(False)
                self.main_window.p8.setEnabled(False)
        self.main_window.widgethandler.disable_idle_buttons()
        self.main_window.set_motor=1

    def setYmotor(self,state):
        self.main_window.C=1
        if state == Qt.Checked:
            self.main_window.D=1
            if self.main_window.USB_CONNECTED==1:
                if self.main_window.c13.isChecked():
                    self.main_window.p9.setEnabled(True)
                self.main_window.p11.setEnabled(True)
                self.main_window.p12.setEnabled(True)
        else:
            self.main_window.D=0
            if self.main_window.USB_CONNECTED==1:
                self.main_window.p9.setEnabled(False)
                self.main_window.p11.setEnabled(False)
                self.main_window.p12.setEnabled(False)
        self.main_window.widgethandler.disable_idle_buttons()
        self.main_window.set_motor=1

    def setZmotor(self,state):
        self.main_window.C=2
        if state == Qt.Checked:
            self.main_window.D=1
            if self.main_window.USB_CONNECTED==1:
                if self.main_window.c14.isChecked():
                    self.main_window.p13.setEnabled(True)
                self.main_window.p15.setEnabled(True)
                self.main_window.p16.setEnabled(True)
        else:
            self.main_window.D=0
            if self.main_window.USB_CONNECTED==1:
                self.main_window.p13.setEnabled(False)
                self.main_window.p15.setEnabled(False)
                self.main_window.p16.setEnabled(False)
        self.main_window.widgethandler.disable_idle_buttons()
        self.main_window.set_motor=1
        
    def setEmotor(self,state):
        self.main_window.C=3
        if state == Qt.Checked:
            self.main_window.D=1
            if self.main_window.USB_CONNECTED==1:
                if self.main_window.c15.isChecked():
                    self.main_window.p17.setEnabled(True)
                    self.main_window.p18.setEnabled(True)
                self.main_window.p19.setEnabled(True)
                self.main_window.p20.setEnabled(True)
        else:
            self.main_window.D=0
            if self.main_window.USB_CONNECTED==1:
                self.main_window.p17.setEnabled(False)
                self.main_window.p18.setEnabled(False)
                self.main_window.p19.setEnabled(False)
                self.main_window.p20.setEnabled(False)
        self.main_window.widgethandler.disable_idle_buttons()
        self.main_window.set_motor=1
        
    def HOME_X_MIN(self):
        if self.main_window.home_axis==0 and self.main_window.rapid_pos==0 and self.main_window.A==0:
            self.main_window.L=1
            self.main_window.widgethandler.disable_idle_buttons()
            self.main_window.I = int(self.main_window.c7.isChecked())                                      #HOME_X_DIRECTION
            if self.main_window.I==1:
               self.main_window.I=0
            else:
               self.main_window.I=1 
            self.main_window.p5.setEnabled(False)
            self.main_window.home_axis=1
            
    def HOME_Y_MIN(self):
        if self.main_window.home_axis==0 and self.main_window.rapid_pos==0 and self.main_window.A==0:
            self.main_window.L=1
            self.main_window.widgethandler.disable_idle_buttons()
            self.main_window.J = int(self.main_window.c8.isChecked())                                      #HOME_X_DIRECTION
            if self.main_window.J==1:
               self.main_window.J=0
            else:
               self.main_window.J=1 
            self.main_window.p9.setEnabled(False)
            self.main_window.home_axis=2
            
    def HOME_Z_MIN(self):
        if self.main_window.home_axis==0 and self.main_window.rapid_pos==0 and self.main_window.A==0:
            self.main_window.L=1
            self.main_window.widgethandler.disable_idle_buttons()
            self.main_window.K = int(self.main_window.c9.isChecked())                                      #HOME_X_DIRECTION
            if self.main_window.K==1:
               self.main_window.K=0
            else:
               self.main_window.K=1 
            self.main_window.p13.setEnabled(False)
            self.main_window.home_axis=3

    def rapid_idle_position(self,axis,dir):
        if self.main_window.home_axis==0 and self.main_window.rapid_pos==0 and self.main_window.A==0:
            self.main_window.widgethandler.disable_idle_buttons()
            self.main_window.A=0
            self.main_window.B=3
            self.main_window.C=axis
            self.main_window.F=dir #only for python
            if axis == 0:
               self.main_window.I=int(1000000/(int(self.main_window.b1.toPlainText().strip())*self.main_window.d5.value()*2))  #delay time
               self.main_window.J=int(self.main_window.d1.value()*int(self.main_window.b1.toPlainText().strip()))
               self.main_window.D=int(self.main_window.c2.isChecked()) #Enabled/Disabled
               self.main_window.E=int(self.main_window.c7.isChecked()) #Direction
               if dir == 1:                    #-X command
                   self.main_window.p7.setEnabled(False)
                   if self.main_window.E==1:
                       self.main_window.E=0                #Direction
                       self.main_window.InvertX = -1
                       self.main_window.rapid_X=-1
                   else:
                       self.main_window.E=1
                       self.main_window.InvertX = 1
                       self.main_window.rapid_X=-1
               else:
                   self.main_window.p8.setEnabled(False)
                   self.main_window.rapid_X=1
            if axis == 1:
               self.main_window.I=int(1000000/(int(self.main_window.b2.toPlainText().strip())*self.main_window.d6.value()*2))  #delay time
               self.main_window.J=int(self.main_window.d2.value()*int(self.main_window.b2.toPlainText().strip()))
               self.main_window.D=int(self.main_window.c3.isChecked()) #Enabled/Disabled
               self.main_window.E=int(self.main_window.c8.isChecked()) #Direction
               if dir == 1:                    #-Y command
                   self.main_window.p11.setEnabled(False)
                   if self.main_window.E==1:
                       self.main_window.E=0                #Direction
                       self.main_window.rapid_Y=-1
                   else:
                       self.main_window.E=1
                       self.main_window.rapid_Y=-1
               else:
                   self.main_window.p12.setEnabled(False)
                   self.main_window.rapid_Y=1
            if axis == 2:
               self.main_window.I=int(1000000/(int(self.main_window.b3.toPlainText().strip())*self.main_window.d7.value()*2))  #delay time
               self.main_window.J=int(self.main_window.d3.value()*int(self.main_window.b3.toPlainText().strip()))
               self.main_window.D=int(self.main_window.c4.isChecked()) #Enabled/Disabled
               self.main_window.E=int(self.main_window.c9.isChecked()) #Direction
               if dir == 1:                    #-Z command
                   self.main_window.p15.setEnabled(False)
                   if self.main_window.E==1:
                       self.main_window.E=0                #Direction
                       self.main_window.rapid_Z=-1
                   else:
                       self.main_window.E=1
                       self.main_window.rapid_Z=-1
               else:
                   self.main_window.p16.setEnabled(False)
                   self.main_window.rapid_Z=1
            if axis == 3:
               self.main_window.I=int(1000000/(int(self.main_window.b4.toPlainText().strip())*self.main_window.d8.value()*2))  #delay time
               self.main_window.J=int(self.main_window.d4.value()*int(self.main_window.b4.toPlainText().strip()))
               self.main_window.D=int(self.main_window.c5.isChecked()) #Enabled/Disabled
               self.main_window.E=int(self.main_window.c10.isChecked()) #Direction
               if dir == 1:                    #-E command
                   self.main_window.p19.setEnabled(False)
                   if self.main_window.E==1:
                       self.main_window.E=0                #Direction
                   else:
                       self.main_window.E=1
               else:
                   self.main_window.p20.setEnabled(False)
            self.main_window.rapid_pos=1
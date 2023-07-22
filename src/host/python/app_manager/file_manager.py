#*******************EXTERNAL IMPORTS************************
import os
import easygui

class FileManager:
    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def openfile(self):#call this function whenever file->open is pressed
        self.main_window.Message_panel.append(">>> select GCODE...loading...")
        path = easygui.fileopenbox() #open window to select file and save path
        if path != None:
           self.main_window.Message_panel.append(">>> Successfully loaded: "+path)
           #self.main_window.file1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\support files\\GCODE.txt','w')
           self.main_window.file = open(path) #open this file
           self.main_window.data = self.main_window.file.read() #read the file
           self.main_window.GCODE_Panel.setPlainText(self.main_window.data)
           self.main_window.file.close() #close the file
           #self.main_window.file1.write(self.main_window.data)
           #self.file1.close()
    def save_settings(self):
        bfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\boxes settings.txt','w')
        cfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\cboxes settings.txt','w')
        dfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\dboxes settings.txt','w')
        cbfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\cbboxes settings.txt','w')
        pinsfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\pins settings.txt','w')
        b_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\boxes settings.txt','w')
        c_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cboxes settings.txt','w')
        d_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\dboxes settings.txt','w')
        cb_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\cbboxes settings.txt','w')
        pins_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\pins settings.txt','w')
        i=0
        for i in range (0,71):
            try:
                b = getattr(self.main_window, "b{}".format(i)) #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                text = b.toPlainText().strip() #strip() removes'/n'
                if (text==''): #check if it is aan empty string
                  bfile.write("\n")
                  b_file.write("\n")
                else:
                  bfile.write(text+"\n")
                  b_file.write(text+"\n")
            except:
                print("b")
        i=1
        for i in range (1,30): #c0-cmax
            try:
                c = getattr(self.main_window, "c{}".format(i))#self.main_window.c[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
                check = c.isChecked()
                if check==0:
                    cfile.write("0\n")
                    c_file.write("0\n")
                else:
                    cfile.write("1\n")
                    c_file.write("1\n")
            except:
                print("c")
        i=1
        for i in range (1,9): #c0-cmax
            try:
                d = getattr(self.main_window, "d{}".format(i))
                value = d.value()
                dfile.write(str(value)+"\n")
                d_file.write(str(value)+"\n")
            except:
                print("d")
        i=1
        for i in range (1,4): #c0-cmax
            try:
                cb = getattr(self.main_window, "comboBox{}".format(i))
                value = cb.currentIndex()
                cbfile.write(str(value)+"\n")
                cb_file.write(str(value)+"\n")
            except:
                print("cb")
        i=0
        for i in range (0,40):
            try:
                pin = getattr(self.main_window, "Pin_Button_{}".format(i))
                value = pin.text()
                if value == 'N':
                    pinsfile.write("100\n")
                    pins_file.write("100\n")
                else:
                    pinsfile.write(str(value)+"\n")
                    pins_file.write(str(value)+"\n")
            except:
                print("pin")
        bfile.close()
        cfile.close()
        dfile.close()
        cbfile.close()
        pinsfile.close()
        b_file.close()
        c_file.close()
        d_file.close()
        cb_file.close()
        pins_file.close()

    def load_settings(self):
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\boxes settings.txt','r') #read general setting file and set them
        boxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\cboxes settings.txt','r') #read general setting file and set them
        cboxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\dboxes settings.txt','r') #read general setting file and set them
        dboxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\cbboxes settings.txt','r') #read general setting file and set them
        cbboxes = file.readlines()
        file.close()
        file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\pins settings.txt','r') #read general setting file and set them
        pins = file.readlines()
        file.close()
        try:
            file = open(os.getenv('LOCALAPPDATA')+'\\3DHex2\\settings\\Printer' + str(self.main_window.printer) + '\\abl_type.txt','r') #read general setting file and set them
            abldata = file.readlines()
            self.main_window.ABL_INTERPOLATION_TYPE = int(abldata[0].strip())
            if self.main_window.ABL_INTERPOLATION_TYPE==1:
                self.main_window.m1.setPlainText('PLANE')
            elif self.main_window.ABL_INTERPOLATION_TYPE==2:
                self.main_window.m1.setPlainText('MESH')
            elif self.main_window.ABL_INTERPOLATION_TYPE==0:
                self.main_window.m1.setPlainText('NO DATA')
        except:
            self.main_window.m1.setPlainText('NO DATA')
        for i in range (0,71): #b0-bmax
           b = getattr(self.main_window, "b{}".format(i))    #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           b.setPlainText('')
           b.insertPlainText(boxes[i].strip()) #strip() removes'/n'
        for i in range (2,6): #c0-cmax
           c = getattr(self.main_window, "c{}".format(i))    #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           c.setChecked(1)                       #set Enable/Disable cboxes always on at start
        for i in range (7,30): #c0-cmax
           c = getattr(self.main_window, "c{}".format(i))    #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           c.setChecked(int(cboxes[i-1].strip())) #strip() removes'/n'
        for i in range (1,9): #c0-cmax
           d = getattr(self.main_window, "d{}".format(i))    #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           d.setValue(float(dboxes[i-1].strip())) #strip() removes'/n'
        for i in range (1,4): #c0-cmax
           cb = getattr(self.main_window, "comboBox{}".format(i))    #self.main_window.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
           cb.setCurrentIndex(int(cbboxes[i-1].strip()))
        for i in range (0,40): #c0-cmax
            pin = getattr(self.main_window, "Pin_Button_{}".format(i))
            val = int(pins[i].strip())
            if val == 100:
                pin.setText('N')
            else:
                pin.setText(str(val))
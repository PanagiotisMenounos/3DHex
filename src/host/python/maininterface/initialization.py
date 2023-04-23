import os
import serial.tools.list_ports

class self_init(object):
    def declare_vars(self):
        self.MM = 0
        self.SS = 0
        self.froze_loop = 0  # prevent fly command when paused
        self.froze = 0  # prevent pause to be pressed multiple times
        self.pause_state = 0  # not at pause state
        self.mirror = 5  # used as flag in C-Python
        self.printer = 0
        self.set_motor = 0
        self.USB_CONNECTED = 0
        self.usb_connected = 1
        self.set_temp = 0
        self.set_fan = 0
        self.home_axis = 0
        self.usb_printing = 0
        self.rapid_pos = 0
        self.rapid_X = 0
        self.rapid_Y = 0
        self.rapid_Z = 0
        self.nozz_auto_tune = 0
        self.InvertX_tongle = 1
        self.InvertY_tongle = 1
        self.InvertZ_tongle = 1
        self.bl_toggle = 0
        self.BL_TOUCH_STATE = 0
        self.ABL_INTERPOLATION_TYPE = 0
        self.A = 0
        self.B = 0
        self.C = 0
        self.D = 0
        self.E = 0
        self.F = 0
        self.G = 0
        self.H = 0
        self.I = 0
        self.J = 0
        self.K = 0
        self.L = 0
        self.M = 0
        self.N = 0
        self.O = 0
        self.P = 0
        self.Q = 0
        self.R = 0
        self.S = 0
        self.T = 0
        self.U = 0
        self.V = 0
        self.W = 0
        self.Auto_P = 0
        self.Auto_I = 0
        self.Auto_D = 0
        self.update_temp = 0
        self.ABL = 0
        self.iterations = 1.0
        self.STPZ = 100
        self.ABL_Z_CENTER = 0
        self.ABL_Sample = 0
        self.plot_num = 0
        self.min = 0
        self.x_pos_last = 0
        self.stepx_direction = 1
        self.stepy_direction = 1
        self.stepz_direction = 1
        self.stepx_pos = 0
        self.stepy_pos = 0
        self.stepz_pos = 0
        self.x_overflow = 0
        self.sum_Xpos = 0
        self.sum_Ypos = 0
        self.sum_Zpos = 0
        self.x_overflow_last = 0
        self.y_pos_last = 0
        self.y_overflow = 0
        self.y_overflow_last = 0
        self.z_pos_last = 0
        self.z_overflow = 0
        self.z_overflow_last = 0
        self.int16_base = 0
        self.chosenPort = ""
        self.InvertX = 1
        self.InvertY = 1
        self.InvertZ = 1
        self.ports = serial.tools.list_ports.comports()
        self.comboBox.addItem("")
        for p in self.ports:
            self.comboBox.addItem(p.device)

    def load_printers(self):
        printer_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\printers\\printers.txt', 'r')
        printers = printer_file.readlines()
        total_printers = len(printers)
        p = 0
        for temp in printers:
            printer = temp.split(',')
            if p < total_printers:
                if int(printer[0]) == 1:
                    self.printer = p
                    self.selected_printer = p
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    self.Message_panel.append(">>> Loaded: " + str(printer[1].replace("\n", "")))
                    action_printer.setText(str("● " + printer[1].replace("\n", "")))
                else:
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    action_printer.setText(str("   " + printer[1].replace("\n", "")))
                if int(printer[2]) == 1:
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    action_printer.setVisible(True)
                else:
                    action_printer = getattr(self, "actionPrinter{}".format(p))
                    action_printer.setVisible(False)
            p = p + 1
        printer_file.close()
        self.load_settings(self)

    def load_settings(self):
        file = open(
            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.printer) + '\\boxes settings.txt',
            'r')  # read general setting file and set them
        boxes = file.readlines()
        file.close()
        file = open(
            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cboxes settings.txt',
            'r')  # read general setting file and set them
        cboxes = file.readlines()
        file.close()
        file = open(
            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.printer) + '\\dboxes settings.txt',
            'r')  # read general setting file and set them
        dboxes = file.readlines()
        file.close()
        file = open(
            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.printer) + '\\cbboxes settings.txt',
            'r')  # read general setting file and set them
        cbboxes = file.readlines()
        file.close()
        file = open(
            os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.printer) + '\\pins settings.txt',
            'r')  # read general setting file and set them
        pins = file.readlines()
        file.close()
        try:
            file = open(
                os.getenv('LOCALAPPDATA') + '\\3DHex2\\settings\\Printer' + str(self.printer) + '\\abl_type.txt',
                'r')  # read general setting file and set them
            abldata = file.readlines()
            self.ABL_INTERPOLATION_TYPE = int(abldata[0].strip())
            if self.ABL_INTERPOLATION_TYPE == 1:
                self.m1.setPlainText('PLANE')
            elif self.ABL_INTERPOLATION_TYPE == 2:
                self.m1.setPlainText('MESH')
            elif self.ABL_INTERPOLATION_TYPE == 0:
                self.m1.setPlainText('NO DATA')
        except:
            self.m1.setPlainText('NO DATA')
        for i in range(0, 71):  # b0-bmax
            b = getattr(self, "b{}".format(
                i))  # self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            b.setPlainText('')
            b.insertPlainText(boxes[i].strip())  # strip() removes'/n'
        for i in range(2, 6):  # c0-cmax
            c = getattr(self, "c{}".format(
                i))  # self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            c.setChecked(1)  # set Enable/Disable cboxes always on at start
        for i in range(7, 30):  # c0-cmax
            c = getattr(self, "c{}".format(
                i))  # self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            c.setChecked(int(cboxes[i - 1].strip()))  # strip() removes'/n'
        for i in range(1, 9):  # c0-cmax
            d = getattr(self, "d{}".format(
                i))  # self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            d.setValue(float(dboxes[i - 1].strip()))  # strip() removes'/n'
        for i in range(1, 4):  # c0-cmax
            cb = getattr(self, "comboBox{}".format(
                i))  # self.b[i], https://stackoverflow.com/questions/47666922/set-properties-of-multiple-qlineedit-using-a-loop
            cb.setCurrentIndex(int(cbboxes[i - 1].strip()))
        for i in range(0, 40):  # c0-cmax
            pin = getattr(self, "Pin_Button_{}".format(i))
            val = int(pins[i].strip())
            if val == 100:
                pin.setText('N')
            else:
                pin.setText(str(val))
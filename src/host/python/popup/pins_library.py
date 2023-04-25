from PyQt5 import QtWidgets

from ui.pinswindow_ui import Ui_pinswindow

class PinsWindow(QtWidgets.QMainWindow, Ui_pinswindow):

    def __init__(self, mainwindow, *args, obj=None, **kwargs):
        super(PinsWindow, self).__init__(*args, **kwargs)
        self.setupUi(self) #import Qtdesigner
        self.main_window = mainwindow
        getattr(self, "pin{}".format(self.main_window.pin_conf)).setStyleSheet("QPushButton {\n"
                "background-color:rgb(47, 47, 47);\n"
                "color: rgb(255, 255, 255);\n"
                "border-style: solid;\n"
                "border-radius:5px;\n"
                "border-width:2px; \n"
                "border-color:rgb(0,255,0)\n"
                "}\n"
                "QPushButton:hover {\n"
                "border-width:3px; \n"
                "border-color:rgb(255, 115, 30);\n"
                "}\n"
                "QPushButton:pressed {\n"
                "background-color:rgb(255, 115, 30);\n"
                "border-color: rgb(255, 195, 110);\n"
                "border-width: 4px;     \n"
                "}\n"
                "QPushButton:disabled{\n"
                "background-color: rgb(255, 149, 62);\n"
                "color: rgb(83, 83, 83);\n"
                "border-color:rgb(83, 83, 83);\n"
                "}\n"
                "\n"
                "")
        self.pin0.clicked.connect(lambda:self.select_pin(0))
        self.pin1.clicked.connect(lambda:self.select_pin(1))
        self.pin2.clicked.connect(lambda:self.select_pin(2))
        self.pin3.clicked.connect(lambda:self.select_pin(3))
        self.pin4.clicked.connect(lambda:self.select_pin(4))
        self.pin5.clicked.connect(lambda:self.select_pin(5))
        self.pin6.clicked.connect(lambda:self.select_pin(6))
        self.pin7.clicked.connect(lambda:self.select_pin(7))
        self.pin8.clicked.connect(lambda:self.select_pin(8))
        self.pin9.clicked.connect(lambda:self.select_pin(9))
        self.pin10.clicked.connect(lambda:self.select_pin(10))
        self.pin11.clicked.connect(lambda:self.select_pin(11))
        self.pin12.clicked.connect(lambda:self.select_pin(12))
        self.pin13.clicked.connect(lambda:self.select_pin(13))
        self.pin14.clicked.connect(lambda:self.select_pin(14))
        self.pin15.clicked.connect(lambda:self.select_pin(15))
        self.pin16.clicked.connect(lambda:self.select_pin(16))
        self.pin17.clicked.connect(lambda:self.select_pin(17))
        self.pin18.clicked.connect(lambda:self.select_pin(18))
        self.pin19.clicked.connect(lambda:self.select_pin(19))
        self.pin20.clicked.connect(lambda:self.select_pin(20))
        self.pin21.clicked.connect(lambda:self.select_pin(21))
        self.pin22.clicked.connect(lambda:self.select_pin(22))
        self.pin23.clicked.connect(lambda:self.select_pin(23))
        self.pin24.clicked.connect(lambda:self.select_pin(24))
        self.pin25.clicked.connect(lambda:self.select_pin(25))
        self.pin26.clicked.connect(lambda:self.select_pin(26))
        self.pin27.clicked.connect(lambda:self.select_pin(27))
        self.pin28.clicked.connect(lambda:self.select_pin(28))
        self.pin29.clicked.connect(lambda:self.select_pin(29))
        self.pin30.clicked.connect(lambda:self.select_pin(30))
        self.pin31.clicked.connect(lambda:self.select_pin(31))
        self.pin32.clicked.connect(lambda:self.select_pin(32))
        self.pin33.clicked.connect(lambda:self.select_pin(33))
        self.pin34.clicked.connect(lambda:self.select_pin(34))
        self.pin35.clicked.connect(lambda:self.select_pin(35))
        self.pin36.clicked.connect(lambda:self.select_pin(36))
        self.pin37.clicked.connect(lambda:self.select_pin(37))
        self.pin38.clicked.connect(lambda:self.select_pin(38))
        self.pin39.clicked.connect(lambda:self.select_pin(39))
        self.pin40.clicked.connect(lambda:self.select_pin(40))
        self.pin41.clicked.connect(lambda:self.select_pin(41))
        self.pin42.clicked.connect(lambda:self.select_pin(42))
        self.pin43.clicked.connect(lambda:self.select_pin(43))
        self.pin44.clicked.connect(lambda:self.select_pin(44))
        self.pin45.clicked.connect(lambda:self.select_pin(45))
        self.pin46.clicked.connect(lambda:self.select_pin(46))
        self.pin47.clicked.connect(lambda:self.select_pin(47))
        self.pin48.clicked.connect(lambda:self.select_pin(48))
        self.pin49.clicked.connect(lambda:self.select_pin(49))
        self.pin50.clicked.connect(lambda:self.select_pin(50))
        self.pin51.clicked.connect(lambda:self.select_pin(51))
        self.pin52.clicked.connect(lambda:self.select_pin(52))
        self.pin53.clicked.connect(lambda:self.select_pin(53))
        self.pin54.clicked.connect(lambda:self.select_pin(54))
        self.pin55.clicked.connect(lambda:self.select_pin(55))
        self.pin56.clicked.connect(lambda:self.select_pin(56))
        self.pin57.clicked.connect(lambda:self.select_pin(57))
        self.pin58.clicked.connect(lambda:self.select_pin(58))
        self.pin59.clicked.connect(lambda:self.select_pin(59))
        self.pin60.clicked.connect(lambda:self.select_pin(60))
        self.pin61.clicked.connect(lambda:self.select_pin(61))
        self.pin62.clicked.connect(lambda:self.select_pin(62))
        self.pin63.clicked.connect(lambda:self.select_pin(63))
        self.pin64.clicked.connect(lambda:self.select_pin(64))
        self.pin65.clicked.connect(lambda:self.select_pin(65))
        self.pin66.clicked.connect(lambda:self.select_pin(66))
        self.pin67.clicked.connect(lambda:self.select_pin(67))
        self.pin68.clicked.connect(lambda:self.select_pin(68))
        self.pin69.clicked.connect(lambda:self.select_pin(69))
        self.pin70.clicked.connect(lambda:self.select_pin(70))
    def select_pin(self,pin):
        if pin==70:
            getattr(self.main_window, "Pin_Button_{}".format(self.main_window.pin_button)).setText('N')
        else:
            getattr(self.main_window, "Pin_Button_{}".format(self.main_window.pin_button)).setText(str(pin))
        self.close()
def assign_buttons(self):
    self.p1.clicked.connect(self.usbconnect.CONNECT)
    self.p3.clicked.connect(self.processesmanager.USB)
    self.p5.clicked.connect(self.rapidmovecontrol.HOME_X_MIN)
    self.p9.clicked.connect(self.rapidmovecontrol.HOME_Y_MIN)
    self.p13.clicked.connect(self.rapidmovecontrol.HOME_Z_MIN)

    button_mapping = {
        self.p7: (0, 1),
        self.p8: (0, 0),
        self.p11: (1, 1),
        self.p12: (1, 0),
        self.p15: (2, 1),
        self.p16: (2, 0),
        self.p19: (3, 1),
        self.p20: (3, 0)
    }
    for button, args in button_mapping.items():
        button.clicked.connect(lambda _, args=args: self.rapidmovecontrol.rapid_idle_position(*args))

    self.p21.clicked.connect(self.processesmanager.PAUSE)
    self.p4.clicked.connect(self.processesmanager.SD_CARD)
    self.p88.clicked.connect(self.tempidlecontrol.setNOZZTEMP)
    self.p89.clicked.connect(self.tempidlecontrol.setBEDTEMP)
    self.p22.clicked.connect(self.widgethandler.clear_GCODE)
    self.p23.clicked.connect(self.processesmanager.CANCEL)
    self.p24.clicked.connect(self.processesmanager.setJFAJ)
    self.p25.clicked.connect(self.tempidlecontrol.setFAN1)
    self.p90.clicked.connect(self.processesmanager.nozz_AUTOTUNE)
    self.p91.clicked.connect(self.processesmanager.bed_AUTOTUNE)
    self.p29.clicked.connect(self.processesmanager.View)
    self.p30.clicked.connect(lambda: self.bltouch.BL_TOUCH_TOGGLE(0))
    self.p31.clicked.connect(lambda: self.bltouch.BL_TOUCH_TOGGLE(1))
    self.p27.clicked.connect(self.processesmanager.execute_ABL)
    self.action_Open.triggered.connect(self.filehandler.openfile)
    self.comboBox.currentTextChanged.connect(self.selectionhandler.selectPort)  # https://zetcode.com/pyqt/qcheckbox/
    self.c2.stateChanged.connect(self.rapidmovecontrol.setXmotor)
    self.c3.stateChanged.connect(self.rapidmovecontrol.setYmotor)
    self.c4.stateChanged.connect(self.rapidmovecontrol.setZmotor)
    self.c5.stateChanged.connect(self.rapidmovecontrol.setEmotor)
    self.c7.stateChanged.connect(self.selectionhandler.readInvertX)
    self.c8.stateChanged.connect(self.selectionhandler.readInvertY)
    self.c9.stateChanged.connect(self.selectionhandler.readInvertZ)
    self.c12.stateChanged.connect(self.widgethandler.setHOME_Xbuttons)
    self.c13.stateChanged.connect(self.widgethandler.setHOME_Ybuttons)
    self.c14.stateChanged.connect(self.widgethandler.setHOME_Zbuttons)
    self.c21.stateChanged.connect(self.widgethandler.UNITS)
    self.c29.stateChanged.connect(self.widgethandler.ABL_include)
    for printer in range(25):
        action = getattr(self, f"actionPrinter{printer}")
        action.triggered.connect(lambda _, printer=printer: self.printconfigurations.select_printer(printer))

    for pin in range(40):
        button = getattr(self, f"Pin_Button_{pin}")
        button.clicked.connect(lambda _, pin=pin: self.popuphandler.select_HW_pin(pin))

    self.actionNew.triggered.connect(self.printconfigurations.new_printer)
    self.actionRemove.triggered.connect(self.printconfigurations.remove_printer)
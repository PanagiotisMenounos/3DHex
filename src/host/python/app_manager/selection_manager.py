class EventConfig:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def selectPort(self,COM):
        self.main_window.chosenPort = COM

    def readInvertX(self):
        self.main_window.InvertX_tongle=1

    def readInvertY(self):
        self.main_window.InvertY_tongle=1

    def readInvertZ(self):
        self.main_window.InvertZ_tongle=1

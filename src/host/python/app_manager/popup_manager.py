#********************LOCAL IMPORTS**************************
from popup.pins_library import PinsWindow
from popup.autopid_update import AutoTuneWindow

class PopupManager:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window
    def select_HW_pin(self,button):
        self.main_window.pin_button=button
        try:
            self.main_window.pin_conf = int(getattr(self.main_window, "Pin_Button_{}".format(button)).text())
        except:
            self.main_window.pin_conf = 70
        self.main_window.pins_window = PinsWindow(self.main_window)
        self.main_window.pins_window.show()

    def test(self): #popup window for autotune results
        self.window3 = AutoTuneWindow(self)
        self.window3.show()
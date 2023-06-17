class BlTouch:

    def __init__(self, main_window):
        super().__init__()
        self.main_window = main_window

    def BL_TOUCH_TOGGLE(self, case):
        if self.main_window.home_axis == 0 and self.main_window.rapid_pos == 0 and self.main_window.A == 0:
            self.main_window.bl_toggle = 1
            self.main_window.A = 0
            self.main_window.B = 7
            if case == 0:  # toggle button
                if self.main_window.BL_TOUCH_STATE == 0:  # UP
                    self.main_window.C = 1
                    self.main_window.BL_TOUCH_STATE = 1
                else:
                    self.main_window.C = 2
                    self.main_window.BL_TOUCH_STATE = 0
            else:  # test button
                self.main_window.C = 0
        else:
            self.main_window.Message_panel.append(">>> Aborted")
import os
import struct
import win32pipe

def USBPrintingHandler(self):  # call this func whenever USB_calculate button is pressed
    if self.USB_CONNECTED == 1 and self.A == 0:
        self.pipe = win32pipe.CreateNamedPipe(
            r'\\.\pipe\Foo',
            win32pipe.PIPE_ACCESS_DUPLEX,
            win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
            1, 65536, 65536,
            0,
            None)
        self.pause_state = 0
        self.A = 1  # printing mode
        self.usb_printing = 1
        self.start_bar()
        if self.ABL == 0:
            gcode_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\support files\\GCODE.txt', 'w')
            gcode = self.GCODE_Panel.toPlainText()
            gcode_file.write(gcode)
            gcode_file.close()
            self.file1 = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\support files\\GCODE.txt', 'w')
            self.data = self.GCODE_Panel.toPlainText()
            self.file1.write(self.data)
            self.file1.close()
        self.save_settings()
        self.savepathfile = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\support files\\savepath.txt', 'w')
        self.savepathfile.close()
        self.child_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin', 'w')
        self.child_file.write(str(struct.pack("i", self.mirror)))
        self.child_file.close()
        self.disable_idle_buttons()
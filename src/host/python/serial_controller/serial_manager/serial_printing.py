import os
import subprocess
import struct

from serial_controller.serial_manager.buffer_sender import BufferSender

class SerialPrint:
    
    def __init__(self, usbworker, main_window):
        super().__init__()
        self.main_window = main_window
        self.serial_manager = usbworker
        self.serialsender = BufferSender(self.main_window)

    def usb_printing(self):  # USB Printing function
        self.main_window.usb_printing = 1
        self.serial_manager.trackZ = 0
        self.main_window.A = 1  # printing mode
        self.serial_manager.serial_command = 0  # reset serial command from arduino
        flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
        flag_file.write(struct.pack('2i', 5,
                                    5))  # Write some trash data in order for 3DHex.C to know Python is in printing function
        flag_file.close()
        flag_py_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag_py.bin', "wb")  # Clear flag file
        flag_py_file.close()
        buffer1_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin',
                            "wb")  # Reset buffer1 binary file
        buffer2_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin',
                            "wb")  # Reset buffer2 binary file
        buffer1_file.close()
        buffer2_file.close()
        self.serial_manager.message.emit(">>> GCODE Post processing..this may take a while..")  # emit the signal
        p1 = subprocess.Popen("3DBrain.exe")  # Start 3DHex.C Proccess
        flag_py_buffer = 0  # Reset flag_py_buffer
        filecase = 1  # Read from buffer1 file
        buffer_file_size = 3000  # Declare buffer file size (This is max arduino buffer array size until all RAM is full)
        self.serial_manager.child_buffer_size = 1  # Means 3DHex.C is still running
        while flag_py_buffer == 0 and self.main_window.usb_printing == 1:  # wait for C to fill binary data to buffer1+buffer2 binary files
            (self.serial_manager.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS,
             self.main_window.Y_POS,
             self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(18))  # Read arduino temp report
            self.serial_manager.new_nozz_temp.emit(self.main_window.nozz_temp)  # emit the signal
            self.serial_manager.new_bed_temp.emit(self.main_window.bed_temp)  # emit the signal
            self.serial_manager.x_pos_report.emit(self.main_window.X_POS)
            self.serial_manager.y_pos_report.emit(self.main_window.Y_POS)
            self.serial_manager.z_pos_report.emit(self.main_window.Z_POS)
            flag_py_buffer = os.path.getsize(os.getenv(
                'LOCALAPPDATA') + '\\3DHex2\\binary files\\flag_py.bin')  # Get size of flag_py_file, if 1 then C has fill binary files
        self.main_window.ser.flushInput()  # very important without delay to fix a bug
        self.main_window.ser.flushOutput()  # very important without delay to fix a bug
        self.serialsender.send_buffer()  # Command Printer to go into printig mode self.main_window.A=1
        self.serial_manager.message.emit(">>> Post processing successfully completed")  # emit the signal
        self.serial_manager.message.emit(">>> Printing...")  # emit the signal        
        if buffer_file_size == 3000 and self.serial_manager.child_buffer_size != 0 and self.serial_manager.serial_command != -260:  # Firt time send binary data to Printer
            if filecase == 1:  # Read from buffer1 binary file
                filecase = 2  # Note to read buffer2 next time
                buffer1_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin', "rb")
                self.serial_manager.decoder.packet_decode()
                self.main_window.ser.write(buffer1_file.read(3000))  # Send binary data to Printer
                buffer1_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5,
                                            5))  # Write some trash data to tell C that buffer1 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            if filecase == 2:
                filecase = 1
                buffer2_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin', "rb")
                self.serial_manager.decoder.packet_decode()
                self.main_window.ser.write(buffer2_file.read(3000))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5,
                                            5))  # Write some trash data to tell C that buffer2 file is free to fill with new data
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            self.serial_manager.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin')
        while self.serial_manager.serial_command != -10 and self.serial_manager.child_buffer_size != 0 and self.main_window.usb_printing == 1:  # Wait for printer to read commanded temp
            (self.serial_manager.serial_command, self.main_window.nozz_temp, self.main_window.bed_temp, self.main_window.X_POS,
             self.main_window.Y_POS,
             self.main_window.Z_POS,) = struct.unpack("3f3H", self.main_window.ser.read(18))  # Read arduino temp report
            self.serial_manager.new_nozz_temp.emit(self.main_window.nozz_temp)  # emit the signal
            self.serial_manager.new_bed_temp.emit(self.main_window.bed_temp)  # emit the signal
            self.serial_manager.x_pos_report.emit(self.main_window.X_POS)
            self.serial_manager.y_pos_report.emit(self.main_window.Y_POS)
            self.serial_manager.z_pos_report.emit(self.main_window.Z_POS)
        while buffer_file_size == 3000 and self.serial_manager.child_buffer_size != 0 and self.serial_manager.serial_command != -260 and self.main_window.usb_printing == 1:  # Start binary data streaming to Printer 
            if filecase == 1:
                filecase = 2
                buffer1_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin', "rb")
                self.serial_manager.decoder.packet_decode()
                self.main_window.ser.write(buffer1_file.read(3000))
                buffer1_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_1.bin')
                flag_file.close()
            elif filecase == 2:
                filecase = 1
                buffer2_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin', "rb")
                self.serial_manager.decoder.packet_decode()
                self.main_window.ser.write(buffer2_file.read(3000))
                buffer2_file.close()
                flag_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\flag.bin', "wb")
                flag_file.write(struct.pack('2i', 5, 5))
                buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\buffer_2.bin')
                flag_file.close()
            self.serial_manager.child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin')
        self.serial_manager.child_buffer_size = 1  # catch last packet after C terminates, if MCU does ot send -260 this will stuck in loop
        self.serial_manager.decoder.packet_decode()
        child_file = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\binary files\\child.bin',
                          'w')  # reset child so 3DHex.C to terminate
        child_file.close()
        savepathfile = open(os.getenv('LOCALAPPDATA') + '\\3DHex2\\support files\\savepath.txt', 'w')  # reset path
        savepathfile.close()
        buffer1_file.close()
        buffer2_file.close()
        self.main_window.usb_printing = 0
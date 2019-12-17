#----------------------------------------------------------------------
#      COPYRIGHT NOTICE FOR 3DHex:
#----------------------------------------------------------------------
#
#      3DHex - 3D Printer Firmware
#
#Copyright (c) 2019 Panagiotis Menounos
#Contact: 3DHexfw@gmail.com
#
#
#3DHex is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#3DHex is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with 3DHex.  If not, see <http://www.gnu.org/licenses/>.




from Tkinter import *  
from PIL import ImageTk, Image #http://www.pythonware.com/products/pil/
import Tkinter as tk #very important to create new window
from tkFileDialog import * #it is used for save path askdirectory()
from multiprocessing import Process #for multiprocessing 
import subprocess
import multiprocessing
import ttk #very important to use state method at check boxes
import easygui #this is needed for file->open to work
import os
import serial #pyserial
import time
#import numpy as np
import struct
import sys

def USB_SERIAL_SENDER(COM_PORT,BAUD_RATE):
  flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\flag.bin',"wb")
  flag_file.write(struct.pack('2i', 5, 5))
  flag_file.close()
  flag_py_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\flag_py.bin',"wb")
  flag_py_file.close()
  p2 = subprocess.Popen("Brain.exe")
  flag_py_buffer=0
  i=0;
  filecase=1
  buffer_file_size=3300
  child_buffer_size=1
  while flag_py_buffer==0:
    flag_py_buffer=os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex\\flag_py.bin')
  ser=serial.Serial(COM_PORT,BAUD_RATE) #first time communicate
  ser.flushOutput()
  ser.flushInput()
  time.sleep(2)
  ser.write('c')
  serial_input='c'
  while buffer_file_size==3300 and child_buffer_size!=0 and serial_input=='c':
    if filecase==1:
        filecase=2
        buffer1_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex\\buffer_1.bin', "rb")
        serial_input=ser.read(1)
        ser.write(buffer1_file.read(3300))
        buffer1_file.close()
        flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\flag.bin',"wb")
        flag_file.write(struct.pack('2i', 5, 5))
        buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex\\buffer_1.bin')
        flag_file.close()
    elif filecase==2:
        filecase=1
        buffer2_file=open(os.getenv('LOCALAPPDATA')+'\\3DHex\\buffer_2.bin', "rb")
        serial_input=ser.read(1)
        ser.write(buffer2_file.read(3300))
        buffer2_file.close()
        flag_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\flag.bin',"wb")
        flag_file.write(struct.pack('2i', 5, 5))
        buffer_file_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex\\buffer_2.bin')
        flag_file.close()
    child_buffer_size = os.path.getsize(os.getenv('LOCALAPPDATA')+'\\3DHex\\child.bin')
  child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\child.bin','w')
  child_file.close()
 # savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\savepath.txt','w')
  #savepathfile.close()
  ser.close()
  buffer1_file.close()
  buffer2_file.close()


def openfile():#call this function whenever file->open is pressed
   path = easygui.fileopenbox() #open window to select file and save path
   if path != None:
     file = open(path) #open this file
     data = file.read() #read the file
     file.close() #close the file
     text.insert(END, data) #write what you have read to text box

def about(): 
   window2 = tk.Toplevel(root)
   window2.title("About")#title of the window1
   window2.geometry('{}x{}'.format(550, 600))#size of the window1 at start up
   window2.resizable(width=False, height=False) #fixed window1 size
   window2.wm_iconbitmap('3DHex.ico')
   photos_frame = Frame(window2,bd=0, relief=GROOVE) #create a frame for labels and boxes for settings
   photos_frame.place(x=160,y=0) #the coordinates for placing the frame on root window1
   lisence_frame = Frame(window2,bd=1, relief=GROOVE) #create a frame for labels and boxes for settings
   lisence_frame.place(x=25,y=150) #the coordinates for placing the frame on root window1
   xscrollbar = Scrollbar(lisence_frame, orient=HORIZONTAL)#create horizontal scrollbar
   xscrollbar.grid(row=1, column=0, sticky=E+W) #place it at the (bottom) 
   yscrollbar = Scrollbar(lisence_frame) #create vertical scrollbar
   yscrollbar.grid(row=0, column=1, sticky=N+S) #place it (right)
   lisence_text = Text(lisence_frame, wrap=NONE, bd=0, #create a text box and place the above scrollbars
            xscrollcommand=xscrollbar.set,
            yscrollcommand=yscrollbar.set,width = 60, height = 21,font=("Calibri", 12))
   lisence_text.grid(row=0, column=0, sticky=N+S+E+W) #choose to place text box with grid(there are also place and pack methods)
   xscrollbar.config(command=lisence_text.xview) #this is for scrollbars to move with cursor
   yscrollbar.config(command=lisence_text.yview)
   #load1=Image.open("3DHex.png")
   load2=Image.open("GPLv3.png")
   #load1 = load1.resize((120,120), Image.ANTIALIAS)
   load2 = load2.resize((138,68), Image.ANTIALIAS)
   #render1 = ImageTk.PhotoImage(load1)
   render2 = ImageTk.PhotoImage(load2)
   #img1=Label(photos_frame,image=render1,width=120,height=120)
   #img1.image=render1
   img2=Label(photos_frame,image=render2,width=210,height=105)
   img2.image=render2
   #img1.grid(row=0, column=0)
   img2.pack()
   lisence_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\LICENSE') #open this file
   lisence_data = lisence_file.read() #read the file
   lisence_file.close() #close the file
   lisence_text.insert(END, lisence_data)
   l300=Label(window2,text="Installed Version: 1.0.1                                 \nCopyright (c) 2019 Panagiotis Menounos  \nContact: 3DHexfw@gmail.com                    ",\
              width=35,height=3,font=("Calibri", 10,'bold'),relief=FLAT) #create label2
   l300.place(x=15, y=98) #choose to place labels with grid(there are also place and pack methods)
 
def guide():
   subprocess.Popen([os.getenv('LOCALAPPDATA')+"\\3DHex\\Guide_1.0.1.pdf"],shell=True)
 
def exit():
 child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\child.bin','w')
 child_file.close()
 savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\savepath.txt','w')
 savepathfile.close()
 root.destroy()
  
def on_closing():
 child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\child.bin','w')
 child_file.close()
 savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\savepath.txt','w')
 savepathfile.close()
 root.destroy()

def USB_calculate(): #call this func whenever USB_calculate button is pressed
  mirror=5
  savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\savepath.txt','w')
  savepathfile.close()
  child_file = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\child.bin','w')
  child_file.write(struct.pack("i",mirror))
  child_file.close()
  p = Process(target=USB_SERIAL_SENDER,args=('COM'+str(t33.get(1.0,END)[:-1]).split()[0],str(t34.get(1.0,END)[:-1]).split()[0],))
  p.start()

def SD_calculate():
 path=askdirectory()
 savepathfile = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\savepath.txt','w')
 savepathfile.write(path.encode('utf8')) #https://stackoverflow.com/questions/6048085/writing-unicode-text-to-a-text-file
 savepathfile.close()
 if(path != ''):
    p2 = subprocess.Popen("Brain.exe")
 
def save_general_settings():
   script=StringVar() #this variable contains all the string
   script=text.get(1.0,END)[:-1] #read all contend from the text box (GCODE)
   saveFile1 = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\GCODE.txt','w') #open a txt file
   saveFile1.write(str(script)) #and write what you have read from text box
   saveFile1.close() #close the file
   open(os.getenv('LOCALAPPDATA')+'\\3DHex\\general settings.txt', 'w').close()
   saveFile2 = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\general settings.txt','w')
   if (not str(t1.get(1.0,END)[:-1])) or (str(t1.get(1.0,END)[:-1])=='') or (str(t1.get(1.0,END)[:-1])=='\n'): #check if it is an empty string
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t1.get(1.0,END)[:-1]).split()[0]+'\n') #split delete '\n' char
   if (not str(t2.get(1.0,END)[:-1])) or (str(t2.get(1.0,END)[:-1])=='') or (str(t2.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t2.get(1.0,END)[:-1]).split()[0]+'\n')
   if (not str(t3.get(1.0,END)[:-1])) or (str(t3.get(1.0,END)[:-1])=='') or (str(t3.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t3.get(1.0,END)[:-1]).split()[0]+'\n')  
   if (not str(t4.get(1.0,END)[:-1])) or (str(t4.get(1.0,END)[:-1])=='') or (str(t4.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t4.get(1.0,END)[:-1]).split()[0]+'\n')  
   if (not str(t5.get(1.0,END)[:-1])) or (str(t5.get(1.0,END)[:-1])=='') or (str(t5.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t5.get(1.0,END)[:-1]).split()[0]+'\n')   
   if (not str(t6.get(1.0,END)[:-1])) or (str(t6.get(1.0,END)[:-1])=='') or (str(t6.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t6.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t7.get(1.0,END)[:-1]) or (str(t7.get(1.0,END)[:-1])=='') or (str(t7.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t7.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t8.get(1.0,END)[:-1]) or (str(t8.get(1.0,END)[:-1])=='') or (str(t8.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t8.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t9.get(1.0,END)[:-1]) or (str(t9.get(1.0,END)[:-1])=='') or (str(t9.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t9.get(1.0,END)[:-1]).split()[0]+'\n')  
   if not str(t10.get(1.0,END)[:-1]) or (str(t10.get(1.0,END)[:-1])=='') or (str(t10.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t10.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t11.get(1.0,END)[:-1]) or (str(t11.get(1.0,END)[:-1])=='') or (str(t11.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t11.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t12.get(1.0,END)[:-1]) or (str(t12.get(1.0,END)[:-1])=='') or (str(t12.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t12.get(1.0,END)[:-1]).split()[0]+'\n')
   if not str(t13.get(1.0,END)[:-1]) or (str(t13.get(1.0,END)[:-1])=='') or (str(t13.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t13.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t14.get(1.0,END)[:-1]) or (str(t14.get(1.0,END)[:-1])=='') or (str(t14.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t14.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t15.get(1.0,END)[:-1]) or (str(t15.get(1.0,END)[:-1])=='') or (str(t15.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n") 
   else:
    saveFile2.write(str(t15.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t16.get(1.0,END)[:-1]) or (str(t16.get(1.0,END)[:-1])=='') or (str(t16.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n") 
   else:
    saveFile2.write(str(t16.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t17.get(1.0,END)[:-1]) or (str(t17.get(1.0,END)[:-1])=='') or (str(t17.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n") 
   else:
    saveFile2.write(str(t17.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t18.get(1.0,END)[:-1]) or (str(t18.get(1.0,END)[:-1])=='') or (str(t18.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")   
   else:
    saveFile2.write(str(t18.get(1.0,END)[:-1]).split()[0]+'\n')  
   if not str(t19.get(1.0,END)[:-1]) or (str(t19.get(1.0,END)[:-1])=='') or (str(t19.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n") 
   else:
    saveFile2.write(str(t19.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t20.get(1.0,END)[:-1]) or (str(t20.get(1.0,END)[:-1])=='') or (str(t20.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")  
   else:
    saveFile2.write(str(t20.get(1.0,END)[:-1]).split()[0]+'\n')  
   if not str(t21.get(1.0,END)[:-1]) or (str(t21.get(1.0,END)[:-1])=='') or (str(t21.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")   
   else:
    saveFile2.write(str(t21.get(1.0,END)[:-1]).split()[0]+'\n')		
   if not str(t22.get(1.0,END)[:-1]) or (str(t22.get(1.0,END)[:-1])=='') or (str(t22.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")   
   else:
    saveFile2.write(str(t22.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t23.get(1.0,END)[:-1]) or (str(t23.get(1.0,END)[:-1])=='') or (str(t23.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t23.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t24.get(1.0,END)[:-1]) or (str(t24.get(1.0,END)[:-1])=='') or (str(t24.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n") 
   else:
    saveFile2.write(str(t24.get(1.0,END)[:-1]).split()[0]+'\n')   
   if not str(t25.get(1.0,END)[:-1]) or (str(t25.get(1.0,END)[:-1])=='') or (str(t25.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")   
   else:
    saveFile2.write(str(t25.get(1.0,END)[:-1]).split()[0]+'\n')      
   if not str(t26.get(1.0,END)[:-1]) or (str(t26.get(1.0,END)[:-1])=='') or (str(t26.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t26.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t27.get(1.0,END)[:-1]) or (str(t27.get(1.0,END)[:-1])=='') or (str(t27.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t27.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t28.get(1.0,END)[:-1]) or (str(t28.get(1.0,END)[:-1])=='') or (str(t28.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t28.get(1.0,END)[:-1]).split()[0]+'\n')     
   if not str(t29.get(1.0,END)[:-1]) or (str(t29.get(1.0,END)[:-1])=='') or (str(t29.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t29.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t30.get(1.0,END)[:-1]) or (str(t30.get(1.0,END)[:-1])=='') or (str(t30.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t30.get(1.0,END)[:-1]).split()[0]+'\n')

   if not str(t31.get(1.0,END)[:-1]) or (str(t31.get(1.0,END)[:-1])=='') or (str(t31.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t31.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t32.get(1.0,END)[:-1]) or (str(t32.get(1.0,END)[:-1])=='') or (str(t32.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t32.get(1.0,END)[:-1]).split()[0]+'\n') 
   if not str(t33.get(1.0,END)[:-1]) or (str(t33.get(1.0,END)[:-1])=='') or (str(t33.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t33.get(1.0,END)[:-1]).split()[0]+'\n')     
   if not str(t34.get(1.0,END)[:-1]) or (str(t34.get(1.0,END)[:-1])=='') or (str(t34.get(1.0,END)[:-1])=='\n'):
    saveFile2.write(" \n")
   else:
    saveFile2.write(str(t34.get(1.0,END)[:-1]).split()[0]+'\n') 	
   saveFile2.write(str(var1.get())+'\n')
   saveFile2.write(str(var2.get())+'\n')
   saveFile2.write(str(var3.get())+'\n')
   saveFile2.write(str(var4.get())+'\n')
   saveFile2.write(str(var5.get())+'\n')
   saveFile2.write(str(var15.get())+'\n')   
   saveFile2.write(str(var101.get())+'\n')
   saveFile2.write(str(var102.get())+'\n')
   saveFile2.write(str(var103.get())+'\n')
   saveFile2.write(str(var104.get())+'\n')
   saveFile2.write(str(var105.get())+'\n')
   saveFile2.close()

def angle_settings():
   window4 = tk.Toplevel(root)
   window4.title("Angle_Error")#title of the window1
   window4.geometry('{}x{}'.format(300, 130))#size of the window1 at start up
   window4.resizable(width=False, height=False) #fixed window1 size
   window4.wm_iconbitmap('3DHex.ico')
   angle_frame = Frame(window4,bd=2, relief=GROOVE) #create a frame for labels and boxes for settings
   angle_frame.place(x=15,y=10) #the coordinates for placing the frame on root window1
   l48=Label(angle_frame,text="XY_plane",width=10,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l48.grid(row=0, column=0) #choose to place labels with grid(there are also place and pack methods)
   l49=Label(angle_frame,text="ZX_plane",width=10,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l49.grid(row=1, column=0) #choose to place labels with grid(there are also place and pack methods)
   l50=Label(angle_frame,text="ZY_plane",width=10,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l50.grid(row=2, column=0) #choose to place labels with grid(there are also place and pack methods)
   t77=Text(angle_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t77.grid(row=0, column=1) #choose to place text box with grid(there are also place and pack methods)
   t78=Text(angle_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t78.grid(row=1, column=1) #choose to place text box with grid(there are also place and pack methods)
   t79=Text(angle_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t79.grid(row=2, column=1) #choose to place text box with grid(there are also place and pack methods)
   with open(os.getenv('LOCALAPPDATA')+'\\3DHex\\angle settings.txt') as file: #read general setting file and set them
      array3 = file.readlines()
      if array3[0]!=' \n': t77.insert(END, array3[0]) #if fixes a bug
      if array3[1]!=' \n': t78.insert(END, array3[1])
      if array3[2]!=' \n': t79.insert(END, array3[2])
   def save_angle_settings():
     open(os.getenv('LOCALAPPDATA')+'\\3DHex\\angle settings.txt', 'w').close()
     saveFile5 = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\angle settings.txt','w')
     if (not str(t77.get(1.0,END)[:-1])) or (str(t77.get(1.0,END)[:-1])=='') or (str(t77.get(1.0,END)[:-1])=='\n'): #check if it is aan empty string
      saveFile5.write("0\n")
     else:
      saveFile5.write(str(t77.get(1.0,END)[:-1]).split()[0]+'\n') #split delete '\n' char
     if (not str(t78.get(1.0,END)[:-1])) or (str(t78.get(1.0,END)[:-1])=='') or (str(t78.get(1.0,END)[:-1])=='\n'):
      saveFile5.write("0\n")
     else:
      saveFile5.write(str(t78.get(1.0,END)[:-1]).split()[0]+'\n')
     if (not str(t79.get(1.0,END)[:-1])) or (str(t79.get(1.0,END)[:-1])=='') or (str(t79.get(1.0,END)[:-1])=='\n'):
      saveFile5.write("0\n")
     else:
      saveFile5.write(str(t79.get(1.0,END)[:-1]).split()[0]+'\n')
   b102=Button(window4, text="SAVE",font=("Calibri", 12,'bold'),width = 7, height = 1,command = save_angle_settings)
   b102.place(relx=0.640,rely=0.18)
   b103=Button(window4, text="CLOSE",font=("Calibri", 12,'bold'),width = 7, height = 1,command=window4.destroy)
   b103.place(relx=0.640,rely=0.58)	 
   
def homing_settings():
   window3 = tk.Toplevel(root)
   window3.title("Homing")#title of the window1
   window3.geometry('{}x{}'.format(550, 200))#size of the window1 at start up
   window3.resizable(width=False, height=False) #fixed window1 size
   window3.wm_iconbitmap('3DHex.ico')
   homing_frame = Frame(window3,bd=2, relief=GROOVE) #create a frame for labels and boxes for settings
   homing_frame.place(x=5,y=10) #the coordinates for placing the frame on root window1
   l41=Label(homing_frame,text="Enable_Min_Endstop",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l41.grid(row=1, column=0) #choose to place labels with grid(there are also place and pack methods)
   l45=Label(homing_frame,text="Home_Pin_State",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l45.grid(row=2, column=0) #choose to place labels with grid(there are also place and pack methods)
   l46=Label(homing_frame,text="Direction",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l46.grid(row=3, column=0) #choose to place labels with grid(there are also place and pack methods)
   l47=Label(homing_frame,text="Feedrate [mm]/[sec]",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label1
   l47.grid(row=4, column=0) #choose to place labels with grid(there are also place and pack methods)
   l42=Label(homing_frame,text="X_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
   l42.grid(row=0, column=1) #choose to place labels with grid(there are also place and pack methods)
   l43=Label(homing_frame,text="Y_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
   l43.grid(row=0, column=2) #choose to place labels with grid(there are also place and pack methods)
   l44=Label(homing_frame,text="Z_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
   l44.grid(row=0, column=3) #choose to place labels with grid(there are also place and pack methods)
   
   t74=Text(homing_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t74.grid(row=4, column=1) #choose to place text box with grid(there are also place and pack methods)
   t75=Text(homing_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t75.grid(row=4, column=2) #choose to place text box with grid(there are also place and pack methods)
   t76=Text(homing_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t76.grid(row=4, column=3) #choose to place text box with grid(there are also place and pack methods)
   var201 = IntVar()  
   var202 = IntVar() 
   var203 = IntVar() 
   var204 = IntVar()
   var205 = IntVar()
   var206 = IntVar()
   var207 = IntVar()
   var208 = IntVar()
   var209 =IntVar()
   with open(os.getenv('LOCALAPPDATA')+'\\3DHex\\homing settings.txt') as file: #read general setting file and set them
      array1 = file.readlines()
      if array1[0]!=' \n': t74.insert(END, array1[0]) #if fixes a bug
      if array1[1]!=' \n': t75.insert(END, array1[1])
      if array1[2]!=' \n': t76.insert(END, array1[2])    
      var201.set(int(array1[3]))
      var202.set(int(array1[4]))
      var203.set(int(array1[5]))
      var204.set(int(array1[6]))
      var205.set(int(array1[7]))
      var206.set(int(array1[8]))
      var207.set(int(array1[9]))
      var208.set(int(array1[10]))
      var209.set(int(array1[11]))
   c201=Checkbutton(homing_frame, variable=var201,onvalue=1, offvalue=0)
   c201.grid(row=1, column=1) 
   c202=Checkbutton(homing_frame, variable=var202,onvalue=1, offvalue=0)
   c202.grid(row=1, column=2)
   c203=Checkbutton(homing_frame, variable=var203,onvalue=1, offvalue=0)
   c203.grid(row=1, column=3)
   c204=Checkbutton(homing_frame, variable=var204,onvalue=1, offvalue=0)
   c204.grid(row=2, column=1)
   c205=Checkbutton(homing_frame, variable=var205,onvalue=1, offvalue=0)
   c205.grid(row=2, column=2) 
   c206=Checkbutton(homing_frame, variable=var206,onvalue=1, offvalue=0)
   c206.grid(row=2, column=3)  
   c207=Checkbutton(homing_frame, variable=var207,onvalue=1, offvalue=0)
   c207.grid(row=3, column=1)
   c208=Checkbutton(homing_frame, variable=var208,onvalue=1, offvalue=0)
   c208.grid(row=3, column=2) 
   c209=Checkbutton(homing_frame, variable=var209,onvalue=1, offvalue=0)
   c209.grid(row=3, column=3) 
   def save_Homing_settings():
     open(os.getenv('LOCALAPPDATA')+'\\3DHex\\homing settings.txt', 'w').close()
     saveFile4 = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\homing settings.txt','w')
     if (not str(t74.get(1.0,END)[:-1])) or (str(t74.get(1.0,END)[:-1])=='') or (str(t74.get(1.0,END)[:-1])=='\n'): #check if it is aan empty string
      saveFile4.write(" \n")
     else:
      saveFile4.write(str(t74.get(1.0,END)[:-1]).split()[0]+'\n') #split delete '\n' char
     if (not str(t75.get(1.0,END)[:-1])) or (str(t75.get(1.0,END)[:-1])=='') or (str(t75.get(1.0,END)[:-1])=='\n'):
      saveFile4.write(" \n")
     else:
      saveFile4.write(str(t75.get(1.0,END)[:-1]).split()[0]+'\n')
     if (not str(t76.get(1.0,END)[:-1])) or (str(t76.get(1.0,END)[:-1])=='') or (str(t76.get(1.0,END)[:-1])=='\n'):
      saveFile4.write(" \n")
     else:
      saveFile4.write(str(t76.get(1.0,END)[:-1]).split()[0]+'\n')
     saveFile4.write(str(var201.get())+'\n')
     saveFile4.write(str(var202.get())+'\n')
     saveFile4.write(str(var203.get())+'\n')
     saveFile4.write(str(var204.get())+'\n')
     saveFile4.write(str(var205.get())+'\n')
     saveFile4.write(str(var206.get())+'\n')
     saveFile4.write(str(var207.get())+'\n')
     saveFile4.write(str(var208.get())+'\n')
     saveFile4.write(str(var209.get())+'\n')
     saveFile4.close() 
   b100=Button(window3, text="SAVE",font=("Calibri", 12,'bold'),width = 7, height = 1,command = save_Homing_settings)
   b100.place(relx=0.840,rely=0.20)
   b101=Button(window3, text="CLOSE",font=("Calibri", 12,'bold'),width = 7, height = 1,command=window3.destroy)
   b101.place(relx=0.840,rely=0.60)	 
   


 
def temp_settings():

   def save_temperature_settings():
     open(os.getenv('LOCALAPPDATA')+'\\3DHex\\temp settings.txt', 'w').close()
     saveFile3 = open(os.getenv('LOCALAPPDATA')+'\\3DHex\\temp settings.txt','w')
     if (not str(t60.get(1.0,END)[:-1])) or (str(t60.get(1.0,END)[:-1])=='') or (str(t60.get(1.0,END)[:-1])=='\n'): #check if it is aan empty string
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t60.get(1.0,END)[:-1]).split()[0]+'\n') #split delete '\n' char
     if (not str(t61.get(1.0,END)[:-1])) or (str(t61.get(1.0,END)[:-1])=='') or (str(t61.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t61.get(1.0,END)[:-1]).split()[0]+'\n')
     if (not str(t62.get(1.0,END)[:-1])) or (str(t62.get(1.0,END)[:-1])=='') or (str(t62.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t62.get(1.0,END)[:-1]).split()[0]+'\n')  
     if (not str(t63.get(1.0,END)[:-1])) or (str(t63.get(1.0,END)[:-1])=='') or (str(t63.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t63.get(1.0,END)[:-1]).split()[0]+'\n')  
     if (not str(t64.get(1.0,END)[:-1])) or (str(t64.get(1.0,END)[:-1])=='') or (str(t64.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t64.get(1.0,END)[:-1]).split()[0]+'\n')   
     if (not str(t65.get(1.0,END)[:-1])) or (str(t65.get(1.0,END)[:-1])=='') or (str(t65.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t65.get(1.0,END)[:-1]).split()[0]+'\n')   
     if not str(t66.get(1.0,END)[:-1]) or (str(t66.get(1.0,END)[:-1])=='') or (str(t66.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t66.get(1.0,END)[:-1]).split()[0]+'\n')   
     if not str(t67.get(1.0,END)[:-1]) or (str(t67.get(1.0,END)[:-1])=='') or (str(t67.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t67.get(1.0,END)[:-1]).split()[0]+'\n') 
     if not str(t68.get(1.0,END)[:-1]) or (str(t68.get(1.0,END)[:-1])=='') or (str(t68.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t68.get(1.0,END)[:-1]).split()[0]+'\n')  
     if not str(t69.get(1.0,END)[:-1]) or (str(t69.get(1.0,END)[:-1])=='') or (str(t69.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t69.get(1.0,END)[:-1]).split()[0]+'\n')   
     if not str(t70.get(1.0,END)[:-1]) or (str(t70.get(1.0,END)[:-1])=='') or (str(t70.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t70.get(1.0,END)[:-1]).split()[0]+'\n')   
     if not str(t71.get(1.0,END)[:-1]) or (str(t71.get(1.0,END)[:-1])=='') or (str(t71.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t71.get(1.0,END)[:-1]).split()[0]+'\n')
     if not str(t72.get(1.0,END)[:-1]) or (str(t72.get(1.0,END)[:-1])=='') or (str(t72.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t72.get(1.0,END)[:-1]).split()[0]+'\n')  
     if not str(t73.get(1.0,END)[:-1]) or (str(t73.get(1.0,END)[:-1])=='') or (str(t73.get(1.0,END)[:-1])=='\n'):
      saveFile3.write(" \n")
     else:
      saveFile3.write(str(t73.get(1.0,END)[:-1]).split()[0]+'\n')
     saveFile3.write(str(var7.get())+'\n')
     saveFile3.write(str(var8.get())+'\n')
     saveFile3.write(str(var9.get())+'\n')
     saveFile3.write(str(var10.get())+'\n')
     saveFile3.write(str(var11.get())+'\n')
     saveFile3.write(str(var12.get())+'\n')
     saveFile3.close() 	 

   def click_PID_Nozzle():
    if var10.get()==0:
     l24.config(state=DISABLED)
     l25.config(state=DISABLED)
     l26.config(state=DISABLED)   
     l27.config(state=NORMAL)
     l28.config(state=NORMAL)
     l29.config(state=NORMAL)
     l30.config(state=NORMAL)
     t61.config(state=DISABLED,bd=0)
     t62.config(state=DISABLED,bd=0)
     t63.config(state=DISABLED,bd=0)
     t64.config(state=NORMAL,bd=1.8)
     t65.config(state=NORMAL,bd=1.8)
     t66.config(state=NORMAL,bd=1.8)
    else:
     l24.config(state=NORMAL)
     l25.config(state=NORMAL)
     l26.config(state=NORMAL)  
     l27.config(state=DISABLED)
     l28.config(state=DISABLED)
     l29.config(state=DISABLED)
     l30.config(state=DISABLED)
     t61.config(state=NORMAL,bd=1.8)
     t62.config(state=NORMAL,bd=1.8)
     t63.config(state=NORMAL,bd=1.8)
     t64.config(state=DISABLED,bd=0)
     t65.config(state=DISABLED,bd=0)
     t66.config(state=DISABLED,bd=0)	 

   def click_PID_Bed():
    if var12.get()==0:
     l34.config(state=DISABLED)
     l35.config(state=DISABLED)
     l36.config(state=DISABLED)   
     l37.config(state=NORMAL)
     l38.config(state=NORMAL)
     l39.config(state=NORMAL)
     l40.config(state=NORMAL)
     t68.config(state=DISABLED,bd=0)
     t69.config(state=DISABLED,bd=0)
     t70.config(state=DISABLED,bd=0)
     t71.config(state=NORMAL,bd=1.8)
     t72.config(state=NORMAL,bd=1.8)
     t73.config(state=NORMAL,bd=1.8)
	 
    else:
     l34.config(state=NORMAL)
     l35.config(state=NORMAL)
     l36.config(state=NORMAL)  
     l37.config(state=DISABLED)
     l38.config(state=DISABLED)
     l39.config(state=DISABLED)
     l40.config(state=DISABLED)
     t68.config(state=NORMAL,bd=1.8)
     t69.config(state=NORMAL,bd=1.8)
     t70.config(state=NORMAL,bd=1.8)
     t71.config(state=DISABLED,bd=0)
     t72.config(state=DISABLED,bd=0)
     t73.config(state=DISABLED,bd=0)
	 
   def click_Heated_Nozzle():
    if var8.get()==0:
     l19.config(state=DISABLED)
     l20.config(state=DISABLED)
     l23.config(state=DISABLED)
     l24.config(state=DISABLED)
     l25.config(state=DISABLED)
     l26.config(state=DISABLED)
     l27.config(state=DISABLED)
     l28.config(state=DISABLED)
     l29.config(state=DISABLED)
     l30.config(state=DISABLED)
     t60.config(state=DISABLED,bd=0)
     t61.config(state=DISABLED,bd=0)
     t62.config(state=DISABLED,bd=0)
     t63.config(state=DISABLED,bd=0)
     t64.config(state=DISABLED,bd=0)
     t65.config(state=DISABLED,bd=0)
     t65.config(state=DISABLED,bd=0)
     t66.config(state=DISABLED,bd=0)
     c10.config(state=DISABLED)
     c7.config(state=DISABLED)
     c9.config(state=NORMAL)	
    else:
     c9.config(state=NORMAL)
     l19.config(state=NORMAL)
     l20.config(state=NORMAL)
     l23.config(state=NORMAL)
     t60.config(state=NORMAL,bd=1.8)
     c10.config(state=NORMAL)
     c7.config(state=NORMAL)
     click_PID_Nozzle()
	 
   def click_Heated_Bed():
    if var9.get()==0:
     l31.config(state=DISABLED)
     l32.config(state=DISABLED)
     l33.config(state=DISABLED)
     l34.config(state=DISABLED)
     l35.config(state=DISABLED)
     l36.config(state=DISABLED)
     l37.config(state=DISABLED)
     l38.config(state=DISABLED)
     l39.config(state=DISABLED)
     l40.config(state=DISABLED)
     t67.config(state=DISABLED,bd=0)
     t68.config(state=DISABLED,bd=0)
     t69.config(state=DISABLED,bd=0)
     t70.config(state=DISABLED,bd=0)
     t71.config(state=DISABLED,bd=0)
     t72.config(state=DISABLED,bd=0)
     t73.config(state=DISABLED,bd=0)
     c12.config(state=DISABLED)
     c11.config(state=DISABLED) 
    else:
     l31.config(state=NORMAL)
     l32.config(state=NORMAL)
     l33.config(state=NORMAL)
     l34.config(state=NORMAL)
     l35.config(state=NORMAL)
     l36.config(state=NORMAL)
     l37.config(state=NORMAL)
     l38.config(state=NORMAL)
     l39.config(state=NORMAL)
     l40.config(state=NORMAL)
     t67.config(state=NORMAL,bd=1.8)
     c10.config(state=NORMAL)
     c12.config(state=NORMAL)
     c7.config(state=NORMAL)
     c11.config(state=NORMAL)
     click_PID_Bed() 
	 
   window1 = tk.Toplevel(root)
   window1.title("Heaters_Settings")#title of the window1
   window1.geometry('{}x{}'.format(680, 450))#size of the window1 at start up
   window1.resizable(width=False, height=False) #fixed window1 size
   window1.wm_iconbitmap('3DHex.ico')
   
   temp_settings_frame = Frame(window1,bd=1, relief=GROOVE) #create a frame for labels and boxes for settings
   temp_settings_frame.place(x=15,y=15) #the coordinates for placing the frame on root window1
   
   l19=Label(temp_settings_frame,text="Nozzle_Temp [C]",width=20,height=2,font=("Calibri", 12,'bold'),relief=FLAT) #create label2
   l19.grid(row=1, column=0) #choose to place labels with grid(there are also place and pack methods)   
   l20=Label(temp_settings_frame,text="Wait",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l20.grid(row=1, column=2) #choose to place labels with grid(there are also place and pack methods)
   l21=Label(temp_settings_frame,text="Heated_Nozzle",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l21.grid(row=0, column=0) #choose to place labels with grid(there are also place and pack methods)  
   l22=Label(temp_settings_frame,text="Heated_Bed",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l22.grid(row=0, column=2) #choose to place labels with grid(there are also place and pack methods)  
   l23=Label(temp_settings_frame,text="Enable_PID",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l23.grid(row=3, column=0) #choose to place labels with grid(there are also place and pack methods)
   l24=Label(temp_settings_frame,text="P_Nozzle",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l24.grid(row=3, column=1) #choose to place labels with grid(there are also place and pack methods)
   l25=Label(temp_settings_frame,text="I_Nozzle",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l25.grid(row=3, column=2) #choose to place labels with grid(there are also place and pack methods)
   l26=Label(temp_settings_frame,text="D_Nozzle",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l26.grid(row=3, column=3) #choose to place labels with grid(there are also place and pack methods)
   l27=Label(temp_settings_frame,text="Digit_Thermostat",width=15,height=4,font=("Calibri", 10),relief=FLAT) #create label2
   l27.grid(row=5, column=0,rowspan=2) #choose to place labels with grid(there are also place and pack methods)  
   l28=Label(temp_settings_frame,text="Therm_type",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l28.grid(row=5, column=1) #choose to place labels with grid(there are also place and pack methods)  
   l29=Label(temp_settings_frame,text="Differential [C]",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l29.grid(row=5, column=2) #choose to place labels with grid(there are also place and pack methods) 
   l30=Label(temp_settings_frame,text="Cycle [msec]",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l30.grid(row=5, column=3) #choose to place labels with grid(there are also place and pack methods)
   l31=Label(temp_settings_frame,text="Bed_Temp [C]",width=15,height=3,font=("Calibri", 12,'bold'),relief=FLAT) #create label2
   l31.grid(row=7, column=0) #choose to place labels with grid(there are also place and pack methods)   
   l32=Label(temp_settings_frame,text="Wait",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l32.grid(row=7, column=2) #choose to place labels with grid(there are also place and pack methods)  
   l33=Label(temp_settings_frame,text="Enable_PID",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l33.grid(row=8, column=0) #choose to place labels with grid(there are also place and pack methods)
   l34=Label(temp_settings_frame,text="P_Bed",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l34.grid(row=8, column=1) #choose to place labels with grid(there are also place and pack methods)
   l35=Label(temp_settings_frame,text="I_Bed",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l35.grid(row=8, column=2) #choose to place labels with grid(there are also place and pack methods)
   l36=Label(temp_settings_frame,text="D_Bed",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l36.grid(row=8, column=3) #choose to place labels with grid(there are also place and pack methods)
   l37=Label(temp_settings_frame,text="Digit_Thermostat",width=15,height=4,font=("Calibri", 10),relief=FLAT) #create label2
   l37.grid(row=10, column=0,rowspan=2) #choose to place labels with grid(there are also place and pack methods)   
   l38=Label(temp_settings_frame,text="Therm_type",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l38.grid(row=10, column=1) #choose to place labels with grid(there are also place and pack methods)  
   l39=Label(temp_settings_frame,text="Differential [C]",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l39.grid(row=10, column=2) #choose to place labels with grid(there are also place and pack methods) 
   l40=Label(temp_settings_frame,text="Cycle [msec]",width=15,height=2,font=("Calibri", 10),relief=FLAT) #create label2
   l40.grid(row=10, column=3) #choose to place labels with grid(there are also place and pack methods)
   
   t60=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t60.grid(row=1, column=1) #choose to place text box with grid(there are also place and pack methods)   
   t61=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t61.grid(row=4, column=1) #choose to place text box with grid(there are also place and pack methods)   
   t62=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t62.grid(row=4, column=2) #choose to place text box with grid(there are also place and pack methods) 
   t63=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t63.grid(row=4, column=3) #choose to place text box with grid(there are also place and pack methods) 
   t64=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t64.grid(row=6, column=1) #choose to place text box with grid(there are also place and pack methods)   
   t65=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t65.grid(row=6, column=2) #choose to place text box with grid(there are also place and pack methods) 
   t66=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t66.grid(row=6, column=3) #choose to place text box with grid(there are also place and pack methods)
   t67=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t67.grid(row=7, column=1) #choose to place text box with grid(there are also place and pack methods)    
   t68=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t68.grid(row=9, column=1) #choose to place text box with grid(there are also place and pack methods) 
   t69=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t69.grid(row=9, column=2) #choose to place text box with grid(there are also place and pack methods)
   t70=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t70.grid(row=9, column=3) #choose to place text box with grid(there are also place and pack methods)   
   t71=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t71.grid(row=11, column=1) #choose to place text box with grid(there are also place and pack methods) 
   t72=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t72.grid(row=11, column=2) #choose to place text box with grid(there are also place and pack methods)
   t73=Text(temp_settings_frame, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
   t73.grid(row=11, column=3) #choose to place text box with grid(there are also place and pack methods) 
   
   var7 = IntVar()  
   var8 = IntVar() 
   var9 = IntVar() 
   var10 = IntVar()
   var11 = IntVar()
   var12 =IntVar()
   with open(os.getenv('LOCALAPPDATA')+'\\3DHex\\temp settings.txt') as file: #read general setting file and set them
      array1 = file.readlines()
      if array1[0]!=' \n': t60.insert(END, array1[0]) #if fixes a bug
      if array1[1]!=' \n': t61.insert(END, array1[1])
      if array1[2]!=' \n': t62.insert(END, array1[2])
      if array1[3]!=' \n': t63.insert(END, array1[3])  
      if array1[4]!=' \n': t64.insert(END, array1[4])
      if array1[5]!=' \n': t65.insert(END, array1[5])
      if array1[6]!=' \n': t66.insert(END, array1[6])
      if array1[7]!=' \n': t67.insert(END, array1[7])    
      if array1[8]!=' \n': t68.insert(END, array1[8])
      if array1[9]!=' \n': t69.insert(END, array1[9])
      if array1[10]!=' \n': t70.insert(END, array1[10])
      if array1[11]!=' \n': t71.insert(END, array1[11])    
      if array1[12]!=' \n': t72.insert(END, array1[12])
      if array1[13]!=' \n': t73.insert(END, array1[13])    
      var7.set(int(array1[14]))
      var8.set(int(array1[15]))
      var9.set(int(array1[16]))
      var10.set(int(array1[17]))
      var11.set(int(array1[18]))
      var12.set(int(array1[19]))
   c7=Checkbutton(temp_settings_frame, variable=var7,onvalue=1, offvalue=0)
   c7.grid(row=1, column=3) 
   c8=Checkbutton(temp_settings_frame, variable=var8,onvalue=1, offvalue=0,command=click_Heated_Nozzle)
   c8.grid(row=0, column=1)
   c9=Checkbutton(temp_settings_frame, variable=var9,onvalue=1, offvalue=0,command=click_Heated_Bed)
   c9.grid(row=0, column=3)
   c10=Checkbutton(temp_settings_frame, variable=var10,height = 2,onvalue=1, offvalue=0,command=click_PID_Nozzle)
   c10.grid(row=4, column=0)
   c11=Checkbutton(temp_settings_frame, variable=var11,onvalue=1, offvalue=0)
   c11.grid(row=7, column=3) 
   c12=Checkbutton(temp_settings_frame, variable=var12, height = 2,onvalue=1, offvalue=0,command=click_PID_Bed)
   c12.grid(row=9, column=0)   
   b3=Button(window1, text="SAVE",font=("Calibri", 10,'bold'),width = 12, height = 2,command=save_temperature_settings)#must be inside the function
   b3.place(x=550,y=120)
   b4=Button(window1, text="EXIT",font=("Calibri", 10,'bold'),width = 12, height = 2,command=window1.destroy)#must be inside the function
   b4.place(x=550,y=230)
   
   click_Heated_Nozzle()
   click_Heated_Bed()

root = Tk() #creates app window
root.title("3DHex")#title of the window
root.geometry('{}x{}'.format(1000, 530))#size of the window at start up
root.resizable(width=False, height=False)
 
textbox_frame = Frame(root, bd=4, relief=GROOVE) #create a frame for text box
textbox_frame.place(relx=0.625,rely=0.025) #place the frame at those coordinates in window app
xscrollbar = Scrollbar(textbox_frame, orient=HORIZONTAL)#create horizontal scrollbar
xscrollbar.grid(row=1, column=0, sticky=E+W) #place it at the (bottom) 
yscrollbar = Scrollbar(textbox_frame) #create vertical scrollbar
yscrollbar.grid(row=0, column=1, sticky=N+S) #place it (right)
text = Text(textbox_frame, wrap=NONE, bd=0, #create a text box and place the above scrollbars
            xscrollcommand=xscrollbar.set,
            yscrollcommand=yscrollbar.set,width = 33, height = 20,font=("Calibri", 14))
text.grid(row=0, column=0, sticky=N+S+E+W) #choose to place text box with grid(there are also place and pack methods)
xscrollbar.config(command=text.xview) #this is for scrollbars to move with cursor
yscrollbar.config(command=text.yview)


settings_frame_1 = Frame(root,bd=2, relief=GROOVE) #create a frame for labels and boxes for settings
settings_frame_1.place(relx=0.01,rely=0.025) #the coordinates for placing the frame on root window
settings_frame_2 = Frame(root,bd=2, relief=GROOVE) #create a frame for labels and boxes for settings
settings_frame_2.place(relx=0.03,rely=0.64) #the coordinates for placing the frame on root window

l1=Label(settings_frame_1,text="GENERAL",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
l1.grid(row=0, column=1) #choose to place labels with grid(there are also place and pack methods)
l8=Label(settings_frame_1,text="X_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
l8.grid(row=0, column=2) #choose to place labels with grid(there are also place and pack methods)
l9=Label(settings_frame_1,text="Y_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
l9.grid(row=0, column=3) #choose to place labels with grid(there are also place and pack methods)
l10=Label(settings_frame_1,text="Z_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
l10.grid(row=0, column=4) #choose to place labels with grid(there are also place and pack methods)
l12=Label(settings_frame_1,text="E_AXIS",width=7,height=1,font=("Calibri", 16,'bold'),relief=FLAT) #create label1
l12.grid(row=0, column=5) #choose to place labels with grid(there are also place and pack methods)
l2=Label(settings_frame_1,text="Steps_Per_Unit [steps/mm]",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l2.grid(row=2, column=0) #choose to place labels with grid(there are also place and pack methods)
l3=Label(settings_frame_1,text="Max_Feedrate [mm/s]",width=25,height=2,font=("Calibri", 10),relief=FLAT)
l3.grid(row=3, column=0)
l4=Label(settings_frame_1,text="Acceleration [mm/s^2]",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l4.grid(row=4, column=0) #choose to place labels with grid(there are also place and pack methods)
l6=Label(settings_frame_1,text="Jerk [mm/s^3]",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l6.grid(row=5, column=0) #choose to place labels with grid(there are also place and pack methods)
l7=Label(settings_frame_1,text="Jump_Feedrate [mm/s]",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l7.grid(row=6, column=0) #choose to place labels with grid(there are also place and pack methods)
l11=Label(settings_frame_1,text="Invert_Direction",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l11.grid(row=8, column=0) #choose to place labels with grid(there are also place and pack methods)
l15=Label(settings_frame_1,text="Parking [mm/s]-[mm]",width=25,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l15.grid(row=7, column=0) #choose to place labels with grid(there are also place and pack methods)
l16=Label(settings_frame_1,text="Enable_Steppers",width=25,height=1,font=("Calibri", 10),relief=FLAT) #create label2
l16.grid(row=1, column=0) #choose to place labels with grid(there are also place and pack methods)

l17=Label(settings_frame_2,text="Service_Routine [KHz]",width=20,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l17.grid(row=0, column=0) #choose to place labels with grid(there are also place and pack methods)
l18=Label(settings_frame_2,text="Max_File_Size [GB]",width=20,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l18.grid(row=0, column=1) #choose to place labels with grid(there are also place and pack methods)
l19=Label(settings_frame_2,text="COM_PORT",width=20,height=2,font=("Calibri", 10),relief=FLAT) #create label2
l19.grid(row=2, column=0) #choose to place labels with grid(there are also place and pack methods)
l21=Label(settings_frame_2,text="Baud_Rate",width=20,height=3,font=("Calibri", 10),relief=FLAT) #create label2
l21.grid(row=4, column=0) #choose to place labels with grid(there are also place and pack methods)

t1=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled') #create text box
t1.grid(row=2, column=1) #choose to place text box with grid(there are also place and pack methods)
t2=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t2.grid(row=2, column=2) #choose to place text box with grid(there are also place and pack methods)
t3=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t3.grid(row=2, column=3) #choose to place text box with grid(there are also place and pack methods)
t4=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t4.grid(row=2, column=4) #choose to place text box with grid(there are also place and pack methods)
t5=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t5.grid(row=2, column=5) #choose to place text box with grid(there are also place and pack methods)
t6=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled')
t6.grid(row=3, column=1)
t7=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t7.grid(row=3, column=2) #choose to place text box with grid(there are also place and pack methods)
t8=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t8.grid(row=3, column=3) #choose to place text box with grid(there are also place and pack methods)
t9=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t9.grid(row=3, column=4) #choose to place text box with grid(there are also place and pack methods)
t10=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t10.grid(row=3, column=5) #choose to place text box with grid(there are also place and pack methods)
t11=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t11.grid(row=4, column=1) #choose to place text box with grid(there are also place and pack methods)
t12=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t12.grid(row=4, column=2) #choose to place text box with grid(there are also place and pack methods)
t13=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t13.grid(row=4, column=3) #choose to place text box with grid(there are also place and pack methods)
t14=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t14.grid(row=4, column=4) #choose to place text box with grid(there are also place and pack methods)
t15=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t15.grid(row=4, column=5) #choose to place text box with grid(there are also place and pack methods)
t16=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t16.grid(row=5, column=1) #choose to place text box with grid(there are also place and pack methods)
t17=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled') #create text box
t17.grid(row=5, column=2) #choose to place text box with grid(there are also place and pack methods)
t18=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled') #create text box
t18.grid(row=5, column=3) #choose to place text box with grid(there are also place and pack methods)
t19=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled') #create text box
t19.grid(row=5, column=4) #choose to place text box with grid(there are also place and pack methods)
t20=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled') #create text box
t20.grid(row=5, column=5) #choose to place text box with grid(there are also place and pack methods)
t21=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t21.grid(row=6, column=1) #choose to place text box with grid(there are also place and pack methods)
t22=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t22.grid(row=6, column=2) #choose to place text box with grid(there are also place and pack methods)
t23=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t23.grid(row=6, column=3) #choose to place text box with grid(there are also place and pack methods)
t24=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t24.grid(row=6, column=4) #choose to place text box with grid(there are also place and pack methods)
t25=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t25.grid(row=6, column=5) #choose to place text box with grid(there are also place and pack methods)
t26=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t26.grid(row=7, column=1) #choose to place text box with grid(there are also place and pack methods)
t27=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t27.grid(row=7, column=2) #choose to place text box with grid(there are also place and pack methods)
t28=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t28.grid(row=7, column=3) #choose to place text box with grid(there are also place and pack methods)
t29=Text(settings_frame_1, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t29.grid(row=7, column=4) #choose to place text box with grid(there are also place and pack methods)
t30=Text(settings_frame_1, wrap=NONE, bd=0,width = 7, height = 1,font=("Calibri", 12),state='disabled') #create text box
t30.grid(row=7, column=5) #choose to place text box with grid(there are also place and pack methods)

t31=Text(settings_frame_2, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t31.grid(row=1, column=0) #choose to place text box with grid(there are also place and pack methods)
t32=Text(settings_frame_2, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t32.grid(row=1, column=1) #choose to place text box with grid(there are also place and pack methods)
t33=Text(settings_frame_2, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t33.grid(row=3, column=0) #choose to place text box with grid(there are also place and pack methods)
t34=Text(settings_frame_2, wrap=NONE, bd=1.8,width = 7, height = 1,font=("Calibri", 12)) #create text box
t34.grid(row=4, column=1) #choose to place text box with grid(there are also place and pack methods)

var1 = IntVar()
var2 = IntVar()
var3 = IntVar()
var4 = IntVar()
var5 = IntVar()
var101 = IntVar()
var102 = IntVar()
var103 = IntVar()
var104 = IntVar()
var105 = IntVar()
var15= IntVar()
var6 = IntVar()
var7 = IntVar()


#var6.set(1) #here we set the value of the check box "disabled"

with open(os.getenv('LOCALAPPDATA')+'\\3DHex\\general settings.txt') as file: #read general setting file and set them
   array0 = file.readlines()
   if array0[0]!=' \n': t1.insert(END, array0[0]) #if fixes a bug
   if array0[1]!=' \n': t2.insert(END, array0[1])
   if array0[2]!=' \n': t3.insert(END, array0[2])
   if array0[3]!=' \n': t4.insert(END, array0[3])  
   if array0[4]!=' \n': t5.insert(END, array0[4])
   if array0[5]!=' \n': t6.insert(END, array0[5])
   if array0[6]!=' \n': t7.insert(END, array0[6])
   if array0[7]!=' \n': t8.insert(END, array0[7])    
   if array0[8]!=' \n': t9.insert(END, array0[8])
   if array0[9]!=' \n': t10.insert(END, array0[9])
   if array0[10]!=' \n': t11.insert(END, array0[10])
   if array0[11]!=' \n': t12.insert(END, array0[11])    
   if array0[12]!=' \n': t13.insert(END, array0[12])
   if array0[13]!=' \n': t14.insert(END, array0[13])
   if array0[14]!=' \n': t15.insert(END, array0[14])        
   if array0[15]!=' \n': t16.insert(END, array0[15])
   if array0[16]!=' \n': t17.insert(END, array0[16])
   if array0[17]!=' \n': t18.insert(END, array0[17])    
   if array0[18]!=' \n': t19.insert(END, array0[18])
   if array0[19]!=' \n': t20.insert(END, array0[19])
   if array0[20]!=' \n': t21.insert(END, array0[20])
   if array0[21]!=' \n': t22.insert(END, array0[21])
   if array0[22]!=' \n': t23.insert(END, array0[22]) 
   if array0[23]!=' \n': t24.insert(END, array0[23])
   if array0[24]!=' \n': t25.insert(END, array0[24]) 
   if array0[25]!=' \n': t26.insert(END, array0[25])
   if array0[26]!=' \n': t27.insert(END, array0[26])
   if array0[27]!=' \n': t28.insert(END, array0[27])
   if array0[28]!=' \n': t29.insert(END, array0[28])
   if array0[29]!=' \n': t30.insert(END, array0[29]) 
   if array0[30]!=' \n': t31.insert(END, array0[30])
   if array0[31]!=' \n': t32.insert(END, array0[31])
   if array0[32]!=' \n': t33.insert(END, array0[32])
   if array0[33]!=' \n': t34.insert(END, array0[33]) 
   var1.set(int(0))
   var2.set(int(array0[35]))
   var3.set(int(array0[36]))
   var4.set(int(array0[37]))
   var5.set(int(array0[38]))
   var15.set(int(array0[39]))
   var101.set(int(0))
   var102.set(int(array0[41]))
   var103.set(int(array0[42]))
   var104.set(int(array0[43]))
   var105.set(int(array0[44]))

def clear_text():
    text.delete('1.0' , END)
	
def click_check_units():
    if var15.get()==0:
       l20=Label(settings_frame_2,text="Units [mm/sec]",width=20,height=2,font=("Calibri", 10),relief=FLAT) #create label2
       l20.grid(row=2, column=1) #choose to place labels with grid(there are also place and pack methods)	 
    else:
       l20=Label(settings_frame_2,text="Units [mm/min]",width=20,height=2,font=("Calibri", 10),relief=FLAT) #create label2
       l20.grid(row=2, column=1) #choose to place labels with grid(there are also place and pack methods)
	
click_check_units()
c1=Checkbutton(settings_frame_1, variable=var1,onvalue=1, offvalue=0,state='disabled')
c1.grid(row=8, column=1)
c2=Checkbutton(settings_frame_1, variable=var2,onvalue=1, offvalue=0)
c2.grid(row=8, column=2)
c3=Checkbutton(settings_frame_1, variable=var3,onvalue=1, offvalue=0)
c3.grid(row=8, column=3)
c4=Checkbutton(settings_frame_1, variable=var4,onvalue=1, offvalue=0)
c4.grid(row=8, column=4)
c5=Checkbutton(settings_frame_1, variable=var5,onvalue=1, offvalue=0)
c5.grid(row=8, column=5)
c15=Checkbutton(settings_frame_2, variable=var15,onvalue=1, offvalue=0,command=click_check_units)
c15.grid(row=3, column=1)
c101=Checkbutton(settings_frame_1, variable=var101,onvalue=1, offvalue=0,state='disabled')
c101.grid(row=1, column=1)
c102=Checkbutton(settings_frame_1, variable=var102,onvalue=1, offvalue=0)
c102.grid(row=1, column=2)
c103=Checkbutton(settings_frame_1, variable=var103,onvalue=1, offvalue=0)
c103.grid(row=1, column=3)
c104=Checkbutton(settings_frame_1, variable=var104,onvalue=1, offvalue=0)
c104.grid(row=1, column=4)
c105=Checkbutton(settings_frame_1, variable=var105,onvalue=1, offvalue=0)
c105.grid(row=1, column=5)

render1 = ImageTk.PhotoImage(Image.open("HOME.png").resize((25,25), Image.ANTIALIAS))
render2 = ImageTk.PhotoImage(Image.open("ANGLE.png").resize((25,25), Image.ANTIALIAS))
b6=Button(root, bg= "grey",bd=1,command = homing_settings)
b7=Button(root, bg= "grey",bd=1,command = angle_settings)
b6.place(relx=0.625,rely=0.942)
b7.place(relx=0.660,rely=0.942)
b6.config(image=render1)
b7.config(image=render2)
b1=Button(root, text="SD CARD",font=("Calibri", 12,'bold'),width = 10, height = 3,command = SD_calculate)
b1.place(relx=0.485,rely=0.815)
b2=Button(root, text="HEATERS",font=("Calibri", 12,'bold'),width = 10, height = 3,command = temp_settings)
b2.place(relx=0.375,rely=0.655)
b3=Button(root,width = 2, height = 1, bg= "red",bd=2,command = clear_text)
b3.place(relx=0.955,rely=0.948)
b4=Button(root, text="SAVE",font=("Calibri", 12,'bold'),width = 10, height = 3,command = save_general_settings)
b4.place(relx=0.485,rely=0.655)
b5=Button(root, text="USB",font=("Calibri", 12,'bold'),width = 10, height = 3,command = USB_calculate)
b5.place(relx=0.375,rely=0.815)


class AppUI(Frame): #create the menu bar

    def __init__(self, master=None):
        Frame.__init__(self, master, relief=SUNKEN, bd=2)

        self.menubar = Menu(self)

        menu = Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="File", menu=menu)
        menu.add_command(label="Open",command = openfile)

        menu = Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="Help", menu=menu)
        menu.add_command(label="Guide",command = guide)
        menu.add_command(label="About",command = about)
		
        menu = Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="Exit", command=exit) #terminate the app

        try:
            self.master.config(menu=self.menubar)
        except AttributeError:
            # master is a toplevel window (Python 1.4/Tkinter 1.63)
            self.master.tk.call(master, "config", "-menu", self.menubar)

def main():			
   app = AppUI(root)
   app.pack()
   root.wm_iconbitmap('3DHex.ico')
   root.protocol("WM_DELETE_WINDOW", on_closing)
   root.mainloop()

if __name__ == "__main__": #https://stackoverflow.com/questions/12297892/tkinter-launching-process-via-multiprocessing-creates-not-wanted-new-window
  multiprocessing.freeze_support() #very important in order for the compiled version to operate https://stackoverflow.com/questions/33970690/why-python-executable-opens-new-window-instance-when-function-by-multiprocessing
  main()
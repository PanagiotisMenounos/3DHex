/*
----------------------------------------------------------------------
COPYRIGHT NOTICE FOR 3DHex:
----------------------------------------------------------------------
3DHex - 3D Printer Firmware
Copyright (c) 2021 Panagiotis Menounos
Contact: 3DHexfw@gmail.com
3DHex is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
3DHex is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with 3DHex.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <main.h>


void configure_idle_pin_modes(){
   pinMode(buffer5.X_ENABLE_PIN,OUTPUT);
   pinMode(buffer5.Y_ENABLE_PIN,OUTPUT);
   pinMode(buffer5.Z_ENABLE_PIN,OUTPUT);
   pinMode(buffer5.E_ENABLE_PIN,OUTPUT); 
   pinMode(buffer5.X_STEP_PIN,OUTPUT);
   pinMode(buffer5.Y_STEP_PIN,OUTPUT);
   pinMode(buffer5.Z_STEP_PIN,OUTPUT);
   pinMode(buffer5.E_STEP_PIN,OUTPUT);
   pinMode(buffer5.X_DIR_PIN,OUTPUT);
   pinMode(buffer5.Y_DIR_PIN,OUTPUT);
   pinMode(buffer5.Z_DIR_PIN,OUTPUT);
   pinMode(buffer5.E_DIR_PIN,OUTPUT);
   pinMode(buffer5.N_SENSOR_PIN,INPUT);
   pinMode(buffer5.B_SENSOR_PIN,INPUT);
   pinMode(buffer5.N_HEATER_PIN,OUTPUT);
   pinMode(buffer5.B_HEATER_PIN,OUTPUT);
   pinMode(buffer5.N_FAN_PIN,OUTPUT);
   pinMode(buffer5.FAN_PIN,OUTPUT);   
   pinMode(buffer5.X_ENDSTOP_PIN,INPUT);
   pinMode(buffer5.Y_ENDSTOP_PIN,INPUT);
   pinMode(buffer5.Z_ENDSTOP_PIN,INPUT);
   pinMode(SD_ENABLE,OUTPUT);
   digitalWrite(SD_ENABLE,HIGH);
   digitalWrite(SD_ENABLE,HIGH);
   digitalWrite(buffer5.N_HEATER_PIN,LOW);
   digitalWrite(buffer5.B_HEATER_PIN,LOW);

   ///////////////***********THERMAL PINS*****************////////////////////
   therm1._pin=buffer5.N_SENSOR_PIN;
   therm1._sensorNumber=buffer3.THERMISTOR_TYPE_NOZZLE;
   therm2._pin=buffer5.B_SENSOR_PIN;
   therm2._sensorNumber=buffer3.THERMISTOR_TYPE_BED;

   ///////////////***********LCD LIQUID CRYSTAL***********////////////////////
   //lcd._rs_pin=16;
   //lcd._enable_pin=17;
   //lcd._rw_pin = 255;
   //lcd._data_pins[0]=23;
   //lcd._data_pins[1]=25;
   //lcd._data_pins[2]=27;
   //lcd._data_pins[3]=29;
   //lcd.begin(20,4);delay(300);
   //lcd.setCursor(0, 0);lcd.print("MODE: CHECK INPUT");
}

void configure_pin_modes(){
   pinMode(buffer3.X_ENABLE_PIN,OUTPUT);
   pinMode(buffer3.Y_ENABLE_PIN,OUTPUT);
   pinMode(buffer3.Z_ENABLE_PIN,OUTPUT);
   pinMode(buffer3.E_ENABLE_PIN,OUTPUT); 
   pinMode(buffer3.X_STEP_PIN,OUTPUT);
   pinMode(buffer3.Y_STEP_PIN,OUTPUT);
   pinMode(buffer3.Z_STEP_PIN,OUTPUT);
   pinMode(buffer3.E_STEP_PIN,OUTPUT);
   pinMode(buffer3.X_DIR_PIN,OUTPUT);
   pinMode(buffer3.Y_DIR_PIN,OUTPUT);
   pinMode(buffer3.Z_DIR_PIN,OUTPUT);
   pinMode(buffer3.E_DIR_PIN,OUTPUT);
   pinMode(buffer3.N_SENSOR_PIN,INPUT);
   pinMode(buffer3.B_SENSOR_PIN,INPUT);
   pinMode(buffer3.N_HEATER_PIN,OUTPUT);
   pinMode(buffer3.B_HEATER_PIN,OUTPUT);
   pinMode(buffer3.N_FAN_PIN,OUTPUT);
   pinMode(buffer3.FAN_PIN,OUTPUT);  
   pinMode(13,OUTPUT);   
   pinMode(buffer3.X_ENDSTOP_PIN,INPUT);
   pinMode(buffer3.Y_ENDSTOP_PIN,INPUT);
   pinMode(buffer3.Z_ENDSTOP_PIN,INPUT);
   pinMode(SD_ENABLE,OUTPUT);
   digitalWrite(SD_ENABLE,HIGH);
   digitalWrite(SD_ENABLE,HIGH);
   digitalWrite(buffer3.N_HEATER_PIN,LOW);
   digitalWrite(buffer3.B_HEATER_PIN,LOW);
   therm1._pin=buffer3.N_SENSOR_PIN;
   therm1._sensorNumber=buffer3.THERMISTOR_TYPE_NOZZLE;
   therm2._pin=buffer3.B_SENSOR_PIN;
   therm2._sensorNumber=buffer3.THERMISTOR_TYPE_BED;
   digitalWrite(13,HIGH);
}

void configure_pin_ports(){
   portxstep = portOutputRegister(digitalPinToPort(buffer3.X_STEP_PIN));
   portxstepmask = digitalPinToBitMask(buffer3.X_STEP_PIN);
   portxdir = portOutputRegister(digitalPinToPort(buffer3.X_DIR_PIN));
   portxdirmask = digitalPinToBitMask(buffer3.X_DIR_PIN);
   
   portystep = portOutputRegister(digitalPinToPort(buffer3.Y_STEP_PIN));
   portystepmask = digitalPinToBitMask(buffer3.Y_STEP_PIN);
   portydir = portOutputRegister(digitalPinToPort(buffer3.Y_DIR_PIN));
   portydirmask = digitalPinToBitMask(buffer3.Y_DIR_PIN);
   
   portzstep = portOutputRegister(digitalPinToPort(buffer3.Z_STEP_PIN));
   portzstepmask = digitalPinToBitMask(buffer3.Z_STEP_PIN);
   portzdir = portOutputRegister(digitalPinToPort(buffer3.Z_DIR_PIN));
   portzdirmask = digitalPinToBitMask(buffer3.Z_DIR_PIN);

   portestep = portOutputRegister(digitalPinToPort(buffer3.E_STEP_PIN));
   portestepmask = digitalPinToBitMask(buffer3.E_STEP_PIN);
   portedir = portOutputRegister(digitalPinToPort(buffer3.E_DIR_PIN));
   portedirmask = digitalPinToBitMask(buffer3.E_DIR_PIN);
}

void initialization_var(){
   buffer3.X_ENABLE=1;
   buffer3.Y_ENABLE=1;
   buffer3.Z_ENABLE=1;
   buffer3.E_ENABLE=1;
   buffer4.command=-253; //-253 Idle
   buffer5.A=0;
   buffer5.B=0;
   buffer5.C=0;
   PRINTING=0;
   i=0;j=0;signal_duration=0;off=0;on=0;bufferstate=true;readstate=false;
   setoff=false;
   nozz_block=false;bed_block=false;once=false;GM_command=false;GM_init=false;
   terminate_counter=0;
}

void setup() {
   bltouch.attach(BLTOUCH_PIN);
   pinMode(ENCODER_PIN,INPUT_PULLUP);
   pinMode(SD_ENABLE,OUTPUT);
   Serial.begin(SERIAL_BAUD_RATE);
   pinMode(LED_BUILTIN,OUTPUT);
   pidbed.SetMode(AUTOMATIC);
   pidbed.SetSampleTime(0);
   pidnozz.SetMode(AUTOMATIC);
   pidnozz.SetSampleTime(0);
   initialization_var();
   
   delay(100); 
   bltouch.write(90);
   do{// blocking if no input
          
   }while(check_inputs());
   PRINTING=true;
   if(PRINT_STATE==0){get_USB_settings();}else{get_SD_settings();}
   if(LCD_16x4==true){lcd.noBlink();}
   check_steppers();
   
   configure_pin_modes();
   configure_pin_ports();   
   
   if((buffer3.Wait_nozz==1 && buffer3.HEATED_NOZZLE==1) || (buffer3.Wait_bed==1 && buffer3.HEATED_BED==1)){
      if(LCD_16x4==true && setoff==false){lcd.setCursor(0, 2);lcd.print("HEAT UP");lcd.blink();delay(1200);}
   }else{
      if(LCD_16x4==true && setoff==false){lcd.setCursor(0, 2);lcd.print("HEAT UP.");delay(1200);}
   }
   PRINTING=false;
   temperature_control(); //blocking if heat up first is enabled
   PRINTING=true; //printing process has started
   buffer4.nozz_temp = therm1.analog2temp(); // nozzle
   buffer4.bed_temp = therm2.analog2temp();  // bed
   buffer4.command = -10;
   Serial.write((char*)&buffer4,sizeof(buffer4));
   delay(1000);
   if(LCD_16x4==true && setoff==false){lcd.noBlink();lcd.setCursor(0, 3);lcd.print("PRINTING..");lcd.blink();delay(1200);}
   Timer3.initialize(time_duration); //[microseconds]
   Timer3.attachInterrupt(service_routine);
}

void loop(){
  if(setoff==false){
     if(PRINT_STATE==0){
        read_serial();
     }else{
        read_sd();
     }
     if(GM_init==true){
        read_GM_data();
        if(homing == true){
           delay_homing=true;
           bltouch.write(10);
           delay(300);
           delay_homing=false;
        }
        temperature_control();
        position_report();
        GM_init=false;
     }
     if(deploy_probe==true){
        delay_homing=true;
        bltouch.write(10);
        delay(300);
        deploy_probe=false;
        delay_homing=false;
     }
     if(probe_up==true){
        if(ABL && save_probe==false){
          buffer4.command =-301; //ABL_ZTrack_Packet
          buffer4.nozz_temp = z_tracking;
          Serial.write((char*)&buffer4,sizeof(buffer4));
        }
        if(save_probe==true && ABL){
           buffer4.command =-303; //ABL_end iteration
           buffer4.nozz_temp = z_tracking;
           Serial.write((char*)&buffer4,sizeof(buffer4));
           save_probe=false;
           GM_command=false;
        }
        bltouch.write(90);
        probe_up=false;
     }
     temperature_control();
     position_report();
     check_BUTTON_terminate();
     check_USB_terminate();
  }else{
     position_report(); 
     terminate_process();
     digitalWrite(buffer5.N_HEATER_PIN,LOW);
     digitalWrite(buffer5.B_HEATER_PIN,LOW);
  }
}

void push_bits(){
   cli();
   i++;
   if(bufferstate==true && i==buffer1.byte_1[j] && buffer1.byte_1[j]!=0){
      j++;
      
      if (bitRead(buffer1.byte_1[j],0)){
        *portxdir |= portxdirmask;
      }else{
        *portxdir &= ~portxdirmask;
      } 
      if (bitRead(buffer1.byte_1[j],1)){
        *portxstep |= portxstepmask;
      }else{
        *portxstep &= ~portxstepmask;
      } 

      if (bitRead(buffer1.byte_1[j],2)){
        *portydir |= portydirmask;
      }else{
        *portydir &= ~portydirmask;
      } 
      if (bitRead(buffer1.byte_1[j],3)){
        *portystep |= portystepmask;
      }else{
        *portystep &= ~portystepmask;
      } 

      if (bitRead(buffer1.byte_1[j],4)){
        *portzdir |= portzdirmask;
      }else{
        *portzdir &= ~portzdirmask;
      } 
      if (bitRead(buffer1.byte_1[j],5)){
        *portzstep |= portzstepmask;
      }else{
        *portzstep &= ~portzstepmask;
      } 

      if (bitRead(buffer1.byte_1[j],6)){
        *portedir |= portedirmask;
      }else{
        *portedir &= ~portedirmask;
      } 
      if (bitRead(buffer1.byte_1[j],7)){
        *portestep |= portestepmask;
      }else{
        *portestep &= ~portestepmask;
      } 
      
      j++;
      i=0;
      if (bitRead(buffer1.byte_1[j-1],1)){
        if(bitRead(buffer1.byte_1[j-1],0)){
          XPOS=XPOS+1;
        }else{
          XPOS=XPOS-1;
        }
      }
      if (bitRead(buffer1.byte_1[j-1],3)){
        if(bitRead(buffer1.byte_1[j-1],2)){
          YPOS=YPOS+1;
        }else{
          YPOS=YPOS-1;
        }
      }
      if (bitRead(buffer1.byte_1[j-1],5)){
        if(bitRead(buffer1.byte_1[j-1],4)){
          ZPOS=ZPOS+1;
        }else{
          ZPOS=ZPOS-1;
        }
      }
      terminate_counter=0;
   }else if(bufferstate==false && i==buffer2.byte_2[j] && buffer2.byte_2[j]!=0){
      j++;
      
      if (bitRead(buffer2.byte_2[j],0)){
        *portxdir |= portxdirmask;
      }else{
        *portxdir &= ~portxdirmask;
      } 
      if (bitRead(buffer2.byte_2[j],1)){
        *portxstep |= portxstepmask;
      }else{
        *portxstep &= ~portxstepmask;
      } 

      if (bitRead(buffer2.byte_2[j],2)){
        *portydir |= portydirmask;
      }else{
        *portydir &= ~portydirmask;
      } 
      if (bitRead(buffer2.byte_2[j],3)){
        *portystep |= portystepmask;
      }else{
        *portystep &= ~portystepmask;
      } 

      if (bitRead(buffer2.byte_2[j],4)){
        *portzdir |= portzdirmask;
      }else{
        *portzdir &= ~portzdirmask;
      } 
      if (bitRead(buffer2.byte_2[j],5)){
        *portzstep |= portzstepmask;
      }else{
        *portzstep &= ~portzstepmask;
      } 

      if (bitRead(buffer2.byte_2[j],6)){
        *portedir |= portedirmask;
      }else{
        *portedir &= ~portedirmask;
      } 
      if (bitRead(buffer2.byte_2[j],7)){
        *portestep |= portestepmask;
      }else{
        *portestep &= ~portestepmask;
      }
       
      j++;
      i=0;
      if (bitRead(buffer2.byte_2[j-1],1)){
        if(bitRead(buffer2.byte_2[j-1],0)){
          XPOS=XPOS+1;
        }else{
          XPOS=XPOS-1;
        }
      }
      if (bitRead(buffer2.byte_2[j-1],3)){
        if(bitRead(buffer2.byte_2[j-1],2)){
          YPOS=YPOS+1;
        }else{
          YPOS=YPOS-1;
        }
      }
      if (bitRead(buffer2.byte_2[j-1],5)){
        if(bitRead(buffer2.byte_2[j-1],4)){
          ZPOS=ZPOS+1;
        }else{
          ZPOS=ZPOS-1;
        }
      }
      terminate_counter=0;
   }else{
    *portxstep &= ~portxstepmask;
    *portystep &= ~portystepmask;
    *portzstep &= ~portzstepmask;
    *portestep &= ~portxstepmask;
   }
   check_buffer();
   sei(); 
}

void position_report(){
  currentMillis_pos = millis();
  if (currentMillis_pos - previousMillis_USBupdate_pos >= 200) {
       previousMillis_USBupdate_pos=currentMillis_pos;
           buffer4.command = -243;
           buffer4.x_pos=XPOS;
           buffer4.y_pos=YPOS;
           buffer4.z_pos=ZPOS;
           Serial.write((char*)&buffer4,sizeof(buffer4));
  }

}

void check_buffer(){
   if (j>=BUFFERSIZE){ //just reset to zero so the buffer locate again at the start point (first object e.g a[0])
      bufferstate=!bufferstate;
      state_r=bufferstate;
      j=0;
      readstate=true; //one of the buffers has pushed all the way so fill it with new values
   }
}

void check_command(int state_c){
  if(buffer1.byte_1[j]==command_value && state_c==0){
       GM_command=true;
       GM_init=true;
       state_r=0;
       i=0;
  }
  if(buffer2.byte_2[j]==command_value && state_c==1){
    GM_command=true;
    GM_init=true;
    state_r=1;
    i=0;
  }
}

void read_GM_data(){
    int temp_counter=0;
    while(buffer1.byte_1[j]==command_value){ //clear command_values
      j++;
      temp_counter++;
      if (j>=BUFFERSIZE){ //just reset to zero so the buffer locate again at the start point (first object e.g a[0])
         check_buffer();
         temp_counter++;
      }
    }
    while(buffer2.byte_2[j]==command_value){ //clear command_values
      temp_counter++;
      
      j++;
      if (j>=BUFFERSIZE){ //just reset to zero so the buffer locate again at the start point (first object e.g a[0])
         check_buffer();
         temp_counter++;
         
      }
    }
    if(temp_counter>2){if(state_r==0){state_r=1;}else{state_r=0;}}//that state means that the data is at the other buffer
    for(cc=0;cc<sizeof(buffer5);cc++){//total bytes of data
      if(state_r==0){
        while(buffer1.byte_1[j]==command_value){ //clear command_values
            j++;
        }
        MG_buf[cc]=buffer1.byte_1[j];
        j++;
      }else{
      
        while(buffer2.byte_2[j]==command_value){ //clear command_values
          j++;
        }
        MG_buf[cc]=buffer2.byte_2[j];
        j++;
      }
    }
    memmove(&buffer5,MG_buf,sizeof(buffer5));
    execute_command();
}

void execute_command(){
  unsigned long timestamp=0;
  switch (buffer5.A){
    case 0: //set bed temp
       //digitalWrite(LED_BUILTIN,HIGH);
       bed_block=true;
       buffer3.BED_TEMP=buffer5.J;
       bedtemp = buffer3.BED_TEMP;
       buffer3.Wait_bed=buffer5.B;
       set_temp=true;
    break;
    case 1: //set hotend temp
       nozz_block=true;
       buffer3.NOZZLE_TEMP=buffer5.J;
       nozztemp = buffer3.NOZZLE_TEMP;
       buffer3.Wait_nozz=buffer5.B;
       set_temp=true;
    break;
    case 2: //enable/disable stepper
       buffer3.X_ENABLE=buffer5.B;
       buffer3.Y_ENABLE=buffer5.C;
       buffer3.Z_ENABLE=buffer5.D;
       buffer3.E_ENABLE=buffer5.E;
       check_steppers();
    break;
    case 3: //homing
       buffer3.HOME_X_ENABLE=buffer5.C;
       buffer3.HOME_Y_ENABLE=buffer5.D;
       buffer3.HOME_Z_ENABLE=buffer5.E;
       buffer3.HOME_X_STATE=buffer5.F;
       buffer3.HOME_Y_STATE=buffer5.G;
       buffer3.HOME_Z_STATE=buffer5.H;
       buffer3.HOME_X_DIR=buffer5.I;
       buffer3.HOME_Y_DIR=buffer5.J;
       buffer3.HOME_Z_DIR=buffer5.K;
       buffer3.HOME_X_DURATION=buffer5.O;
       buffer3.HOME_Y_DURATION=buffer5.P;
       buffer3.HOME_Z_DURATION=buffer5.Q;
       homing=true;
       delay_homing=true;
    break;
    case 4: //fan control
       analogWrite(buffer5.FAN_PIN,buffer5.J);
       GM_command=false;
    break;
    case 5: //pause for time
       dwell=true;
       if(buffer5.J!=0){
          timestamp = millis();
          while(millis()-timestamp<buffer5.J){
            temperature_control();
            on=millis(); //prevent termination from check_Button_terminate()
          }
       }else if(buffer5.K!=0){
          buffer5.K=buffer5.K*1000;
          timestamp = millis();
          while(millis()-timestamp<buffer5.K){
            temperature_control();
            on=millis(); //prevent termination from check_Button_terminate()
          }
       }
       dwell=false;
    break;
    case 6: //pause until interaction
       dwell=true;
       while(digitalRead(ENCODER_PIN)==HIGH && Serial.available()==0){
           temperature_control();
           on=millis(); //prevent termination from check_Button_terminate()
       }
       if(Serial.available()!=0){
           Serial.read();
       }
       dwell=false;
    break;
    case 7: //ABL start/stop
        ABL_ITERATIONS=buffer5.B;
        if(ABL){
          ABL=false;
          buffer4.command =-302; //ABL stop
        }else{
          ABL=true;
          buffer4.command =-300; //ABL start
        }
        Serial.write((char*)&buffer4,sizeof(buffer4));
        GM_command=false;
    break;
  }
}

void service_routine(){ //Timer interrupted service routine for pushing out the bytes
   if(set_temp==false && dwell==false){
      if(bufferstate==true && buffer1.byte_1[j]==255){
        check_command(0);
      }else if(bufferstate==false && buffer2.byte_2[j]==255){
        check_command(1);
      }
   }
   if(GM_command==false){
      push_bits();
   }else{
      homing_routine();
   }
}

void terminate_process(){
    if(PRINTING==true){
       initialization_var();
       Timer3.stop();
       if(PRINT_STATE==0){
          //digitalWrite(LED_BUILTIN,HIGH);
          buffer4.command = -260; //-260 terminate
          Serial.write((char*)&buffer4,sizeof(buffer4)); 
          Serial.readBytes((uint8_t *)&buffer1, sizeof(buffer1));
       }else{
          myFile.close();
       }
       memset(&buffer1, 0, sizeof(buffer1));
       memset(&buffer2, 0, sizeof(buffer2));
       lcd.clear();
       if(LCD_16x4==true){lcd.setCursor(0, 3);lcd.print("TERMINATED");lcd.blink();}
       delay(3000);
       lcd.clear();
       setup();  //locate again at startup
    } 
}

void check_USB_terminate(){
   if(bufferstate==true && buffer1.byte_1[j]==0){
      terminate_counter++;
      //setoff=true;
   }
   if(bufferstate==false && buffer2.byte_2[j]==0){
      terminate_counter++;
      //setoff=true;
   }
   if(terminate_counter>2000){
    setoff=true;
   }
}

void check_BUTTON_terminate(){
   if(digitalRead(ENCODER_PIN)==LOW){
      off=millis();
      signal_duration=off-on;}
   else{
      on=millis();
      signal_duration=0;
   } 
   if(signal_duration >= BUTTON_TIME_DURATION && PRINTING==true){ 
      setoff=true;
   } 
}

void read_serial(){
   if(readstate==true){
      buffer4.command = -253; 
      buffer4.nozz_temp = therm1.analog2temp(); // nozzle
      buffer4.bed_temp = therm2.analog2temp(); // bed
      if(bufferstate==false){ 
         memset(&buffer1, 0, sizeof(buffer1));
         Serial.write((char*)&buffer4,sizeof(buffer4));  
         Serial.readBytes((uint8_t *)&buffer1, sizeof(buffer1));
      }else{
         memset(&buffer2, 0, sizeof(buffer2));
         Serial.write((char*)&buffer4,sizeof(buffer4));  
         Serial.readBytes((uint8_t *)&buffer2, sizeof(buffer2));     
      }
      readstate=false;
   } 
}

void read_sd(){
   if(readstate==true){ 
      if(bufferstate==false){ 
         myFile.read(&buffer1,BUFFERSIZE);
      }else{
         myFile.read(&buffer2,BUFFERSIZE);    
      }
      readstate=false;
   } 
}

int SD_setup()
{
  volatile char savefile[12]="test000.bin";
  volatile int centi,mod_file_num,deca,mona,file_num=0;
  boolean sucess=true;
  
  if(LCD_16x4==true){lcd.setCursor(0, 0);lcd.print("MODE: SD CARD    ");lcd.noBlink();delay(1200);}
  if(LCD_16x4==true){lcd.setCursor(0, 1);lcd.print("INITIALIZING");lcd.blink();delay(1200);}
  sei();
  if (!SD.begin(SD_ENABLE)){
       sucess=false; 
  }
  //digitalWrite(LED_BUILTIN,HIGH);
  delay(1200);
  if(sucess==true){
      if(LCD_16x4==true){lcd.noBlink();lcd.setCursor(0, 1);lcd.print("SD DETECTED!");delay(1200);}
      if(LCD_16x4==true){lcd.setCursor(0, 2);lcd.print("SEARCH");lcd.blink();delay(1200);}
  }else{
       if(LCD_16x4==true){lcd.noBlink();lcd.setCursor(0, 1);lcd.print("SD ERROR!   ");delay(2000);lcd.clear();}
       return 1;
   }
   do {
      myFile = SD.open(savefile);
      file_num++;
      centi=file_num/100;
      mod_file_num=file_num%100;
      deca=mod_file_num/10;
      mona=mod_file_num%10;
      savefile[4]= centi +'0';
      savefile[5]= deca+'0';
      savefile[6]= mona+'0';            
   } while (!myFile && file_num<=999);
   if (myFile) {
      PRINT_STATE=1;
      return 0;
   }else{
      if(LCD_16x4==true){lcd.setCursor(0, 2);lcd.print("FILE ERROR");delay(2000);lcd.clear();}
      return 1;
   }
}

void get_SD_settings(){
   myFile.read(&buffer3,sizeof(buffer3));
   myFile.read(&buffer1,BUFFERSIZE);
   myFile.read(&buffer2,BUFFERSIZE);
   time_duration=1000/buffer3.CORE_FREQ; 
   bedtemp = buffer3.BED_TEMP; 
   p_bed=buffer3.P_BED/10.0;
   i_bed=buffer3.I_BED/10.0;
   d_bed=buffer3.D_BED/10.0;
   nozztemp = buffer3.NOZZLE_TEMP; 
   p_nozz=buffer3.P_NOZZ/10.0;
   i_nozz=buffer3.I_NOZZ/10.0;
   d_nozz=buffer3.D_NOZZ/10.0;
   pidbed.SetTunings(p_bed,i_bed,d_bed);
   pidnozz.SetTunings(p_nozz,i_nozz,d_nozz);
   if(buffer3.Wait_nozz){nozz_block=true;}else{ nozz_block=false;}
   if(buffer3.Wait_bed){bed_block=true;}else{bed_block=false;}
}

void get_USB_settings(){
   if(LCD_16x4==true){lcd.setCursor(0, 0);lcd.print("MODE: USB        ");lcd.noBlink();delay(1200);}
   if(LCD_16x4==true){lcd.setCursor(0, 1);lcd.print("CONNECTED!");delay(1200);}
   buffer4.command = -253; 
   Serial.write((char*)&buffer4,sizeof(buffer4));  
   Serial.readBytes((uint8_t *)&buffer1, sizeof(buffer1));
   Serial.write((char*)&buffer4,sizeof(buffer4));  
   Serial.readBytes((uint8_t *)&buffer2, sizeof(buffer2));
   for(cc=0;cc<sizeof(buffer3);cc++){
      buf[cc]=buffer1.byte_1[cc];
      j++;
   }
   memmove(&buffer3,buf,sizeof(buffer3));
   time_duration=1000/buffer3.CORE_FREQ;
   bedtemp = buffer3.BED_TEMP; 
   p_bed=buffer3.P_BED/10.0;
   i_bed=buffer3.I_BED/10.0;
   d_bed=buffer3.D_BED/10.0;
   nozztemp = buffer3.NOZZLE_TEMP; 
   p_nozz=buffer3.P_NOZZ/10.0;
   i_nozz=buffer3.I_NOZZ/10.0;
   d_nozz=buffer3.D_NOZZ/10.0;
   pidbed.SetTunings(p_bed,i_bed,d_bed);
   pidnozz.SetTunings(p_nozz,i_nozz,d_nozz);
   if(buffer3.Wait_nozz){nozz_block=true;}else{ nozz_block=false;}
   if(buffer3.Wait_bed){bed_block=true;}else{bed_block=false;}

}

void check_steppers(){
   if(buffer3.X_ENABLE==0){pinMode(buffer5.X_ENABLE_PIN,INPUT);}else{pinMode(buffer5.X_ENABLE_PIN,OUTPUT);}
   if(buffer3.Y_ENABLE==0){pinMode(buffer5.Y_ENABLE_PIN,INPUT);}else{pinMode(buffer5.Y_ENABLE_PIN,OUTPUT);}
   if(buffer3.Z_ENABLE==0){pinMode(buffer5.Z_ENABLE_PIN,INPUT);}else{pinMode(buffer5.Z_ENABLE_PIN,OUTPUT);}
   if(buffer3.E_ENABLE==0){pinMode(buffer5.E_ENABLE_PIN,INPUT);}else{pinMode(buffer5.E_ENABLE_PIN,OUTPUT);}
}

void temperature_control(){
   do{
      currentMillis = millis();
      if (currentMillis - previousMillis_USBupdate >= IDLE_USB_TEMP_UPDATE_INTERVAL) {
        previousMillis_USBupdate=currentMillis;
        if (PRINTING==0 || GM_command==true){                                          //update temperature from here only when not printing
           temperature_USB_update();
        }
      }
      if (AUTOTUNE==false){
         if (buffer3.THERMOSTAT_bed==1){
             if(currentMillis - previousMillis_Bed >= buffer3.CYCLE_BED){
                 previousMillis_Bed = currentMillis;
                 temp2 = therm2.analog2temp(); // bed
                 if((temp2<MIN_TEMP_SAFETY || temp2>MAX_TEMP_BED) && buffer3.HEATED_BED==1){digitalWrite(buffer5.B_HEATER_PIN,LOW);digitalWrite(buffer5.N_HEATER_PIN,LOW);setoff=true;}
                 if(temp2<=buffer3.BED_TEMP && buffer3.HEATED_BED==1){
                     digitalWrite(buffer5.B_HEATER_PIN,HIGH);
                 }else{
                     digitalWrite(buffer5.B_HEATER_PIN,LOW);
                     bed_block=false;
                 }
             }
         }else if(buffer3.THERMOSTAT_bed==0){
             if (currentMillis - previousMillis_Bed >=buffer3.CYCLE_BED && buffer3.HEATED_BED==1){
                 previousMillis_Bed = currentMillis;
                 temp2 = therm2.analog2temp();
                 pidbed.Compute();
                 digitalWrite(buffer5.B_HEATER_PIN,bedpwm);
                 if(temp2>buffer3.BED_TEMP){
                   bed_block=false;
                 }
             }else if(buffer3.HEATED_BED==0){
                  //digitalWrite(LED_BUILTIN,LOW);
                  digitalWrite(buffer5.B_HEATER_PIN,LOW);
                  bed_block=false;
              }
         }
         if (buffer3.THERMOSTAT_nozz==1){
           if (currentMillis - previousMillis_Nozz >= buffer3.CYCLE_NOZZ){
              previousMillis_Nozz = currentMillis;
              temp1 = therm1.analog2temp(); // nozzle
              if((temp1<MIN_TEMP_SAFETY || temp1>MAX_TEMP_NOZZLE) && buffer3.HEATED_NOZZLE==1){digitalWrite(buffer5.B_HEATER_PIN,LOW);digitalWrite(buffer5.B_HEATER_PIN,LOW);setoff=true;}
              if(temp1<=buffer3.NOZZLE_TEMP && buffer3.HEATED_NOZZLE==1){
               if(bed_block==false){
                 digitalWrite(buffer5.N_HEATER_PIN,HIGH);
                 }else{
                   digitalWrite(buffer5.N_HEATER_PIN,LOW);
                   }
              }else{
                digitalWrite(buffer5.N_HEATER_PIN,LOW); 
                nozz_block=false;}
           }
         }else if(buffer3.THERMOSTAT_nozz==0){
             if (currentMillis - previousMillis_Nozz >=buffer3.CYCLE_NOZZ && buffer3.HEATED_NOZZLE==1){
                 //digitalWrite(LED_BUILTIN,HIGH);
                 previousMillis_Nozz = currentMillis;
                 if(bed_block==false){
                    temp1 = therm1.analog2temp(); // nozzle
                    pidnozz.Compute();
                    digitalWrite(buffer5.N_HEATER_PIN,nozzpwm);
                 }else{
                    digitalWrite(buffer5.N_HEATER_PIN,LOW);
                 }
                 if(temp1>buffer3.NOZZLE_TEMP){
                   nozz_block=false;
                 }
             }else if(buffer3.HEATED_NOZZLE==0 && (bed_block==false && (buffer3.HEATED_BED==1 || buffer3.HEATED_BED==0))){
                digitalWrite(buffer5.N_HEATER_PIN,LOW); 
                nozz_block=false;
             }
         }
      }
      if((digitalRead(ENCODER_PIN)==LOW) && ((nozz_block==1 && buffer3.Wait_nozz==1) || (bed_block==1 && buffer3.Wait_bed==1))){setoff=true;}
   }while(((nozz_block==1 && buffer3.Wait_nozz==1) || (bed_block==1 && buffer3.Wait_bed==1)) && setoff==false);
   if(setoff==true){
    digitalWrite(buffer5.N_HEATER_PIN,LOW);
    digitalWrite(buffer5.B_HEATER_PIN,LOW);
   }
   if(set_temp==true){
      GM_command=false;
      set_temp=false;
      //digitalWrite(LED_BUILTIN,LOW);
   }
}

void temperature_USB_update(){
  buffer4.nozz_temp = therm1.analog2temp(); // nozzle
  buffer4.bed_temp = therm2.analog2temp();  // bed
  buffer4.command = -243;
  buffer4.x_pos=XPOS;
  buffer4.y_pos=YPOS;
  buffer4.z_pos=ZPOS;
  Serial.write((char*)&buffer4,sizeof(buffer4));
}


int check_inputs(){
  
  ////////////////////**************SD****************////////////////////
   while(signal_duration < BUTTON_TIME_DURATION && !Serial.available()){
      if(digitalRead(ENCODER_PIN)==LOW){
         off=millis();
         signal_duration=off-on;
      }else{
         on=millis();
         signal_duration=0;
      }
   }
   if(signal_duration >= BUTTON_TIME_DURATION && !Serial.available()){
      signal_duration=0;
      return SD_setup();
   }
   
////////////////////////////***********USB***************/////////////////////////////
   if(Serial.available() && signal_duration < BUTTON_TIME_DURATION){
       Serial.readBytes((uint8_t *)&buffer6, sizeof(buffer6));
       while(buffer6.test !=5){
        Serial.readBytes((uint8_t *)&buffer6, sizeof(buffer6));
       }
       Serial.write((uint8_t*)&pass,sizeof(pass));
       Serial.readBytes((uint8_t *)&buffer5, sizeof(buffer5));
       Serial.write((uint8_t*)&pass,sizeof(pass));
       if(buffer5.A==0){                             ///A=0 => idle mode
           configure_idle_pin_modes();
           if(buffer5.B == 0){                       ///B=0 => temp command
              if(buffer5.C == 0){                    ///C=0 => set noz temp
                bed_block=false;
                buffer3.THERMOSTAT_nozz=buffer5.E;
                buffer3.HEATED_NOZZLE=buffer5.G;
                buffer3.NOZZLE_TEMP=buffer5.I;
                nozztemp=buffer3.NOZZLE_TEMP;
                buffer3.CYCLE_NOZZ=buffer5.K;
                buffer3.THERMISTOR_TYPE_NOZZLE=buffer5.M;
                buffer3.Wait_nozz=0;
                buffer3.Wait_bed=0;
                p_nozz=buffer5.O/10.0;
                i_nozz=buffer5.P/10.0;
                d_nozz=buffer5.Q/10.0;
                pidnozz.SetTunings(p_nozz,i_nozz,d_nozz);
              }else if(buffer5.C == 1){                                ///C=1 => set bed temp                       
                buffer3.THERMOSTAT_bed=buffer5.E;
                buffer3.HEATED_BED=buffer5.G;
                buffer3.BED_TEMP=buffer5.I;
                bedtemp = buffer3.BED_TEMP;
                buffer3.CYCLE_BED=buffer5.K;
                buffer3.THERMISTOR_TYPE_BED=buffer5.M;
                buffer3.Wait_nozz=0;
                buffer3.Wait_bed=0;
                p_bed=buffer5.O/10.0;
                i_bed=buffer5.P/10.0;
                d_bed=buffer5.Q/10.0;
                pidbed.SetTunings(p_bed,i_bed,d_bed);
               }
            }else if(buffer5.B == 1){              ///B=1 Enable/Disable motor command
                if(buffer5.C==0){                  //C=0 => XAXIS
                  if(buffer5.D==0){                //D=0 => Disable
                    buffer3.X_ENABLE=0;
                  }else{                           //D=1 => Enable
                    buffer3.X_ENABLE=1;
                  }
                }
                if(buffer5.C==1){                  //C=1 => YAXIS
                  if(buffer5.D==0){                //D=0 => Disable
                    buffer3.Y_ENABLE=0;
                  }else{                           //D=1 => Enable
                    buffer3.Y_ENABLE=1;
                  }
                }
                if(buffer5.C==2){                  //C=2 => ZAXIS
                  if(buffer5.D==0){                //D=0 => Disable
                    buffer3.Z_ENABLE=0;
                  }else{                           //D=1 => Enable
                    buffer3.Z_ENABLE=1;
                  }
                }
                if(buffer5.C==3){                  //C=3 => EAXIS
                  if(buffer5.D==0){                //D=0 => Disable
                    buffer3.E_ENABLE=0;
                  }else{                           //D=1 => Enable
                    buffer3.E_ENABLE=1;
                  }
                }
              check_steppers();
            }else if(buffer5.B == 2){              //B=2 => Home
               buffer3.HOME_X_ENABLE=buffer5.C;
               buffer3.HOME_Y_ENABLE=buffer5.D;
               buffer3.HOME_Z_ENABLE=buffer5.E;
               buffer3.HOME_X_STATE=buffer5.F;
               buffer3.HOME_Y_STATE=buffer5.G;
               buffer3.HOME_Z_STATE=buffer5.H;
               buffer3.HOME_X_DIR=buffer5.I;
               buffer3.HOME_Y_DIR=buffer5.J;
               buffer3.HOME_Z_DIR=buffer5.K;
               buffer3.HOME_X_DURATION=buffer5.O;
               buffer3.HOME_Y_DURATION=buffer5.P;
               buffer3.HOME_Z_DURATION=buffer5.Q;
               homing_routine();
            }else if(buffer5.B == 3){              //B=3 => Rapid potision 
                    rapid_position();
            }else if(buffer5.B == 4){
                   analogWrite(buffer5.FAN_PIN,buffer5.J);
            }else if(buffer5.B == 5){
                   Atune_loop(buffer5.C);
            }else if(buffer5.B == 7){ //BLtouch
              if(buffer5.C==0){ //test
                 bltouch.write(160);
                 delay(300);
                 bltouch.write(120);
              }else if(buffer5.C==1){ //deploy
                 bltouch.write(90);
              }else if(buffer5.C==2){ //stow
                 bltouch.write(10);
              }
              
            }
            while(Serial.available()<3){
                 temperature_control();  
            }
        }else if (buffer5.A==1){
            bed_block=true;
            PRINT_STATE=0;
            return 0;
       }
   }
    return 1;
}
void Atune_loop(uint8_t tune_case){
  boolean repeat=true,last_power=false,power=true,get_extreme=false,firsttime=true;
  double iterations=0,last_tune_temp=0,max_tune_temp=0,min_tune_temp=0;
  unsigned long  period=0,prev_time=0;
  unsigned long max_temp_counter=0,min_temp_counter=0,sum_period=0;
  double target_tune_temp, tune_temp, p_tune, i_tune, d_tune;
  int heater;
  AUTOTUNE=true;
  if(tune_case==1){ //Nozzle Autotune
      target_tune_temp = NOZZLE_TUNE_TEMP;
      heater = buffer5.N_HEATER_PIN;
  }else{  //Bed Autotune
      target_tune_temp = BED_TUNE_TEMP;
      heater = buffer5.B_HEATER_PIN;
  }
  while(repeat==true){
    if(tune_case==1){tune_temp = therm1.analog2temp();}else{tune_temp = therm2.analog2temp();}
    if(tune_temp<target_tune_temp && last_power==false){
      iterations=iterations+0.5;
      power=true;
      digitalWrite(heater,HIGH);
    }
    if(tune_temp>target_tune_temp && last_power==true){
      iterations=iterations+0.5;
      power=false;
      digitalWrite(heater,LOW);
    }
    if(firsttime==true){
      last_power=true;
      firsttime=false;
    }
    if(last_power!=power){
      while(get_extreme==false){
        delay(300); //need better sollution in the future
        if(tune_case==1){tune_temp = therm1.analog2temp();}else{tune_temp = therm2.analog2temp();}
        if(last_power==true){
          if(last_tune_temp>tune_temp){
            period=millis()-prev_time;
            prev_time=millis();
            get_extreme=true;
            if(iterations>2){ //avoid overshoot at the begining
                sum_period=sum_period+period;
                max_tune_temp=max_tune_temp+last_tune_temp;
                max_temp_counter++;
            }
          }
        }else{
          if(tune_temp>last_tune_temp){
            get_extreme=true;
            if(iterations>2){ //avoid overshoot at the begining
               min_tune_temp=min_tune_temp+last_tune_temp;
               min_temp_counter++;
            }
          }          
        }
        last_tune_temp=tune_temp;
        temperature_control();
      }
      get_extreme=false;
    }
    last_power=power;
    last_tune_temp=tune_temp;
    if(iterations>5){
      max_tune_temp=max_tune_temp/max_temp_counter;
      period=(sum_period/max_temp_counter)/1000;
      min_tune_temp=min_tune_temp/min_temp_counter;
      double b = max_tune_temp-min_tune_temp;
      double Pu = (4.0*255)/(pi*b);
      p_tune=0.6*Pu;
      double Ti=0.5*period;
      double Td=0.125*period;
      i_tune=p_tune/Ti;
      d_tune=p_tune*Td;
      repeat=false;
      buffer4.command=-200; //Autotune command
      buffer4.nozz_temp=p_tune; //in this case nozz_temp,bed_temp is just a place to send the data (PID)
      buffer4.bed_temp=i_tune;
      Serial.write((char*)&buffer4,sizeof(buffer4));
      buffer4.command=-200;
      buffer4.nozz_temp=d_tune;
      buffer4.bed_temp=i_tune;
      Serial.write((char*)&buffer4,sizeof(buffer4));
      buffer4.command=-253;
    }
    temperature_control();
  }
  AUTOTUNE=false;
}

void rapid_position(){
   digitalWrite(buffer5.B_HEATER_PIN,LOW);    //Disable Heaters while rapid positioning for safety
   digitalWrite(buffer5.N_HEATER_PIN,LOW);   //Disable Heaters while rapid positioning for safety
   while(step_counter<buffer5.J && buffer5.C==0 && buffer5.D==1 && setoff==false && Serial.available()==0){
      digitalWrite(buffer5.X_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.X_STEP_PIN,HIGH);
      delayMicroseconds(buffer5.I);
      digitalWrite(buffer5.X_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.X_STEP_PIN,LOW);
      delayMicroseconds(buffer5.I);
      if(buffer5.E){
        XPOS=XPOS+1;
      }else{
        XPOS=XPOS-1;
      }
      step_counter++;
      //if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
   }
   while(step_counter<buffer5.J && buffer5.C==1 && buffer5.D==1 && setoff==false && Serial.available()==0){
      digitalWrite(buffer5.Y_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.Y_STEP_PIN,HIGH);
      delayMicroseconds(buffer5.I);
      digitalWrite(buffer5.Y_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.Y_STEP_PIN,LOW);
      delayMicroseconds(buffer5.I);
      if(buffer5.E){
        YPOS=YPOS+1;
      }else{
        YPOS=YPOS-1;
      }
      step_counter++;
      //if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
   }
   while(step_counter<buffer5.J && buffer5.C==2 && buffer5.D==1 && setoff==false && Serial.available()==0){
      digitalWrite(buffer5.Z_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.Z_STEP_PIN,HIGH);
      delayMicroseconds(buffer5.I);
      digitalWrite(buffer5.Z_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.Z_STEP_PIN,LOW);
      delayMicroseconds(buffer5.I);
      step_counter++;
      if(buffer5.E){
        ZPOS=ZPOS+1;
      }else{
        ZPOS=ZPOS-1;
      }
      //if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
   }
   while(step_counter<buffer5.J && buffer5.C==3 && buffer5.D==1 && setoff==false && Serial.available()==0){
      digitalWrite(buffer5.E_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.E_STEP_PIN,HIGH);
      delayMicroseconds(buffer5.I);
      digitalWrite(buffer5.E_DIR_PIN,buffer5.E);
      digitalWrite(buffer5.E_STEP_PIN,LOW);
      delayMicroseconds(buffer5.I);
      step_counter++;
      //if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
   }
   if(Serial.available()!=0){
     Serial.read();
   }
   step_counter=0;
}

void homing_routine(){ 
    if(homing==true && delay_homing==false){
        //step_counter=0;
        if(ABL){
            time_counter++;
            if (buffer3.HOME_Z_ENABLE==true && buffer5.T==1 && time_counter>=15 && elevation==false){
                ABL_Z=ABL_Z+1;
                time_counter=0;
                if (signalstate){
                  digitalWrite(buffer5.Z_DIR_PIN,buffer3.HOME_Z_DIR);
                  digitalWrite(buffer5.Z_STEP_PIN,HIGH);
                }else{
                  digitalWrite(buffer5.Z_DIR_PIN,buffer3.HOME_Z_DIR);
                  digitalWrite(buffer5.Z_STEP_PIN,LOW);
                }
                signalstate=!signalstate;
                
                ZMIN_READ=digitalRead(buffer5.Z_ENDSTOP_PIN);
                
                if(ZMIN_READ!=buffer3.HOME_Z_STATE){
                  iter++;
                  elevation=true;
                  probe_up=true;
                  time_counter=0;
                  z_tracking = ABL_Z;
                  if(buffer3.HOME_Z_DIR==0){Z_revdir=1;}else{Z_revdir=0;}
                }
            }
            
            if(buffer3.HOME_Z_ENABLE==true && buffer5.T==1 && time_counter>=15 && elevation==true && ABL==true){
                ABL_Z=ABL_Z-1;
                time_counter=0;
                if (signalstate){
                  digitalWrite(buffer5.Z_DIR_PIN,Z_revdir);
                  digitalWrite(buffer5.Z_STEP_PIN,HIGH);
                }else{
                  digitalWrite(buffer5.Z_DIR_PIN,Z_revdir);
                  digitalWrite(buffer5.Z_STEP_PIN,LOW);
                }
                signalstate=!signalstate;          
                if(ABL_Z==0){
                  elevation=false;
                  ABL_Z=0;
                    if(iter<ABL_ITERATIONS){
                        deploy_probe=true;
                    }else{
                        save_probe=true;
                        homing=false;
                        probe_up=true;
                        iter=0;
                    }
                }
            } 
        }else{
            time_counter++;
            if(setoff==false){
                if (buffer3.HOME_X_ENABLE==true && buffer5.R==1 && time_counter>=15 && homeX_done==false && execute_offsetX==false){
                  time_counter=0;
                  
                  if (signalstate){
                      digitalWrite(buffer5.X_DIR_PIN,buffer3.HOME_X_DIR);
                      digitalWrite(buffer5.X_STEP_PIN,HIGH);
                    }else{
                      digitalWrite(buffer5.X_DIR_PIN,buffer3.HOME_X_DIR);
                      digitalWrite(buffer5.X_STEP_PIN,LOW);
                  }
                  signalstate=!signalstate;
                  XMIN_READ=digitalRead(buffer5.X_ENDSTOP_PIN);
                  if(XMIN_READ!=buffer3.HOME_X_STATE){
                     XMIN_READ=buffer3.HOME_X_STATE;
                     execute_offsetX=true;
                     step_counter=0;
                     if(buffer5.U>0){buffer3.HOME_X_DIR=!buffer3.HOME_X_DIR;}else{buffer5.U=-buffer5.U;}
                  }
                }else if(buffer5.R==0){
                  homeX_done=true;
                }
              /*
              if (execute_offsetX==true){
                    if(time_counter>=15){
                      time_counter=0;
                    if (signalstate){
                      digitalWrite(buffer5.X_DIR_PIN,buffer3.HOME_X_DIR);
                      digitalWrite(buffer5.X_STEP_PIN,HIGH);
                    }else{
                      digitalWrite(buffer5.X_DIR_PIN,buffer3.HOME_X_DIR);
                      digitalWrite(buffer5.X_STEP_PIN,LOW);
                    }
                    signalstate=!signalstate;
                    step_counter++;
                    }
                  if(step_counter>=buffer5.U){
                      execute_offsetX=false;
                      homeX_done=true;
                      step_counter=0;
                      time_counter=0;
                  }
                  
              }
                if (buffer3.HOME_Y_ENABLE==true && buffer5.S==1 && time_counter>=15 && homeY_done==false && execute_offsetY==false && homeX_done==true){
                  time_counter=0;
                  if (signalstate){
                      digitalWrite(buffer5.Y_DIR_PIN,buffer3.HOME_Y_DIR);
                      digitalWrite(buffer5.Y_STEP_PIN,HIGH);
                    }else{
                      digitalWrite(buffer5.Y_DIR_PIN,buffer3.HOME_Y_DIR);
                      digitalWrite(buffer5.Y_STEP_PIN,LOW);
                    }
                    signalstate=!signalstate;
                  YMIN_READ=digitalRead(buffer5.Y_ENDSTOP_PIN);
                  if(YMIN_READ!=buffer3.HOME_Y_STATE){
                     YMIN_READ=buffer3.HOME_Y_STATE;
                     execute_offsetY=true;
                     step_counter=0;
                     if(buffer5.V>0){buffer3.HOME_Y_DIR=!buffer3.HOME_Y_DIR;}else{buffer5.V=-buffer5.V;}
                  }
                }else if(buffer5.S==0){
                  //digitalWrite(LED_BUILTIN,HIGH);
                  homeY_done=true;
                }
              if (execute_offsetY==true){
                    if(time_counter>=15){
                      time_counter=0;
                    if (signalstate){
                      digitalWrite(buffer5.Y_DIR_PIN,buffer3.HOME_Y_DIR);
                      digitalWrite(buffer5.Y_STEP_PIN,HIGH);
                    }else{
                      digitalWrite(buffer5.Y_DIR_PIN,buffer3.HOME_Y_DIR);
                      digitalWrite(buffer5.Y_STEP_PIN,LOW);
                    }
                    signalstate=!signalstate;
                    step_counter++;
                    }
                  if(step_counter>=buffer5.V){
                      execute_offsetY=false;
                      homeY_done=true;
                      step_counter=0;
                      time_counter=0;
                  }
              }
              if (buffer3.HOME_Z_ENABLE==true && buffer5.T==1 && time_counter>=15 && homeX_done==true && homeY_done==true){
                  time_counter=0;
                  if (signalstate){
                      digitalWrite(buffer5.Z_DIR_PIN,buffer3.HOME_Z_DIR);
                      digitalWrite(buffer5.Z_STEP_PIN,HIGH);
                    }else{
                      digitalWrite(buffer5.Z_DIR_PIN,buffer3.HOME_Z_DIR);
                      digitalWrite(buffer5.Z_STEP_PIN,LOW);
                      ABL_Z=ABL_Z+1;
                    }
                    signalstate=!signalstate;
                }
                ZMIN_READ=digitalRead(buffer5.Z_ENDSTOP_PIN);
               // if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
                if(ZMIN_READ!=buffer3.HOME_Z_STATE){
                  digitalWrite(LED_BUILTIN,HIGH);
                  probe_up=true;
                  GM_command=false;
                  ZMIN_READ=buffer3.HOME_Z_STATE;
                  homing=false;
                }
                */
            }
        }
    }
}
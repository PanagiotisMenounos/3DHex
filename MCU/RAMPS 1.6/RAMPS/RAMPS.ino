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

#include <SPI.h>
#include "SdFat.h"
#include <TimerThree.h>
#include <thermistor.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <Servo.h>

SdFat SD;
File myFile;

Servo bltouch;

#define SERIAL_BAUD_RATE 400000
#define BUTTON_TIME_DURATION 1000
#define LCD_16x4 true
#define ENCODER_PIN 35
#define SD_ENABLE 53
#define MIN_TEMP_SAFETY 5
#define MAX_TEMP_NOZZLE 250
#define MAX_TEMP_BED 80
#define HOME_XMIN_PIN 3
#define HOME_YMIN_PIN 14
#define HOME_ZMIN_PIN 18
#define BLTOUCH_PIN 4
#define USB_SETTING_BYTES 52
#define MG_BYTES 36
#define BUFFERSIZE 3100
#define X_EN 38
#define Y_EN A2
#define Z_EN A8
#define E_EN 24 
#define X_STEP A0        //PF0
#define Y_STEP A6        //PF6
#define Z_STEP 46        //PL3
#define E_STEP 26        //PA4
#define X_DIR A1         //PF1
#define Y_DIR A7         //PF7
#define Z_DIR 48         //PL1
#define E_DIR 28         //PA6
#define NOZZ_THRMSTR A13
#define BED_THRMSTR A14
#define NOZZ_HEATER 10
#define FAN 9
#define BED_HEATER 8
#define IDLE_USB_TEMP_UPDATE_INTERVAL 1000
#define NOZZLE_TUNE_TEMP 80
#define BED_TUNE_TEMP 50
#define pi 3.14159265358979323846

void service_routine();
void temperature_control();
void temperature_USB_update();
void get_USB_settings();
void get_SD_settings();
void check_steppers();
void read_serial();
void read_sd();
int SD_setup();
int check_inputs();
void check_USB_terminate();
void check_BUTTON_terminate();
void terminate_process();
void homing_routine();
void initialzation_vars();
void check_buffer();
void check_command(int state_c);
void read_GM_data();
void Atune_loop(uint8_t tune_case);
void position_report();

struct data1 { 
   volatile byte byte_1[BUFFERSIZE];
};

struct data2 { 
   volatile byte byte_2[BUFFERSIZE];
};

struct data3{ 
   volatile uint16_t CORE_FREQ;
   volatile uint8_t X_ENABLE;
   volatile uint8_t Y_ENABLE;
   volatile uint8_t Z_ENABLE;
   volatile uint8_t E_ENABLE;
   volatile uint16_t NOZZLE_TEMP;
   volatile uint16_t BED_TEMP;
   volatile uint16_t CYCLE_NOZZ;
   volatile uint16_t CYCLE_BED;
   volatile uint16_t P_NOZZ;
   volatile uint16_t I_NOZZ;
   volatile uint16_t D_NOZZ;
   volatile uint16_t P_BED;
   volatile uint16_t I_BED;
   volatile uint16_t D_BED;
   volatile uint8_t Wait_nozz;
   volatile uint8_t Wait_bed;
   volatile int8_t THERMISTOR_TYPE_NOZZLE;
   volatile int8_t THERMISTOR_TYPE_BED;
   volatile uint8_t HEATED_NOZZLE;
   volatile uint8_t HEATED_BED;
   volatile uint8_t THERMOSTAT_nozz;
   volatile uint8_t THERMOSTAT_bed;
   volatile uint8_t DIFFERENTIAL_NOZZ;
   volatile uint8_t DIFFERENTIAL_BED;
   volatile uint16_t HOME_X_DURATION;
   volatile uint16_t HOME_Y_DURATION;
   volatile uint16_t HOME_Z_DURATION;
   volatile uint8_t HOME_X_ENABLE;
   volatile uint8_t HOME_Y_ENABLE;
   volatile uint8_t HOME_Z_ENABLE;
   volatile uint8_t HOME_X_DIR;
   volatile uint8_t HOME_Y_DIR;
   volatile uint8_t HOME_Z_DIR;
   volatile uint8_t HOME_X_STATE;
   volatile uint8_t HOME_Y_STATE;
   volatile uint8_t HOME_Z_STATE_MIRROR;
   volatile uint8_t HOME_Z_STATE;//MUST BE WRITTEN TWICE
};

struct data4{
  volatile double command;
  volatile double nozz_temp;
  volatile double bed_temp;
  volatile unsigned int x_pos;
  volatile unsigned int y_pos;
  volatile unsigned int z_pos;
};

struct data5{
  volatile uint8_t A;
  volatile uint8_t B;
  volatile uint8_t C;
  volatile uint8_t D;
  volatile uint8_t E;
  volatile uint8_t F;
  volatile uint8_t G;
  volatile uint8_t H;
  volatile uint16_t I;
  volatile uint16_t J;
  volatile uint16_t K;
  volatile uint16_t L;
  volatile uint8_t M;
  volatile uint8_t N;
  volatile uint16_t O;
  volatile uint16_t P;
  volatile uint16_t Q;
  volatile uint16_t R;
  volatile uint16_t S;
  volatile uint16_t T;
  volatile int16_t U;
  volatile int16_t V;
  volatile int16_t W;
};

volatile struct data1 buffer1;
volatile struct data2 buffer2;
volatile struct data3 buffer3;
volatile struct data4 buffer4;
volatile struct data5 buffer5;


thermistor therm1(NOZZ_THRMSTR,buffer3.THERMISTOR_TYPE_NOZZLE);// nozzle
thermistor therm2(BED_THRMSTR,buffer3.THERMISTOR_TYPE_BED);  // bed

double temp1,temp2,nozzpwm,nozztemp,p_nozz,i_nozz,d_nozz,bedpwm,bedtemp,p_bed,i_bed,d_bed;
volatile boolean bufferstate=true,readstate=false,setoff=false,nozz_block=true,bed_block=true,once=false,PRINTING=false,GM_command=false,GM_end=false,print_history=false;
volatile unsigned int i=0,j=0,cc,step_counter=0;
volatile byte buf[USB_SETTING_BYTES],PRINT_STATE,MG_buf[MG_BYTES];
volatile float time_duration;
volatile unsigned long terminate_counter=0,currentMillis=0,previousMillis_Nozz = 0,previousMillis_Bed = 0,signal_duration=0,off=0,on=0,previousMillis_USBupdate=0,usboff=0,usb_duration=0,usbon=0,last_usb_wait=0,usb_wait=0;
volatile int XMIN_READ,YMIN_READ,ZMIN_READ,set_counter=0;
volatile uint8_t pass=1,fail=0,command_value=255;
volatile int8_t p;
double input, output, setpoint=0;
int state_r,ABL_ITERATIONS;
volatile boolean AUTOTUNE=false,ABL=false;
volatile unsigned int XPOS=0,YPOS=0,ZPOS=0;
volatile unsigned long currentMillis_pos=0,previousMillis_USBupdate_pos=0;


LiquidCrystal lcd(16, 17, 23, 25, 27, 29);

PID pidnozz(&temp1, &nozzpwm, &nozztemp,p_nozz,i_nozz,d_nozz, DIRECT);
PID pidbed(&temp2, &bedpwm, &bedtemp,p_bed,i_bed,d_bed, DIRECT);

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
   nozz_block=false;bed_block=false;once=false;GM_command=false;GM_end=false;
   terminate_counter=0;
}

void setup() {
   bltouch.attach(BLTOUCH_PIN);
   pinMode(ENCODER_PIN,INPUT_PULLUP);
   pinMode(SD_ENABLE,OUTPUT);
   if(print_history==false){
   pinMode(X_EN,OUTPUT);
   pinMode(Y_EN,OUTPUT);
   pinMode(Z_EN,OUTPUT);
   pinMode(E_EN,OUTPUT); 
   }
   pinMode(X_STEP,OUTPUT);
   pinMode(Y_STEP,OUTPUT);
   pinMode(Z_STEP,OUTPUT);
   pinMode(E_STEP,OUTPUT);
   pinMode(X_DIR,OUTPUT);
   pinMode(Y_DIR,OUTPUT);
   pinMode(Z_DIR,OUTPUT);
   pinMode(E_DIR,OUTPUT);
   pinMode(NOZZ_THRMSTR,INPUT);
   pinMode(BED_THRMSTR,INPUT);
   pinMode(NOZZ_HEATER,OUTPUT);
   pinMode(BED_HEATER,OUTPUT);
   pinMode(FAN,OUTPUT);
   pinMode(HOME_XMIN_PIN,INPUT);
   pinMode(HOME_YMIN_PIN,INPUT);
   pinMode(HOME_ZMIN_PIN,INPUT);
   pinMode(SD_ENABLE,OUTPUT);
   digitalWrite(SD_ENABLE,HIGH);
   digitalWrite(SD_ENABLE,HIGH);
   digitalWrite(BED_HEATER,LOW);
   digitalWrite(NOZZ_HEATER,LOW);
   //digitalWrite(FAN,HIGH);
   print_history=true;
   Serial.begin(SERIAL_BAUD_RATE);
   pinMode(LED_BUILTIN,OUTPUT);
   pidbed.SetMode(AUTOMATIC);
   pidbed.SetSampleTime(0);
   pidnozz.SetMode(AUTOMATIC);
   pidnozz.SetSampleTime(0);
   initialization_var();
   if(LCD_16x4==true){lcd.begin(20,4);delay(100);}
   delay(100); 
   bltouch.write(90);
   do{// blocking if no input
      if(LCD_16x4==true){lcd.setCursor(0, 0);lcd.print("MODE: CHECK INPUT");lcd.blink();}    
   }while(check_inputs());
   PRINTING=true;
   if(PRINT_STATE==0){get_USB_settings();}else{get_SD_settings();}
   if(LCD_16x4==true){lcd.noBlink();}
   check_steppers();
   //homing_routine();
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
     if(PRINT_STATE==0){read_serial();}else{read_sd();}
     if(GM_command==true){read_GM_data();temperature_control();position_report();GM_command=false;}
     temperature_control();
     position_report();
     check_BUTTON_terminate();
     check_USB_terminate();
  }else{
     position_report(); 
     terminate_process();
  }
}

void service_routine(){ //Timer interrupted service routine for pushing out the bytes
   if(bufferstate==true && buffer1.byte_1[j]==255){
     check_command(0);
   }else if(bufferstate==false && buffer2.byte_2[j]==255){
     check_command(1);
   }
   if(GM_command==false){
   cli();
   i++;
   if(bufferstate==true && i==buffer1.byte_1[j] && buffer1.byte_1[j]!=0){
      j++;
      PORTF = (bitRead(buffer1.byte_1[j],0)<<PF1)|(bitRead(buffer1.byte_1[j],2)<<PF7);
      PORTF = (bitRead(buffer1.byte_1[j],0)<<PF1)|(bitRead(buffer1.byte_1[j],1)<<PF0)|(bitRead(buffer1.byte_1[j],2)<<PF7)|(bitRead(buffer1.byte_1[j],3)<<PF6);
      PORTL = (bitRead(buffer1.byte_1[j],4)<<PL1);
      PORTL = (bitRead(buffer1.byte_1[j],4)<<PL1)|(bitRead(buffer1.byte_1[j],5)<<PL3);
      PORTA = (bitRead(buffer1.byte_1[j],6)<<PA6);
      PORTA = (bitRead(buffer1.byte_1[j],6)<<PA6)|(bitRead(buffer1.byte_1[j],7)<<PA4);
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
      //check_command(0);
   }else if(bufferstate==false && i==buffer2.byte_2[j] && buffer2.byte_2[j]!=0){
      j++;
      PORTF = (bitRead(buffer2.byte_2[j],0)<<PF1)|(bitRead(buffer2.byte_2[j],2)<<PF7);
      PORTF = (bitRead(buffer2.byte_2[j],0)<<PF1)|(bitRead(buffer2.byte_2[j],1)<<PF0)|(bitRead(buffer2.byte_2[j],2)<<PF7)|(bitRead(buffer2.byte_2[j],3)<<PF6);
      PORTL = (bitRead(buffer2.byte_2[j],4)<<PL1);
      PORTL = (bitRead(buffer2.byte_2[j],4)<<PL1)|(bitRead(buffer2.byte_2[j],5)<<PL3);
      PORTA = (bitRead(buffer2.byte_2[j],6)<<PA6);
      PORTA = (bitRead(buffer2.byte_2[j],6)<<PA6)|(bitRead(buffer2.byte_2[j],7)<<PA4);
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
      //check_command(1);
   }else{
      PORTF = (0<<PF0)|(0<<PF6);
      PORTL = (0<<PL3);
      PORTA = (0<<PA4);
   }
   check_buffer();
   sei();
   }
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
       GM_end=false;
       state_r=0;
       i=0;
  }
  if(buffer2.byte_2[j]==command_value && state_c==1){
    GM_command=true;
    GM_end=false;
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
    for(cc=0;cc<MG_BYTES;cc++){//total bytes of data
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
       bed_block=true;
       buffer3.BED_TEMP=buffer5.J;
       bedtemp = buffer3.BED_TEMP;
       buffer3.Wait_bed=buffer5.B;
    break;
    case 1: //set hotend temp
       nozz_block=true;
       buffer3.NOZZLE_TEMP=buffer5.J;
       nozztemp = buffer3.NOZZLE_TEMP;
       buffer3.Wait_nozz=buffer5.B;
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
       homing_routine();
    break;
    case 4: //fan control
       analogWrite(FAN,buffer5.J);
    break;
    case 5: //pause for time
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
    break;
    case 6: //pause until interaction
       while(digitalRead(ENCODER_PIN)==HIGH && Serial.available()==0){
           temperature_control();
           on=millis(); //prevent termination from check_Button_terminate()
       }
       if(Serial.available()!=0){
           Serial.read();
       }
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
    break;
  }
}

void terminate_process(){
    if(PRINTING==true){
       initialization_var();
       Timer3.stop();
       if(PRINT_STATE==0){
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
   for(cc=0;cc<USB_SETTING_BYTES;cc++){//total 52 bytes
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
   if(buffer3.X_ENABLE==0){pinMode(X_EN,INPUT);}else{pinMode(X_EN,OUTPUT);}
   if(buffer3.Y_ENABLE==0){pinMode(Y_EN,INPUT);}else{pinMode(Y_EN,OUTPUT);}
   if(buffer3.Z_ENABLE==0){pinMode(Z_EN,INPUT);}else{pinMode(Z_EN,OUTPUT);}
   if(buffer3.E_ENABLE==0){pinMode(E_EN,INPUT);}else{pinMode(E_EN,OUTPUT);}
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
                 if((temp2<MIN_TEMP_SAFETY || temp2>MAX_TEMP_BED) && buffer3.HEATED_BED==1){digitalWrite(BED_HEATER,LOW);digitalWrite(NOZZ_HEATER,LOW);setoff=true;}
                 if(temp2<=buffer3.BED_TEMP && buffer3.HEATED_BED==1){
                     digitalWrite(BED_HEATER,HIGH);
                 }else{
                     digitalWrite(BED_HEATER,LOW);
                     bed_block=false;
                 }
             }
         }else if(buffer3.THERMOSTAT_bed==0){
             if (currentMillis - previousMillis_Bed >=buffer3.CYCLE_BED && buffer3.HEATED_BED==1){
                 previousMillis_Bed = currentMillis;
                 temp2 = therm2.analog2temp();
                 pidbed.Compute();
                 digitalWrite(BED_HEATER,bedpwm);
                 if(temp2>buffer3.BED_TEMP){
                   bed_block=false;
                 }
             }else if(buffer3.HEATED_BED==0){
                  //digitalWrite(LED_BUILTIN,LOW);
                  digitalWrite(BED_HEATER,LOW);
                  bed_block=false;
              }
         }
         if (buffer3.THERMOSTAT_nozz==1){
           if (currentMillis - previousMillis_Nozz >= buffer3.CYCLE_NOZZ){
              previousMillis_Nozz = currentMillis;
              temp1 = therm1.analog2temp(); // nozzle
              if((temp1<MIN_TEMP_SAFETY || temp1>MAX_TEMP_NOZZLE) && buffer3.HEATED_NOZZLE==1){digitalWrite(BED_HEATER,LOW);digitalWrite(NOZZ_HEATER,LOW);setoff=true;}
              if(temp1<=buffer3.NOZZLE_TEMP && buffer3.HEATED_NOZZLE==1){
               if(bed_block==false){
                 digitalWrite(NOZZ_HEATER,HIGH);
                 }else{
                   digitalWrite(NOZZ_HEATER,LOW);
                   }
              }else{
                digitalWrite(NOZZ_HEATER,LOW); 
                nozz_block=false;}
           }
         }else if(buffer3.THERMOSTAT_nozz==0){
             if (currentMillis - previousMillis_Nozz >=buffer3.CYCLE_NOZZ && buffer3.HEATED_NOZZLE==1){
                 digitalWrite(LED_BUILTIN,HIGH);
                 previousMillis_Nozz = currentMillis;
                 if(bed_block==false){
                    temp1 = therm1.analog2temp(); // nozzle
                    pidnozz.Compute();
                    digitalWrite(NOZZ_HEATER,nozzpwm);
                 }else{
                    digitalWrite(NOZZ_HEATER,LOW);
                 }
                 if(temp1>buffer3.NOZZLE_TEMP){
                   nozz_block=false;
                 }
             }else if(buffer3.HEATED_NOZZLE==0 && (bed_block==false && buffer3.HEATED_BED==1 || buffer3.HEATED_BED==0)){
                digitalWrite(NOZZ_HEATER,LOW); 
                nozz_block=false;
             }
         }
      }
      if((digitalRead(ENCODER_PIN)==LOW) && (GM_command==false) && ((nozz_block==1 && buffer3.Wait_nozz==1) || (bed_block==1 && buffer3.Wait_bed==1))){setoff=true;}
   }while(((nozz_block==1 && buffer3.Wait_nozz==1) || (bed_block==1 && buffer3.Wait_bed==1)) && setoff==false);
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
  int ii=0;
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
///////////////////////////**************************///////////////////
   if(Serial.available() && signal_duration < BUTTON_TIME_DURATION){
       Serial.readBytes((uint8_t *)&buffer5, sizeof(buffer5));
       Serial.write((uint8_t*)&pass,sizeof(pass));
       if(buffer5.A==0){                             ///A=0 => idle mode
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
                   analogWrite(FAN,buffer5.J);
            }else if(buffer5.B == 5){
                   digitalWrite(LED_BUILTIN,HIGH);
                   Atune_loop(buffer5.C);
                   digitalWrite(LED_BUILTIN,LOW);
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
      heater = NOZZ_HEATER;
  }else{  //Bed Autotune
      target_tune_temp = BED_TUNE_TEMP;
      heater = BED_HEATER;
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
   digitalWrite(BED_HEATER,LOW);    //Disable Heaters while rapid positioning for safety
   digitalWrite(NOZZ_HEATER,LOW);   //Disable Heaters while rapid positioning for safety
   while(step_counter<buffer5.J && buffer5.C==0 && buffer5.D==1 && setoff==false && Serial.available()==0){
      PORTF = (buffer5.E<<PF1);
      PORTF = (buffer5.E<<PF1)|(1<<PF0);
      delayMicroseconds(buffer5.I);
      PORTF = (buffer5.E<<PF1);
      PORTF = (buffer5.E<<PF1)|(0<<PF0);
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
      PORTF = (buffer5.E<<PF7);
      PORTF = (buffer5.E<<PF7)|(1<<PF6);
      delayMicroseconds(buffer5.I);
      PORTF = (buffer5.E<<PF7);
      PORTF = (buffer5.E<<PF7)|(0<<PF6);
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
      PORTL = (buffer5.E<<PL1);
      PORTL = (buffer5.E<<PL1)|(1<<PL3);
      delayMicroseconds(buffer5.I);
      PORTL = (buffer5.E<<PL1);
      PORTL = (buffer5.E<<PL1)|(0<<PL3);
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
      PORTA = (buffer5.E<<PA6);
      PORTA = (buffer5.E<<PA6)|(1<<PA4);
      delayMicroseconds(buffer5.I);
      PORTA = (buffer5.E<<PA6);
      PORTA = (buffer5.E<<PA6)|(0<<PA4);
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
  boolean signalstate=true;
  unsigned long prev=0;
  long  ABL_Height=500, rev=0;
  float ABL_Z=0;
  int Z_revdir=buffer3.HOME_Z_DIR,iter=0;
    
  digitalWrite(BED_HEATER,LOW);    //Disable Heaters while homing for safety
  digitalWrite(NOZZ_HEATER,LOW);   //Disable Heaters while homing for safety
  XMIN_READ=digitalRead(HOME_XMIN_PIN);
  while(buffer3.HOME_X_ENABLE==true && buffer5.R==1 && XMIN_READ==buffer3.HOME_X_STATE && setoff==false){
    PORTF = (buffer3.HOME_X_DIR<<PF1);
    PORTF = (buffer3.HOME_X_DIR<<PF1)|(1<<PF0);
    delayMicroseconds(buffer3.HOME_X_DURATION);
    PORTF = (buffer3.HOME_X_DIR<<PF1);
    PORTF = (buffer3.HOME_X_DIR<<PF1)|(0<<PF0);
    delayMicroseconds(buffer3.HOME_X_DURATION);
    XMIN_READ=digitalRead(HOME_XMIN_PIN);
    if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
  }
  YMIN_READ=digitalRead(HOME_YMIN_PIN);
  while(buffer3.HOME_Y_ENABLE==true && buffer5.S==1 && YMIN_READ==buffer3.HOME_Y_STATE && setoff==false){
    PORTF = (buffer3.HOME_Y_DIR<<PF7);
    PORTF = (buffer3.HOME_Y_DIR<<PF7)|(1<<PF6);
    delayMicroseconds(buffer3.HOME_Y_DURATION);
    PORTF = (buffer3.HOME_Y_DIR<<PF7);
    PORTF = (buffer3.HOME_Y_DIR<<PF7)|(0<<PF6);
    delayMicroseconds(buffer3.HOME_Y_DURATION);
    YMIN_READ=digitalRead(HOME_YMIN_PIN);
    if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
  }
  if(buffer5.U>0){buffer3.HOME_X_DIR=!buffer3.HOME_X_DIR;}else{buffer5.U=-buffer5.U;}
  step_counter=0;
   while(step_counter<buffer5.U){
      PORTF = (buffer3.HOME_X_DIR<<PF1);
      PORTF = (buffer3.HOME_X_DIR<<PF1)|(1<<PF0);
      delayMicroseconds(buffer3.HOME_X_DURATION);
      PORTF = (buffer3.HOME_X_DIR<<PF1);
      PORTF = (buffer3.HOME_X_DIR<<PF1)|(0<<PF0);
      delayMicroseconds(buffer3.HOME_X_DURATION);
      step_counter++;
      //if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
   }
   if(buffer5.V>0){buffer3.HOME_Y_DIR=!buffer3.HOME_Y_DIR;}else{buffer5.V=-buffer5.V;}
   step_counter=0;
   while(step_counter<buffer5.V){
      PORTF = (buffer3.HOME_Y_DIR<<PF7);
      PORTF = (buffer3.HOME_Y_DIR<<PF7)|(1<<PF6);
      delayMicroseconds(buffer3.HOME_Y_DURATION);
      PORTF = (buffer3.HOME_Y_DIR<<PF7);
      PORTF = (buffer3.HOME_Y_DIR<<PF7)|(0<<PF6);
      delayMicroseconds(buffer3.HOME_Y_DURATION);
      step_counter++;
      //if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
   }
    step_counter=0;
  if(ABL){
    bltouch.write(10);
    delay(300);
    while(iter<ABL_ITERATIONS){
      ZMIN_READ=digitalRead(HOME_ZMIN_PIN);
      while(buffer3.HOME_Z_ENABLE==true && buffer5.T==1 && ZMIN_READ==buffer3.HOME_Z_STATE && setoff==false){
        if (micros()-prev >= buffer3.HOME_Z_DURATION){
          ABL_Z=ABL_Z+1;
          prev = micros();
          if (signalstate){
            PORTL = (buffer3.HOME_Z_DIR<<PL1);
            PORTL = (buffer3.HOME_Z_DIR<<PL1)|(1<<PL3);
          }else{
            PORTL = (buffer3.HOME_Z_DIR<<PL1);
            PORTL = (buffer3.HOME_Z_DIR<<PL1)|(0<<PL3);
          }
          signalstate=!signalstate;
        }
        if(digitalRead(ENCODER_PIN)==LOW){
          setoff=true;
        }
        ZMIN_READ=digitalRead(HOME_ZMIN_PIN);
        if(ZMIN_READ!=buffer3.HOME_Z_STATE){
          bltouch.write(90);
          delay(200);
          buffer4.command =-301; //ABL_ZTrack_Packet
          buffer4.nozz_temp = ABL_Z;
          Serial.write((char*)&buffer4,sizeof(buffer4));
          if(buffer3.HOME_Z_DIR==0){Z_revdir=1;}else{Z_revdir=0;}
          while(ABL_Z>0){
            if (micros()-prev >= buffer3.HOME_Z_DURATION){
              ABL_Z=ABL_Z-1;
              prev = micros();
              if (signalstate){
                PORTL = (Z_revdir<<PL1);
                PORTL = (Z_revdir<<PL1)|(1<<PL3);
              }else{
                PORTL = (Z_revdir<<PL1);
                PORTL = (Z_revdir<<PL1)|(0<<PL3);
              }
              signalstate=!signalstate;
            }            
          }
          if(iter<ABL_ITERATIONS-1){
            bltouch.write(10);
            delay(300);
          }
          ABL_Z=0;
          rev=0;
        }            
      }
      iter++;
    }
    bltouch.write(90);
    delay(300);
    buffer4.command =-303; //ABL_ZTrack_Packet
    Serial.write((char*)&buffer4,sizeof(buffer4));
    GM_command=false;
    iter=0;
  }else{              
    bltouch.write(10);
    delay(300);
    ZMIN_READ=digitalRead(HOME_ZMIN_PIN);
    while(buffer3.HOME_Z_ENABLE==true && buffer5.T==1 && ZMIN_READ==buffer3.HOME_Z_STATE && setoff==false){
      ZMIN_READ=digitalRead(HOME_ZMIN_PIN);
      if (micros()-prev >= buffer3.HOME_Z_DURATION){
        prev = micros();
        if (signalstate){
        PORTL = (buffer3.HOME_Z_DIR<<PL1);
        PORTL = (buffer3.HOME_Z_DIR<<PL1)|(1<<PL3);
        }else{
        PORTL = (buffer3.HOME_Z_DIR<<PL1);
        PORTL = (buffer3.HOME_Z_DIR<<PL1)|(0<<PL3);
        ABL_Z=ABL_Z+1;
        }
        signalstate=!signalstate;
      }
      if(digitalRead(ENCODER_PIN)==LOW){setoff=true;}
      if(ZMIN_READ!=buffer3.HOME_Z_STATE){
          bltouch.write(90);
          delay(300);
      }
    }
  }
  if(buffer5.W<0){buffer3.HOME_Z_DIR=!buffer3.HOME_Z_DIR;buffer5.W=-buffer5.W;}
  step_counter=0;
  while(step_counter<buffer5.W){
      PORTL = (buffer3.HOME_Z_DIR<<PL1);
      PORTL = (buffer3.HOME_Z_DIR<<PL1)|(1<<PL3);
      delayMicroseconds(buffer3.HOME_Y_DURATION);
      PORTL = (buffer3.HOME_Z_DIR<<PL1);
      PORTL = (buffer3.HOME_Z_DIR<<PL1)|(0<<PL3);
      delayMicroseconds(buffer3.HOME_Y_DURATION);
      step_counter++;
   }
   step_counter=0;
}

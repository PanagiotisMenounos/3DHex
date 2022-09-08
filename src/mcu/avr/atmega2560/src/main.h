#include <SPI.h>
#include "SdFat.h"
#include <TimerThree.h>
#include <thermistor.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define SERIAL_BAUD_RATE 400000
#define BUTTON_TIME_DURATION 1000
#define LCD_16x4 true
#define ENCODER_PIN 35
#define SD_ENABLE 53
#define MIN_TEMP_SAFETY 5
#define MAX_TEMP_NOZZLE 250
#define MAX_TEMP_BED 80
#define BLTOUCH_PIN 4
#define BUFFERSIZE 3000
#define IDLE_USB_TEMP_UPDATE_INTERVAL 1000
#define NOZZLE_TUNE_TEMP 80
#define BED_TUNE_TEMP 50
#define pi 3.14159265358979323846

uint8_t *portxstep;
uint8_t portxstepmask;
uint8_t *portxdir;
uint8_t portxdirmask;

uint8_t *portystep;
uint8_t portystepmask;
uint8_t *portydir;
uint8_t portydirmask;

uint8_t *portzstep;
uint8_t portzstepmask;
uint8_t *portzdir;
uint8_t portzdirmask;

uint8_t *portestep;
uint8_t portestepmask;
uint8_t *portedir;
uint8_t portedirmask;


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
void configure_pin_modes();
void configure_pin_ports();
void configure_idle_pin_modes();
void configure_idle_pin_ports();
void push_bits();
void execute_command();
void rapid_position();
void service_routine();

SdFat SD;
File myFile;
Servo bltouch;

struct data1 { 
   volatile byte byte_1[BUFFERSIZE];
};

struct data2 { 
   volatile byte byte_2[BUFFERSIZE];
};

struct data6{ 
   volatile uint8_t test;
};
struct data3{
   volatile int16_t X_ENABLE_PIN;
   volatile int16_t X_STEP_PIN;
   volatile int16_t X_DIR_PIN;
   volatile int16_t X_ENDSTOP_PIN;
   volatile int16_t Y_ENABLE_PIN;
   volatile int16_t Y_STEP_PIN;
   volatile int16_t Y_DIR_PIN;
   volatile int16_t Y_ENDSTOP_PIN;
   volatile int16_t Z_ENABLE_PIN;
   volatile int16_t Z_STEP_PIN;
   volatile int16_t Z_DIR_PIN;
   volatile int16_t Z_ENDSTOP_PIN;
   volatile int16_t Z1_ENABLE_PIN;
   volatile int16_t Z1_STEP_PIN;
   volatile int16_t Z1_DIR_PIN;
   volatile int16_t Z1_ENDSTOP_PIN;
   volatile int16_t E_ENABLE_PIN;
   volatile int16_t E_STEP_PIN;
   volatile int16_t E_DIR_PIN;
   volatile int16_t E_ENDSTOP_PIN;
   
   volatile int16_t N_HEATER_PIN;
   volatile int16_t N_SENSOR_PIN;
   volatile int16_t N_FAN_PIN;
   volatile int16_t B_HEATER_PIN;
   volatile int16_t B_SENSOR_PIN;
   volatile int16_t FAN_PIN;
   volatile int16_t SERVO1_PIN;
   volatile int16_t SERVO2_PIN;
   
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
   volatile int8_t test1;
   volatile int8_t test2;
   volatile int16_t X_ENABLE_PIN;
   volatile int16_t X_STEP_PIN;
   volatile int16_t X_DIR_PIN;
   volatile int16_t X_ENDSTOP_PIN;
   volatile int16_t Y_ENABLE_PIN;
   volatile int16_t Y_STEP_PIN;
   volatile int16_t Y_DIR_PIN;
   volatile int16_t Y_ENDSTOP_PIN;
   volatile int16_t Z_ENABLE_PIN;
   volatile int16_t Z_STEP_PIN;
   volatile int16_t Z_DIR_PIN;
   volatile int16_t Z_ENDSTOP_PIN;
   volatile int16_t Z1_ENABLE_PIN;
   volatile int16_t Z1_STEP_PIN;
   volatile int16_t Z1_DIR_PIN;
   volatile int16_t Z1_ENDSTOP_PIN;
   volatile int16_t E_ENABLE_PIN;
   volatile int16_t E_STEP_PIN;
   volatile int16_t E_DIR_PIN;
   volatile int16_t E_ENDSTOP_PIN;
   volatile int16_t N_HEATER_PIN;
   volatile int16_t N_SENSOR_PIN;
   volatile int16_t N_FAN_PIN;
   volatile int16_t B_HEATER_PIN;
   volatile int16_t B_SENSOR_PIN;
   volatile int16_t FAN_PIN;
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
volatile struct data6 buffer6;

thermistor therm1(0,buffer3.THERMISTOR_TYPE_NOZZLE);// nozzle
thermistor therm2(0,buffer3.THERMISTOR_TYPE_BED);  // bed

double temp1,temp2,nozzpwm,nozztemp,p_nozz,i_nozz,d_nozz,bedpwm,bedtemp,p_bed,i_bed,d_bed;
volatile boolean bufferstate=true,readstate=false,setoff=false,nozz_block=true,bed_block=true,once=false,PRINTING=false,GM_command=false,GM_init=false;
volatile unsigned int i=0,j=0,cc,step_counter=0;
volatile byte buf[sizeof(buffer3)],PRINT_STATE,MG_buf[sizeof(buffer5)];
volatile float time_duration;
volatile unsigned long terminate_counter=0,currentMillis=0,previousMillis_Nozz = 0,previousMillis_Bed = 0,signal_duration=0,off=0,on=0,previousMillis_USBupdate=0,usboff=0,usb_duration=0,usbon=0,last_usb_wait=0,usb_wait=0,time_counter=0;
volatile int XMIN_READ,YMIN_READ,ZMIN_READ,set_counter=0;
volatile uint8_t pass=1,fail=0,command_value=255;
volatile int8_t p;
double input, output, setpoint=0;
int state_r,ABL_ITERATIONS;
volatile boolean AUTOTUNE=false,ABL=false,homing=false,init_homing=true;
volatile unsigned int XPOS=0,YPOS=0,ZPOS=0;
volatile unsigned long currentMillis_pos=0,previousMillis_USBupdate_pos=0;
volatile boolean signalstate=true,probe_up=false,elevation=false,deploy_probe=false,delay_homing=true,save_probe=false,set_temp=false,dwell=false,homeX_done=false,homeY_done=false;
volatile boolean execute_offsetX=false,execute_offsetY=false;
volatile float ABL_Z=0,z_tracking;
volatile int Z_revdir=buffer3.HOME_Z_DIR,iter=0;

LiquidCrystal lcd(16, 17, 23, 25, 27, 29);
//LiquidCrystal lcd(0, 0, 0, 0, 0, 0);

PID pidnozz(&temp1, &nozzpwm, &nozztemp,p_nozz,i_nozz,d_nozz, DIRECT);
PID pidbed(&temp2, &bedpwm, &bedtemp,p_bed,i_bed,d_bed, DIRECT);
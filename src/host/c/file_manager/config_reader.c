#include "../global_vars/globals.h"

void read_settings()
{
	#define total_pins 48
	
    FILE *box_sett;
    FILE *cbox_sett;
    FILE *pins_sett;
    char b[70][30],c[29][30],temp_str[30],pins[total_pins][30];  
    int i;
    box_sett=_wfopen(boxes_path,L"r");
	cbox_sett=_wfopen(cboxes_path,L"r");
	pins_sett=_wfopen(pins_path,L"r");
    
    for (i=0;i<71;i++){
        fgets (temp_str, 30, box_sett);
        strcpy(b[i],temp_str);
    }
    sscanf(b[1], "%lf", &STPU_X);
	sscanf(b[2], "%lf", &STPU_Y);
	sscanf(b[3], "%lf", &STPU_Z);
	sscanf(b[4], "%lf", &STPU_E);
	HOME_FEED_X=atoi(b[6]);
	HOME_FEED_Y=atoi(b[7]);
	HOME_FEED_Z=atoi(b[8]);
    sscanf(b[11], "%lf", &MAX_FX);
	sscanf(b[12], "%lf", &MAX_FY);
	sscanf(b[13], "%lf", &MAX_FZ);
	sscanf(b[14], "%lf", &MAX_FE);
    sscanf(b[15], "%lf", &JMFEED);
	sscanf(b[16], "%lf", &MAX_JFX); 
	sscanf(b[17], "%lf", &MAX_JFY);
	sscanf(b[18], "%lf", &MAX_JFZ);
	sscanf(b[19], "%lf", &MAX_JFE);
    sscanf(b[20], "%lf", &ACCELERATION);
	sscanf(b[21], "%lf", &MAX_ACCX);
	sscanf(b[22], "%lf", &MAX_ACCY);
	sscanf(b[23], "%lf", &MAX_ACCZ); 
	sscanf(b[24], "%lf", &MAX_ACCE);
	sscanf(b[25], "%lf", &JERK);
    sscanf(b[31], "%lf", &T0_X_OFFSET);
	sscanf(b[32], "%lf", &T0_Y_OFFSET);
	sscanf(b[33], "%lf", &T0_Z_OFFSET);
	NOZZLE_TEMP=atoi(b[35]);
	BED_TEMP=atoi(b[36]);
    THERMISTOR_TYPE_NOZZLE=atoi(b[37]);
    THERMISTOR_TYPE_BED=atoi(b[38]);
    CYCLE_NOZZ=atoi(b[39]);
    CYCLE_BED=atoi(b[40]);
    sscanf(b[41], "%lf", &P_NOZZ);
    sscanf(b[42], "%lf", &P_BED);
    sscanf(b[43], "%lf", &I_NOZZ);
    sscanf(b[44], "%lf", &I_BED);
    sscanf(b[45], "%lf", &D_NOZZ);
    sscanf(b[46], "%lf", &D_BED);
    CORE_FREQ=atoi(b[47]);
    BAUD_RATE=atoi(b[48]);
    sscanf(b[49], "%lf", &MAX_FILE_SIZE);
    sscanf(b[50], "%lf", &XY_PLANE);	
	sscanf(b[51], "%lf", &ZX_PLANE);
	sscanf(b[52], "%lf", &ZY_PLANE);
	sscanf(b[53], "%lf", &BED_XSIZE);
	sscanf(b[54], "%lf", &BED_YSIZE);
	ABL_ITERATIONS=atoi(b[57]);
	sscanf(b[61], "%lf", &ABL_Z_Fade);
	sscanf(b[66], "%lf", &GRID_RESOLUTION);
	sscanf(b[70], "%lf", &ANGLE_THRESHOLD);
    i=0;
    for (i=0;i<29;i++){
        fgets (temp_str, 30, cbox_sett);
        strcpy(c[i],temp_str);
    }
    X_ENABLE=atoi(c[1]);
	Y_ENABLE=atoi(c[2]);
	Z_ENABLE=atoi(c[3]);
	E_ENABLE=atoi(c[4]);
    Invrt_X=atoi(c[6]);
	Invrt_Y=atoi(c[7]);
	Invrt_Z=atoi(c[8]);
	Invrt_E=atoi(c[9]);
    HOME_X_ENABLE=atoi(c[11]);
	HOME_Y_ENABLE=atoi(c[12]);
	HOME_Z_ENABLE=atoi(c[13]);
	HOME_X_STATE=atoi(c[16]);
	HOME_Y_STATE=atoi(c[17]);
	HOME_Z_STATE=atoi(c[18]);
    UNITS=atoi(c[20]);
	HEATED_NOZZLE=atoi(c[21]);
	HEATED_BED=atoi(c[22]);
    Wait_nozz=atoi(c[23]);
    Wait_bed=atoi(c[24]);
    PID_nozz=atoi(c[25]); 
	PID_bed=atoi(c[26]);
    CURVE_DETECTION=atoi(c[27]);
	ABL_Include	= atoi(c[28]);
	
    for (i=0;i<total_pins;i++){
        fgets (temp_str, 30, pins_sett);
        strcpy(pins[i],temp_str);
    }
    X_ENABLE_PIN=atoi(pins[0]);
    X_STEP_PIN=atoi(pins[1]);
    X_DIR_PIN=atoi(pins[2]);
    X_ENDSTOP_PIN=atoi(pins[3]);
    Y_ENABLE_PIN=atoi(pins[4]);
    Y_STEP_PIN=atoi(pins[5]);
    Y_DIR_PIN=atoi(pins[6]);
    Y_ENDSTOP_PIN=atoi(pins[7]);
    Z_ENABLE_PIN=atoi(pins[8]);
    Z_STEP_PIN=atoi(pins[9]);
    Z_DIR_PIN=atoi(pins[10]);
    Z_ENDSTOP_PIN=atoi(pins[11]);
    Z1_ENABLE_PIN=atoi(pins[12]);
    Z1_STEP_PIN=atoi(pins[13]);
    Z1_DIR_PIN=atoi(pins[14]);
    Z1_ENDSTOP_PIN=atoi(pins[15]);
    E_ENABLE_PIN=atoi(pins[16]);
    E_STEP_PIN=atoi(pins[17]);
    E_DIR_PIN=atoi(pins[18]);
    E_ENDSTOP_PIN=atoi(pins[19]);
    N_HEATER_PIN=atoi(pins[20]);
    N_SENSOR_PIN=atoi(pins[21]);
    N_FAN_PIN=atoi(pins[22]);
    B_HEATER_PIN=atoi(pins[23]);
    B_SENSOR_PIN=atoi(pins[24]);
    FAN_PIN=atoi(pins[25]);
    SERVO1_PIN=atoi(pins[26]);
    SERVO2_PIN=atoi(pins[27]);
    EXP1_1_PIN=atoi(pins[28]);
    EXP1_3_PIN=atoi(pins[29]);
    EXP1_5_PIN=atoi(pins[30]);
    EXP1_7_PIN=atoi(pins[31]);
    EXP1_9_PIN=atoi(pins[32]);
    EXP1_2_PIN=atoi(pins[33]);
    EXP1_4_PIN=atoi(pins[34]);
    EXP1_6_PIN=atoi(pins[35]);
    EXP1_8_PIN=atoi(pins[36]);
    EXP1_10_PIN=atoi(pins[37]);
    EXP2_1_PIN=atoi(pins[38]);
    EXP2_3_PIN=atoi(pins[39]);
    EXP2_5_PIN=atoi(pins[40]);
    EXP2_7_PIN=atoi(pins[41]);
    EXP2_9_PIN=atoi(pins[42]);
    EXP2_2_PIN=atoi(pins[43]);
    EXP2_4_PIN=atoi(pins[44]);
    EXP2_6_PIN=atoi(pins[45]);
    EXP2_8_PIN=atoi(pins[46]);
    EXP2_10_PIN=atoi(pins[47]);
	//FOR THE MOMMENT
	if(Invrt_X==0){HOME_X_DIR=1;}else{HOME_X_DIR=0;}	
	if(Invrt_Y==0){HOME_Y_DIR=1;}else{HOME_Y_DIR=0;}
	if(Invrt_Z==0){HOME_Z_DIR=1;}else{HOME_Z_DIR=0;}
    COM_PORT=0;
	DIFFERENTIAL_NOZZ=0;
    DIFFERENTIAL_BED=0;
    ////
    MAX_FILE_SIZE=MAX_FILE_SIZE*pow(10,-5);
	P_NOZZ_MIRROR=P_NOZZ*10;
	I_NOZZ_MIRROR=I_NOZZ*10;
	D_NOZZ_MIRROR=D_NOZZ*10;
	P_BED_MIRROR=P_BED*10;
	I_BED_MIRROR=I_BED*10;
	D_BED_MIRROR=D_BED*10;
	if(STPU_X!=0 && HOME_X_ENABLE==1 && HOME_FEED_X!=0){
	 	HOME_X_DURATION=1000000/(STPU_X*HOME_FEED_X*2);
	 }else{
	 	HOME_X_DURATION=0;
	 }
	if(STPU_Y!=0 && HOME_Y_ENABLE==1 && HOME_FEED_Y!=0){
	 	HOME_Y_DURATION=1000000/(STPU_Y*HOME_FEED_Y*2);
	 }else{
	 	HOME_Y_DURATION=0;
	 }
	if(STPU_Z!=0 && HOME_Z_ENABLE==1 && HOME_FEED_Z!=0){
	 	HOME_Z_DURATION=1000000/(STPU_Z*HOME_FEED_Z*2);
	 }else{
	 	HOME_Z_DURATION=0;
	 }
	X_GLOB_STEP=1.0/STPU_X;
	Y_GLOB_STEP=1.0/STPU_Y;
	fclose(box_sett);
    fclose(cbox_sett);
    fclose(pins_sett);
    			MG_data.X_ENABLE_PIN  =X_ENABLE_PIN  ;
                MG_data.X_STEP_PIN    =X_STEP_PIN    ;
				MG_data.X_DIR_PIN     =X_DIR_PIN     ;
				MG_data.X_ENDSTOP_PIN =X_ENDSTOP_PIN ;
				MG_data.Y_ENABLE_PIN  =Y_ENABLE_PIN  ;
				MG_data.Y_STEP_PIN    =Y_STEP_PIN    ;
				MG_data.Y_DIR_PIN     =Y_DIR_PIN     ;
				MG_data.Y_ENDSTOP_PIN =Y_ENDSTOP_PIN ;
				MG_data.Z_ENABLE_PIN  =Z_ENABLE_PIN  ;
				MG_data.Z_STEP_PIN    =Z_STEP_PIN    ;
				MG_data.Z_DIR_PIN     =Z_DIR_PIN     ;
				MG_data.Z_ENDSTOP_PIN =Z_ENDSTOP_PIN ;
				MG_data.Z1_ENABLE_PIN =Z1_ENABLE_PIN ;
				MG_data.Z1_STEP_PIN   =Z1_STEP_PIN   ;
				MG_data.Z1_DIR_PIN    =Z1_DIR_PIN    ;
				MG_data.Z1_ENDSTOP_PIN=Z1_ENDSTOP_PIN;
				MG_data.E_ENABLE_PIN  =E_ENABLE_PIN  ;
				MG_data.E_STEP_PIN    =E_STEP_PIN    ;
				MG_data.E_DIR_PIN     =E_DIR_PIN     ;
				MG_data.E_ENDSTOP_PIN =E_ENDSTOP_PIN ;
				MG_data.N_HEATER_PIN  =N_HEATER_PIN  ;
				MG_data.N_SENSOR_PIN  =N_SENSOR_PIN  ;
				MG_data.N_FAN_PIN     =N_FAN_PIN     ;
				MG_data.B_HEATER_PIN  =B_HEATER_PIN  ;
				MG_data.B_SENSOR_PIN  =B_SENSOR_PIN  ;
				MG_data.FAN_PIN       =FAN_PIN       ;
			    //MG_data.SERVO1_PIN    =SERVO1_PIN    ;
   				//MG_data.SERVO2_PIN    =SERVO2_PIN    ;
}


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
#include <stdio.h>
#include <stdint.h>
#include <process.h> 
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <malloc.h> 
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <shlobj.h> //to get %APPDATA% path
#define pi 3.14159265358979323846
#define max_bufferfile_size 3000
#define pipename "\\\\.\\pipe\\Foo"
#define MG_DATA_SIZE 36

union{
    int progress;
    byte temp_array[4];
} u;
  
union {
	int M;
    byte fly_array[4];
} y;

union {
	float S;
    byte fly_array[4];
  } t;
       ///////////////**************GLOBAL VARIABLES FOR SETTINGS******************/////////////////////
double STPU_X,STPU_Y,STPU_Z,STPU_E,MAX_FX,MAX_FY,MAX_FZ,MAX_FE,MAX_ACCX,MAX_ACCY,MAX_ACCZ,MAX_ACCE, ACCELERATION,T_ACCEL_ERATION\
       ,JERK,T_JERK,MAX_JFX,\
       MAX_JFY,MAX_JFZ,MAX_JFE,JMFEED,T_JMFEED,PARK_X=0,PARK_Y=0,PARK_Z=100,PARK_FEED=30,MAX_FILE_SIZE,T0_X_OFFSET,T0_Y_OFFSET,T0_Z_OFFSET;
int Invrt_X,Invrt_Y,Invrt_Z,Invrt_E,COM_PORT,UNITS,ABL_ITERATIONS;
int32_t BAUD_RATE;
uint16_t CORE_FREQ;
uint8_t X_ENABLE,Y_ENABLE,Z_ENABLE,E_ENABLE;

double P_NOZZ,I_NOZZ,D_NOZZ,P_BED,I_BED,D_BED;
uint16_t NOZZLE_TEMP,BED_TEMP,CYCLE_NOZZ,CYCLE_BED,P_NOZZ_MIRROR,I_NOZZ_MIRROR,D_NOZZ_MIRROR,P_BED_MIRROR,I_BED_MIRROR,D_BED_MIRROR;
uint8_t Wait_nozz,Wait_bed,THERMISTOR_TYPE_NOZZLE,THERMISTOR_TYPE_BED,HEATED_NOZZLE,HEATED_BED,PID_nozz,\
        PID_bed,DIFFERENTIAL_NOZZ,DIFFERENTIAL_BED;
        
int HOME_FEED_X,HOME_FEED_Y,HOME_FEED_Z;
uint16_t HOME_X_DURATION,HOME_Y_DURATION,HOME_Z_DURATION;
uint8_t HOME_X_ENABLE,HOME_Y_ENABLE,HOME_Z_ENABLE,HOME_X_STATE,HOME_Y_STATE,HOME_Z_STATE,HOME_X_DIR,HOME_Y_DIR,HOME_Z_DIR;

double XY_PLANE,ZX_PLANE,ZY_PLANE;

int CURVE_DETECTION;
double ANGLE_THRESHOLD;

uint8_t GP,GC,LAST_TIME_BOXES;
bool embedded_line=false;

double JM_PRC=1,FD_PRC=1,AC_PRC=1,JR_PRC=1;

double ABL_X=0,ABL_Y=0,BED_WIDTH=200,BED_LENGTH=200,ABL_Z_Fade;
double BED_XSIZE, BED_YSIZE,ABL_Z_last=0,GRID_RESOLUTION=1;
bool ABL_Process=false,ABL_Data=false,HOME_ALL=false;
int ABL_COORD_SIZE=0,ABL_resolution=1,ABL_Include;

uint8_t X_ENABLE_PIN,X_STEP_PIN,X_DIR_PIN,X_ENDSTOP_PIN,Y_ENABLE_PIN,Y_STEP_PIN,Y_DIR_PIN,Y_ENDSTOP_PIN,\
    	Z_ENABLE_PIN,Z_STEP_PIN,Z_DIR_PIN,Z_ENDSTOP_PIN,Z1_ENABLE_PIN,Z1_STEP_PIN,Z1_DIR_PIN,Z1_ENDSTOP_PIN,\
		E_ENABLE_PIN,E_STEP_PIN,E_DIR_PIN,E_ENDSTOP_PIN,N_HEATER_PIN,N_SENSOR_PIN,N_FAN_PIN,B_HEATER_PIN,B_SENSOR_PIN,\
    	FAN_PIN,SERVO1_PIN,SERVO2_PIN,EXP1_1_PIN,EXP1_3_PIN,EXP1_5_PIN,EXP1_7_PIN,EXP1_9_PIN,EXP1_2_PIN,EXP1_4_PIN,\
		EXP1_6_PIN,EXP1_8_PIN,EXP1_10_PIN,EXP2_1_PIN,EXP2_3_PIN,EXP2_5_PIN,EXP2_7_PIN,EXP2_9_PIN,EXP2_2_PIN,\
		EXP2_4_PIN, EXP2_6_PIN, EXP2_8_PIN,EXP2_10_PIN;


//float *ABL_YCOORD=(float*)malloc(sizeof(float));
//float *ABL_ZCOORD=(float*)malloc(sizeof(float));
     ////////////////////////*****************/////////////////////////////////////

//////////////////////////////***********GLOBAL VARIABLES******//////////////////
int total_digi_lines=0,state,file_buffer_size=0,PRINT_STATE, file_num=0;//number name of binary file
double storage_step=0.00000000000001,storage_counter=0,ACCEL_ERATION,theta_adj_last,flag_num=101101101.101010,axis_num=10201.102012;
bool first=false,flag_file_state=false,first_time_executed=true,f_adj=true,clockwise,ABSOLUTE_POSITIONING=1,E_ABSOLUTE_POSITIONING=1;
double tmin,u1_t1,u2_t2,x1_t1,x2_t2,x3_t3,x4_t4,x5_t5,x6_t6,x7_t7,t1,t2,t3,t4,t5,t6,t7,cu,ca,time=0,last_time=0;
double LOC_CASE=1,new_CURVE,gen_DISTANCE,gen_FEED,trajectory_POINT=0;
double X_GLOB=0,Y_GLOB=0,X_GLOB_STEP,Y_GLOB_STEP,Z_GLOB=0;
wchar_t savepath[19][100]={L"//3DHex2//support files//coordinates.txt\0"  \
                          ,L"//3DHex2//support files//fcoordinates.txt\0" \
						  ,L"//3DHex2//support files//gc2info.txt\0"      \
						  ,L"//3DHex2//support files//savepath.txt\0"     \
						  ,L"//3DHex2//support files//gen.txt\0"          \
						  ,L"//3DHex2//support files//GCODE.txt\0"        \
						  ,L"//3DHex2//settings//boxes settings.txt\0"    \
						  ,L"//3DHex2//settings//cboxes settings.txt\0"   \
						  ,L"//3DHex2//settings//dboxes settings.txt\0"   \
						  ,L"//3DHex2//binary files//buffer_1.bin\0"      \
						  ,L"//3DHex2//binary files//buffer_2.bin\0"      \
						  ,L"//3DHex2//binary files//flag.bin\0"          \
						  ,L"//3DHex2//binary files//flag_py.bin\0"       \
						  ,L"//3DHex2//binary files//child.bin\0"         \
						  ,L"//3DHex2//binary files//fly.bin\0"           \
						  ,L"//3DHex2//settings//abl_x.txt\0"             \
						  ,L"//3DHex2//settings//abl_y.txt\0"             \
						  ,L"//3DHex2//settings//XYZ.txt\0"				  \
						  ,L"//3DHex2//settings//pins settings.txt\0"};                      		  

wchar_t coordinates_path[150]  \
       ,fcoordinates_path[150] \
	   ,gc2info_path[150]      \
	   ,savepath_path[150]     \
	   ,gen_path[150]          \
	   ,gcode_path[150]        \
	   ,boxes_path[150]        \
	   ,cboxes_path[150]       \
	   ,dboxes_path[150]       \
	   ,buffer1_path[150]      \
	   ,buffer2_path[150]      \
	   ,flag_path[150]         \
	   ,startpy_path[150]      \
	   ,child_path[150]        \
	   ,fly_path[150]          \
	   ,abl_x_path[150]        \
	   ,abl_y_path[150]        \
	   ,abl_xyz_path[150]      \
	   ,pins_path[150];
 /////////////////////////*******//////////////////////

FILE *SD_binary_file;
FILE *buffer1_file;
FILE *buffer2_file;
FILE *flag_file;
FILE *startpy_file;
FILE *child_file;
FILE *fly_file;
FILE *abl_x_file;
FILE *abl_y_file;


void LINE(double xf, double yf, double zf, double ef, double xl, double yl, double zl, double el, double FEEDRATE);             //G01,G00 calculations   
void ARC(bool clockwise,double k,double l,double x1_f,double y1_f,double Ef,double x2_f,double y2_f,double El,double FEEDRATE); //GO2,G03 calculations 
int casequartile(double x,double y,double K,double L, int point);                                                                          //find the quartile (1-4) of a point in polar coordinate system
double max_n(double num1, double num2);                                                                                         //returns the max value
double min_n(double num1, double num2) ;                                                                                        //returns the min value
double theta_func(double x,double y,double k,double l,int a);                                                                   //returns the angle of a point-line in polar coordinate system
void time_momments(double L,double umax);                                                                                       //returns the time momments of velocity profile
double L_time_calc(double l);                                                                                                   //returns the time momment of points-pulses in XYZ axis
double line_accel_feed_limits(double dx,double dy,double dz,double E_DIST,double FEEDRATE);                                     //check derivative limits of G00-G01 (line)
double arc_accel_feed_limits(double R,double ARC_DIST,double E_DIST,double FEEDRATE);                                           //check derivativelimits of G02-G03 (arc)
double check_jfeed_limits(double dx,double dy,double dz,double E_DIST);                                                         //check jump feed rate limits (only for lines)
void curve_detection(unsigned long total_lines);                                                                                //finds tangent moves, prevent decelaration
double curve_lines_angles(double xf,double yf,double xl,double yl, double Gl);                                                             //check angle for speed adjustment
void curve_length(unsigned long total_lines);                                                                                   //callculate total curve length
void composite(double Command,double Ef,double El);                                                                             //finds when should be cut the fiber
void path_files();                                                                                                              //locates where the %APPDATA% is 
double check_units(double Fl);                                                                                                  //convert to mm/sec everything
void crt_file();                                                                                                                //creates the output files
void wr2bin(int stepx, int stepy, int stepz, int stepe, double l);                                                              //writes the stop byte to output file
unsigned long gc2info(double flag_num);                                                                                         //write all the information clear to a file provided from Gcode
void read_settings();                                                                                                           //returns all stored settings
void close_SD_binary_file();                                                                                                    //close the output file
void hidecursor();                                                                                                              //hide the cursor on the console
void write_hex2file(uint8_t hex_value);
void mcu_settings_send();
int check_print_state();                                                                                                        //return 0 => USB return 1 => SD_CARD
void check_SD_file_size(double Xf, double Yf, double Zf, double Xl, double Yl, double Zl);
void ABL_readXYZ();
uint8_t bits2val(char *bits);                                                                                                  //convert a serial of 0,1 to a number 16bit (0,1) = 2 bytes(unsigned integer=uint16_t)

///////////////////////****************//////////////////////////
float *ABL_XCOORD;
float *ABL_YCOORD;
float *ABL_ZCOORD;

struct data{
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
}MG_data;

int main(){
	ABL_XCOORD = (float*)malloc(sizeof(float));
	ABL_YCOORD = (float*)malloc(sizeof(float));
	ABL_ZCOORD = (float*)malloc(sizeof(float));
	//HWND hWnd = GetConsoleWindow();
    //ShowWindow( hWnd, SW_MINIMIZE );  //won't hide the window without SW_MINIMIZE
    //ShowWindow( hWnd, SW_HIDE );
    int *ABL_XCOORD;
    
    HANDLE pipe = CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        printf("Error: %d", GetLastError());
    }
    
	DWORD numWritten;
    
    	FILE *g1;
    	FILE *gen1;
    	
    	struct stat fly_buffer;
        int   on_the_fly=0;
    	
        double Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl,Sl,Pl,Rl,Tl,Gt,Mt,St,TEMP_PERC=0;
        unsigned long total_lines,j;
        bool first_line=true,GM_command=false,fly_command=false;
        WriteFile(pipe, &TEMP_PERC, sizeof(float), &numWritten, NULL); //FIX A BUG
        int e_space,ii;
        GP=255;
        path_files();
    	read_settings();
    	ABL_readXYZ();
    	T_ACCEL_ERATION=ACCELERATION;
    	T_JMFEED=JMFEED;
    	T_JERK=JERK;
		hidecursor(); //temporary
		PRINT_STATE = check_print_state(); 
		if(PRINT_STATE==0){ //usb print
		    mcu_settings_send();
		}else{               //sd print
			crt_file();
		}        
        total_lines=gc2info(flag_num); //GCODE parsing
        if(CURVE_DETECTION==1){ 
        	curve_detection(total_lines);
            curve_length(total_lines);
            ABSOLUTE_POSITIONING=1; //curve detection algorithm writes to fcoordinates.txt absolute coordinates
			E_ABSOLUTE_POSITIONING=1; //curve detection algorithm writes to fcoordinates.txt absolute coordinates
            g1=_wfopen(fcoordinates_path,L"r");
		}else{
			g1=_wfopen(coordinates_path,L"r");
		}
        fly_file=_wfopen(fly_path,L"wb"); //reset fly file
		fclose(fly_file);
        gen1=_wfopen(gen_path,L"r"); //Used for curve detection algorithm
        for(j=1;j<=total_lines;j++){ //Read orsed gcode file untli the end
		    if(first_line==true){
		        first_line=false;
	            fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gf,&Mf,&Xf,&Yf,&Zf,&If,&Jf,&Ef,&Ff,&Sf,&Pf,&Rf,&Tf,&new_CURVE);
	            j++;
	    	}
    		on_the_fly = wstat(fly_path, &fly_buffer); //check for on the fly command (Python writes bytes to the fly file in order to inform C)
            if(fly_buffer.st_size==0){ // 0 = no command
               fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&new_CURVE);
    		}else if(fly_buffer.st_size!=0){ //execute on the fly command
                ReadFile(pipe, &y.fly_array, 4, &numWritten, NULL); //read command from PIPE 
                if(y.M!=0){ //M command
                	j=j-1; //no readline at this loop
                	ReadFile(pipe, &t.fly_array, 4, &numWritten, NULL); //S value
                	Gt=Gl; //save current commands
    		    	Mt=Ml; //
    			    St=Sl; //
    			    fly_command=true;
    			    Gl=flag_num; //M command =? G has to be flag_num
    			    Ml=y.M; //Mcommand code
    			    Sl=t.S; //S value
				}else{ //JFAJ command
                    ReadFile(pipe, &t.fly_array, 4, &numWritten, NULL);	
					//printf("%s %f\n","PIPE RECEIVED,%" ,t.S);
    	            JMFEED=T_JMFEED*(t.S/100.0);
                    ReadFile(pipe, &t.fly_array, 4, &numWritten, NULL);	
					//printf("%s %f\n","PIPE RECEIVED,%" ,t.S);
                    FD_PRC=(t.S/100.0);
                    ReadFile(pipe, &t.fly_array, 4, &numWritten, NULL);	
					//printf("%s %f\n","PIPE RECEIVED,%" ,t.S);
					ACCELERATION=T_ACCEL_ERATION*(t.S/100.0);
                    ReadFile(pipe, &t.fly_array, 4, &numWritten, NULL);	
					//printf("%s %f\n","PIPE RECEIVED,%" ,t.S);
					JERK=T_JERK*(t.S/100.0);
					fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&new_CURVE);
                    fly_file=_wfopen(fly_path,L"wb"); //reset fly file
            		fclose(fly_file);										
				}
			}
			if(CURVE_DETECTION==1 && fly_command==false && new_CURVE==1 && (Gl==1 || Gl==2 || Gl==3 || Gl==01 || Gl==02 || Gl==03 || Gl==0 || Gl==00)){
			   last_time=0; //FIX time BUG at the begining of new command
               fscanf(gen1,"%lf" "%lf",&gen_DISTANCE,&gen_FEED); //read curvedistance and feedrate
               gen_FEED=FD_PRC*gen_FEED; 
               gen_FEED=check_units(gen_FEED);
               Fl=gen_FEED;
			}
			if (CURVE_DETECTION==0){
				Fl=Fl*FD_PRC;
				Fl=check_units(Fl);
				last_time=0; //FIX time BUG at the begining of new command
			}
			//printf("%s %f\n","FL=", Fl);
            if(Gl!=flag_num){ /// enter only of it is a G command
            	if(Gl==1 || Gl==01 || Gl==0 || Gl==00){   // G1 LINE COMMAND
            	    if(ABSOLUTE_POSITIONING==0){
                    	Xl=Xf+Xl;
                    	Yl=Yf+Yl;
                    	Zl=Zf+Zl;
		        	}
	        		if(E_ABSOLUTE_POSITIONING==0){
                    	El=Ef+El;
		        	}
				    if(XY_PLANE!=0 && (ZX_PLANE!=0 || ZY_PLANE!=0)){
				    	LINE(0,0,Zf,0,0,0,Zl,0,0); //if Z is lifted, remember to add when Z gets down
				    	LINE(Xf,Yf,0,Ef,Xl,Yl,0,El,Fl);
					}else{
						LINE(Xf,Yf,Zf,Ef,Xl,Yl,Zl,El,Fl);
					}     		
                }
			    if (Gl==2 || Gl==02 || Gl==3 || Gl==03){  // G2/G3 ARC COMMAND
	        		if(E_ABSOLUTE_POSITIONING==0){
                    	El=Ef+El;
		        	}
				    if(Gl==2 || Gl==02){
		    			clockwise=true; //clockwise
		    		}else{
		    			clockwise=false; //counterclockwise
		    		}
		    		ARC(clockwise,Il,Jl,Xf,Yf,Ef,Xl,Yl,El,Fl);                	
				}
				if(Gl==28){ // G28 homing
				    if(ABL_Process){
				      fprintf(abl_x_file,"%lf\n",ABL_X);
				      fprintf(abl_y_file,"%lf\n",ABL_Y);
					}
				    GM_command=true;
					MG_data.A=3;               
					MG_data.I=HOME_X_DIR;
					MG_data.J=HOME_Y_DIR;
					MG_data.K=HOME_Z_DIR;
					if(HOME_X_ENABLE==1 && Xl==axis_num){MG_data.C=1;MG_data.R=1;}else{MG_data.C=0;MG_data.R=0;}
					if(HOME_Y_ENABLE==1 && Yl==axis_num){MG_data.D=1;MG_data.S=1;}else{MG_data.D=0;MG_data.S=0;}
					if(HOME_Z_ENABLE==1 && Zl==axis_num){MG_data.E=1;MG_data.T=1;}else{MG_data.E=0;MG_data.T=0;}
					MG_data.F=HOME_X_STATE;
					MG_data.G=HOME_Y_STATE;
					MG_data.H=HOME_Z_STATE;
					MG_data.O=HOME_X_DURATION;
					MG_data.P=HOME_Y_DURATION;
					MG_data.Q=HOME_Z_DURATION;
					MG_data.U=T0_X_OFFSET*STPU_X;
					MG_data.V=T0_Y_OFFSET*STPU_Y;
					MG_data.W=T0_Z_OFFSET*STPU_Z;
					Xl=T0_X_OFFSET;
               	    Yl=T0_Y_OFFSET;
               	    Zl=0;
					HOME_ALL=true;
				}
				if(Gl==4){ // G4 pause
					GM_command=true;
					MG_data.A=5;
					MG_data.J=0;
					MG_data.K=0;
					MG_data.J=Pl;
					MG_data.K=Sl;					
				}
				if(Gl==2929){//auto bed leveling - bltouch on the way
				    GM_command=true;
				    MG_data.A=7; //
				    MG_data.B=ABL_ITERATIONS;
				    if(ABL_Process){
				    	fclose(abl_x_file);
				    	fclose(abl_y_file);
					}else{
						abl_x_file=_wfopen(abl_x_path,L"w");
						abl_y_file=_wfopen(abl_y_path,L"w");
					}
					
					ABL_Process=!ABL_Process;
				}
				if(Gl==90){// G90 Absolute
				    ABSOLUTE_POSITIONING=1;
				    E_ABSOLUTE_POSITIONING=1;
				}
				if(Gl==91 && CURVE_DETECTION==0){// G91 Relative, enable only if curve detection algorithm is off
					ABSOLUTE_POSITIONING=0;
					E_ABSOLUTE_POSITIONING=0;
				}
				if(Gl==92){// G92 Reset origin if not any of XYZE is given
					if(Xl==axis_num && Yl==axis_num && Zl==axis_num && El==axis_num){
						Xl=0;Yl=0;Zl=0;El=0;
					}
                    ABL_X = round(Xl*STPU_X)/STPU_X;
                    ABL_Y = round(Yl*STPU_Y)/STPU_Y;
				}
				if(GM_command==true){ //GM command is when data needs to be passed to MCU side
					GM_command=false;
		     		write_hex2file(GP);
			    	write_hex2file(GP);
                    e_space=max_bufferfile_size-(file_buffer_size); 
                    if(e_space<=MG_DATA_SIZE&& e_space!=0){
                    	e_space=30;//fix a bug arduino side
                        for(ii=0;ii<e_space;ii++){ //fill with stopbyte until 2times file size;
            	           write_hex2file(GP);
    	               }
    	            }
	                unsigned char *MG_byte_buffer=(char*)malloc(sizeof(MG_data)); //https://www.includehelp.com/c/how-to-copy-complete-structure-in-a-byte-array-character-buffer-in-c%20language.aspx
	                int temp_counter;
	                memcpy(MG_byte_buffer,(const unsigned char*)&MG_data,sizeof(MG_data));
	                for(temp_counter=0;temp_counter<sizeof(MG_data);temp_counter++){
	                	write_hex2file(MG_byte_buffer[temp_counter]);
			    	}
	                free(MG_byte_buffer);
	            }
			}
			if(Ml!=flag_num){
                if(Ml==104){ // M104 set hotend temp no wait
			        MG_data.A=1;
			        MG_data.B=0;
			     	MG_data.J=Sl;				    	
				}
				if(Ml==109){// M190 set hotend temp & wait
			        MG_data.A=1;
			        MG_data.B=1;
			     	MG_data.J=Sl;					
				}
				if(Ml==140){// M140 set bed temp no wait
			        MG_data.A=0;
			        MG_data.B=0;
			     	MG_data.J=Sl;
				}
				if(Ml==190){// M190 sed bed temp & wait
				    MG_data.A=0;
			        MG_data.B=1;
			     	MG_data.J=Sl;
				}
				if(Ml==106){// M106 turn on fan & set speed 0-255
					MG_data.A=4;
			     	MG_data.J=Sl;					
				}
				if(Ml==107){// M107 turn fan off
                    MG_data.A=4;
                    MG_data.J=0;
				}
				if(Ml==17){// M17 Enable stepper motors
				    MG_data.A=2;
			        if(Xl==axis_num){MG_data.B=1;}else{MG_data.B=0;}
					if(Yl==axis_num){MG_data.C=1;}else{MG_data.C=0;}
					if(Zl==axis_num){MG_data.D=1;}else{MG_data.D=0;}
					if(El==axis_num){MG_data.E=1;}else{MG_data.E=0;}					
				}
				if(Ml==18 || Ml==84){// M18/M84 Disable stepper motor
				    MG_data.A=2;
			        if(Xl==axis_num){MG_data.B=0;}else{MG_data.B=1;}
					if(Yl==axis_num){MG_data.C=0;}else{MG_data.C=1;}
					if(Zl==axis_num){MG_data.D=0;}else{MG_data.D=1;}
					if(El==axis_num){MG_data.E=0;}else{MG_data.E=1;}					
				}
				if(Ml==204){// M204 set acceleration
					ACCELERATION=Pl;
				}
				if(Ml==205){// M205 set advanced settings
					if(Jl!=axis_num){JMFEED=Jl;}
					if(Sl!=axis_num){JERK=Sl;}
				}				
				if(Ml==851){// M851 XYZ Probe Offset
					
				}
				if(Ml==82){// M82 E Absolute
					E_ABSOLUTE_POSITIONING=1;
				}
				if(Ml==83){// M83 E Absolute
					E_ABSOLUTE_POSITIONING=0;
				}
				if(Ml==226){// M226 pause until iteraction
					MG_data.A=6;
				}

				write_hex2file(GP);
				write_hex2file(GP);
                e_space=max_bufferfile_size-(file_buffer_size-1); ///-1 fix a bug arduino side
                if(e_space<=MG_DATA_SIZE && e_space!=0){
                   for(ii=0;ii<e_space;ii++){ //fill with stopbyte until 2times file size;
        	          write_hex2file(GP);
    	           }
    	        }
	            unsigned char *MG_byte_buffer=(char*)malloc(sizeof(MG_data)); //https://www.includehelp.com/c/how-to-copy-complete-structure-in-a-byte-array-character-buffer-in-c%20language.aspx
	            int temp_counter;
	            memcpy(MG_byte_buffer,(const unsigned char*)&MG_data,sizeof(MG_data));
	            for(temp_counter=0;temp_counter<sizeof(MG_data);temp_counter++){
	            	write_hex2file(MG_byte_buffer[temp_counter]);
				}
	            free(MG_byte_buffer);
			}
			if(fly_command==false){
				if(Xl!=axis_num && Ml!=205){Xf=Xl;} //do not consider axis_num as a coordinate 
			    if(Yl!=axis_num && Ml!=205){Yf=Yl;}
			    if(Zl!=axis_num && Ml!=205){Zf=Zl;}
			    if(El!=axis_num && Ml!=205){Ef=El;}
			    Gf=Gl;Mf=Ml;If=Il;Jf=Jl;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle
			}else{
				Gl=Gt;
				Ml=Mt;
				Sl=St;
		        fly_file=_wfopen(fly_path,L"w"); //reset fly file
		        fclose(fly_file);
		        //printf("%s\n","RESET FLY");
		        fly_command=false; //reset on the fly command
			}
			u.progress=100*j/total_lines;//calculate the completed percent
			WriteFile(pipe, &u.temp_array, sizeof(float), &numWritten, NULL); //Update python with progress %
			check_SD_file_size(Xf,Yf,Zf,Xl,Yl,Zl);			
        } 
        u.progress=100;
        WriteFile(pipe, &u.temp_array, sizeof(float), &numWritten, NULL);
		//printf("%s\n","COMPLETED!");
		//printf("%s %f\n","X_GLOB=",X_GLOB);
		//printf("%s %f\n","Y_GLOB=",Y_GLOB);
        close_SD_binary_file();
        fclose(buffer1_file);
        fclose(buffer2_file);
        fclose(SD_binary_file);
        fclose(gen1);
		child_file=_wfopen(child_path,L"w"); //reset child file -> to prevent python print >>> Abort after SD
		fclose(child_file);
		free(ABL_XCOORD);
		free(ABL_YCOORD);
		free(ABL_ZCOORD);
		//printf("%s %f\n","z",Z_GLOB);
		CloseHandle(pipe);
}

void ABL_readXYZ(){
	struct stat buffer;
	char string[100];
	int place=0,status;
	
	status = wstat(abl_xyz_path, &buffer);
	
	if(buffer.st_size!=0){
		ABL_Data=true;
		FILE *abl_xyz_file;
		abl_xyz_file=_wfopen(abl_xyz_path,L"r");
		while (fgets(string, 100, abl_xyz_file) != NULL){//TILL THE END OF FILE
			ABL_COORD_SIZE++;
		}
		
		fclose(abl_xyz_file);
		ABL_XCOORD = (float*)malloc(ABL_COORD_SIZE*sizeof(float));
		ABL_YCOORD = (float*)malloc(ABL_COORD_SIZE*sizeof(float));
		ABL_ZCOORD = (float*)malloc(ABL_COORD_SIZE*sizeof(float));
		abl_xyz_file=_wfopen(abl_xyz_path,L"r");
		while(place<ABL_COORD_SIZE){
			fscanf(abl_xyz_file,"%f" "%f" "%f" ,&ABL_XCOORD[place],&ABL_YCOORD[place],&ABL_ZCOORD[place]);
			place++;
		}
		fclose(abl_xyz_file);
    }else{
    	ABL_Data=false;
	}
}
    
    
int check_print_state(){
	struct stat savepath_buffer;
    int         savepath_status;
    savepath_status = wstat(savepath_path, &savepath_buffer);
    if(savepath_buffer.st_size==0){
    	return 0;
    }else{
    	return 1;
	}
}

void mcu_settings_send()
{

    buffer1_file=_wfopen(buffer1_path,L"wb");
    fwrite(&X_STEP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&X_DIR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&CORE_FREQ, sizeof(uint16_t),1,buffer1_file); 
    fwrite(&X_ENABLE, sizeof(uint8_t),1,buffer1_file); 
    fwrite(&Y_ENABLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z_ENABLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&E_ENABLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&NOZZLE_TEMP, sizeof(uint16_t),1,buffer1_file);
    fwrite(&BED_TEMP, sizeof(uint16_t),1,buffer1_file);
    fwrite(&CYCLE_NOZZ, sizeof(uint16_t),1,buffer1_file);
    fwrite(&CYCLE_BED, sizeof(uint16_t),1,buffer1_file);
    fwrite(&P_NOZZ_MIRROR, sizeof(uint16_t),1,buffer1_file);
    fwrite(&I_NOZZ_MIRROR, sizeof(uint16_t),1,buffer1_file);
    fwrite(&D_NOZZ_MIRROR, sizeof(uint16_t),1,buffer1_file);
    fwrite(&P_BED_MIRROR, sizeof(uint16_t),1,buffer1_file);
    fwrite(&I_BED_MIRROR, sizeof(uint16_t),1,buffer1_file);
    fwrite(&D_BED_MIRROR, sizeof(uint16_t),1,buffer1_file);
    fwrite(&Wait_nozz, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Wait_bed, sizeof(uint8_t),1,buffer1_file);
    fwrite(&THERMISTOR_TYPE_NOZZLE, sizeof(int8_t),1,buffer1_file);
    fwrite(&THERMISTOR_TYPE_BED, sizeof(int8_t),1,buffer1_file);
    fwrite(&HEATED_NOZZLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HEATED_BED, sizeof(uint8_t),1,buffer1_file);
    fwrite(&PID_nozz, sizeof(uint8_t),1,buffer1_file);
    fwrite(&PID_bed, sizeof(uint8_t),1,buffer1_file);
    fwrite(&DIFFERENTIAL_NOZZ, sizeof(uint8_t),1,buffer1_file);
    fwrite(&DIFFERENTIAL_BED, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_X_DURATION, sizeof(uint16_t),1,buffer1_file);
    fwrite(&HOME_Y_DURATION, sizeof(uint16_t),1,buffer1_file);
    fwrite(&HOME_Z_DURATION, sizeof(uint16_t),1,buffer1_file);
    fwrite(&HOME_X_ENABLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_Y_ENABLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_Z_ENABLE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_X_DIR, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_Y_DIR, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_Z_DIR, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_X_STATE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_Y_STATE, sizeof(uint8_t),1,buffer1_file);
    fwrite(&HOME_Z_STATE, sizeof(uint8_t),1,buffer1_file); 
    fwrite(&HOME_Z_STATE, sizeof(uint8_t),1,buffer1_file);
    /*fwrite(&X_ENABLE_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&X_STEP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&X_DIR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&X_ENDSTOP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Y_ENABLE_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Y_STEP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Y_DIR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Y_ENDSTOP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z_ENABLE_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z_STEP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z_DIR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z_ENDSTOP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z1_ENABLE_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&Z1_STEP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z1_STEP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z1_DIR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&Z1_ENDSTOP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&E_ENABLE_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&E_ENABLE_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&E_STEP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&E_DIR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&E_ENDSTOP_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&N_HEATER_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&N_SENSOR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&N_FAN_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&B_HEATER_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&B_SENSOR_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&FAN_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&SERVO1_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&SERVO2_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_1_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_3_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_5_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_7_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_9_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_2_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_4_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_6_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_8_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP1_10_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP2_1_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP2_3_PIN, sizeof(uint8_t),1,buffer1_file);
    fwrite(&EXP2_5_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&EXP2_7_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&EXP2_2_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&EXP2_4_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&EXP2_6_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&EXP2_8_PIN, sizeof(uint8_t),1,buffer1_file);
	fwrite(&EXP2_10_PIN, sizeof(uint8_t),1,buffer1_file);*/
	file_buffer_size=file_buffer_size+56;
}

void crt_file()
{
   int i,j=0,mod_file_num,centi,deca,mona;
   wchar_t temp_path[100],path[150],savefile[14]=L"//test000.bin\0";///very important the '\0' at the end to fix a bug
   FILE *pth;  
   pth=_wfopen(savepath_path,L"r, ccs=UTF-8");
   fgetws (temp_path, 100, pth);//read saved path
   fclose(pth);
   for(i=0;i<100;i++){
   	    if (temp_path[i] != L'\0'){ //end of line
         	path[j]=temp_path[i];
         	j++;
        }else{
        	break; //exit for loop
		}
   }
   centi=file_num/100;
   mod_file_num=file_num%100;
   deca=mod_file_num/10;
   mona=mod_file_num%10;
   savefile[6]= centi +'0'; //covert integer to char
   savefile[7]= deca+'0';
   savefile[8]= mona+'0';
   for(i=0;i<14;i++){
    	path[j]=savefile[i]; //append the file name to the path
    	j++;
   }
   SD_binary_file=_wfopen(path,L"wb"); //open the output file 
   fwrite(&CORE_FREQ, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&X_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&Y_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&Z_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&E_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&NOZZLE_TEMP, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&BED_TEMP, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&CYCLE_NOZZ, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&CYCLE_BED, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&P_NOZZ_MIRROR, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&I_NOZZ_MIRROR, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&D_NOZZ_MIRROR, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&P_BED_MIRROR, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&I_BED_MIRROR, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&D_BED_MIRROR, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&Wait_nozz, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&Wait_bed, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&THERMISTOR_TYPE_NOZZLE, sizeof(int8_t),1,SD_binary_file);
   fwrite(&THERMISTOR_TYPE_BED, sizeof(int8_t),1,SD_binary_file);
   fwrite(&HEATED_NOZZLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HEATED_BED, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&PID_nozz, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&PID_bed, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&DIFFERENTIAL_NOZZ, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&DIFFERENTIAL_BED, sizeof(uint8_t),1,SD_binary_file);
   
   fwrite(&HOME_X_DURATION, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&HOME_Y_DURATION, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&HOME_Z_DURATION, sizeof(uint16_t),1,SD_binary_file);
   fwrite(&HOME_X_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Y_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Z_ENABLE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_X_DIR, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Y_DIR, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Z_DIR, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_X_STATE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Y_STATE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Z_STATE, sizeof(uint8_t),1,SD_binary_file);
   fwrite(&HOME_Z_STATE, sizeof(uint8_t),1,SD_binary_file);//MUST BE WRITTEN TWICE
   storage_counter=0.00000000000052; 
}

void check_SD_file_size(double Xf, double Yf, double Zf, double Xl, double Yl, double Zl){
    if(storage_counter>=MAX_FILE_SIZE){ //create a new output file when the file size reached
        LINE(0,0,Zf,0,0,0,PARK_Z,0,PARK_FEED);
		LINE(Xf,Yf,0,0,PARK_X,PARK_Y,0,0,PARK_FEED);///GO TO PARKING BEFORE STOP
		close_SD_binary_file();
		file_num++;
        crt_file(file_num);
        LINE(0,0,PARK_Z,0,0,0,Zl,0,PARK_FEED);
        LINE(PARK_X,PARK_Y,0,0,Xl,Yl,0,0,PARK_FEED);///RETURN TO PREVIOUS POSITION
        storage_counter=0;
	}
}
    
void path_files()
{
    int i=0,j=0,f,p;
    wchar_t *appdata_path;
    appdata_path=_wgetenv(L"LOCALAPPDATA"); //https://msdn.microsoft.com/en-us/library/tehxacec(v=VS.100).aspx
    //SHGetFolderPath (0, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata_path);
	  
    while(appdata_path[i] != L'\0'){
   	    coordinates_path[j]=appdata_path[i];
		fcoordinates_path[j]=appdata_path[i];
		gc2info_path[j]=appdata_path[i];
		savepath_path[j]=appdata_path[i];
		gen_path[j]=appdata_path[i];
		gcode_path[j]=appdata_path[i];
		boxes_path[j]=appdata_path[i];
		cboxes_path[j]=appdata_path[i];
		dboxes_path[j]=appdata_path[i];
		buffer1_path[j]=appdata_path[i];
		buffer2_path[j]=appdata_path[i];
		flag_path[j]=appdata_path[i];
		startpy_path[j]=appdata_path[i];
		child_path[j]=appdata_path[i];
		fly_path[j]=appdata_path[i];
		abl_x_path[j]=appdata_path[i];
		abl_y_path[j]=appdata_path[i];
		abl_xyz_path[j]=appdata_path[i];
		pins_path[j]=appdata_path[i];
   		j++;
		i++;	   
    }
    for(f=0;f<19;f++){
    	p=j;
    	i=0;
    	if(f==0){
		    while(savepath[f][i]!=L'\0'){
             	coordinates_path[p]=savepath[f][i];
            	p++;
             	i++;
            }   		
		}
    	if(f==1){
		    while(savepath[f][i]!=L'\0'){
             	fcoordinates_path[p]=savepath[f][i];
            	p++;
             	i++;
            }   		
		}
    	if(f==2){
		    while(savepath[f][i]!=L'\0'){
             	gc2info_path[p]=savepath[f][i];
            	p++;
             	i++;
            }     		
		}		
    	if(f==3){
		    while(savepath[f][i]!=L'\0'){
             	savepath_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==4){
		    while(savepath[f][i]!=L'\0'){
             	gen_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==5){
		    while(savepath[f][i]!=L'\0'){
             	gcode_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==6){
		    while(savepath[f][i]!=L'\0'){
             	boxes_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==7){
		    while(savepath[f][i]!=L'\0'){
             	cboxes_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==8){
		    while(savepath[f][i]!=L'\0'){
             	dboxes_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==9){
		    while(savepath[f][i]!=L'\0'){
             	buffer1_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==10){
		    while(savepath[f][i]!=L'\0'){
             	buffer2_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}	
    	if(f==11){
		    while(savepath[f][i]!=L'\0'){
             	flag_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==12){
		    while(savepath[f][i]!=L'\0'){
             	startpy_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==13){
		    while(savepath[f][i]!=L'\0'){
             	child_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==14){
		    while(savepath[f][i]!=L'\0'){
             	fly_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==15){
		    while(savepath[f][i]!=L'\0'){
             	abl_x_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==16){
		    while(savepath[f][i]!=L'\0'){
             	abl_y_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==17){
		    while(savepath[f][i]!=L'\0'){
             	abl_xyz_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==18){
		    while(savepath[f][i]!=L'\0'){
             	pins_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}																										
	}
}
    
double check_units(double Fl)
{
	if (UNITS==1){
        Fl=Fl/60;
	}
	return Fl;
}

double line_accel_feed_limits(double dx,double dy,double dz,double E_DIST,double FEEDRATE)
  {
	double ux,uy,uz,ue,div,Dux,Duy,Duz,Due,ax,ay,az,ae,Dax,Day,Daz,Dae;
	dx=fabs(dx);
	dy=fabs(dy);
	dz=fabs(dz);
	if((sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2)))!=0){
		div=E_DIST/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	}else{
		div=1;
	}
	ux=dx*FEEDRATE/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uy=dy*FEEDRATE/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uz=dz*FEEDRATE/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	ue=div*FEEDRATE;
	Dux=ux-MAX_FX;
	Duy=uy-MAX_FY;
	Duz=uz-MAX_FZ;
	Due=ue-MAX_FE;
	if(Dux>0 || Duy>0 || Duz>0 || Due>0){
    	if(Dux>Duy){
    		if(Dux>Duz){
    			if(Dux>Due){
        			FEEDRATE=MAX_FX*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dx;
     			}else{
    				FEEDRATE=MAX_FE/div;
    			}
    		}else{
    			if(Duz>Due){
    				FEEDRATE=MAX_FZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
    				FEEDRATE=MAX_FE/div;
    			}
    		}
    	}else{
      		if(Duy>Duz){
    			if(Duy>Due){
    				FEEDRATE=MAX_FY*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dy;
    			}else{
     				FEEDRATE=MAX_FE/div;
    			}
    		}else{
    			if(Duz>Due){
     				FEEDRATE=MAX_FZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
      				FEEDRATE=MAX_FE/div;
    			}
    		}
     	}
    }

	ax=ux*ACCELERATION/sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2));
	ay=uy*ACCELERATION/sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2));
	az=uz*ACCELERATION/sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2));
	ae=div*ACCELERATION;
	Dax=ax-MAX_ACCX;
	Day=ay-MAX_ACCY;
	Daz=az-MAX_ACCZ;
	Dae=ae-MAX_ACCE;		
	if(Dax>0 || Day>0 || Daz>0 || Dae>0){
    	if(Dax>Day){
    		if(Dax>Daz){
    			if(Dax>Dae){
        			ACCEL_ERATION=MAX_ACCX*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/ux;
     			}else{
    				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}else{
    			if(Daz>Dae){
    				ACCEL_ERATION=MAX_ACCZ*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/uz;
    			}else{
    				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}
    	}else{
      		if(Day>Daz){
    			if(Day>Dae){
    				ACCEL_ERATION=MAX_ACCY*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/uy;
    			}else{
     				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}else{
    			if(Daz>Dae){
     				ACCEL_ERATION=MAX_ACCZ*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/uz;
    			}else{
      				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}
     	}
    }else{
    	ACCEL_ERATION=ACCELERATION;
	}
	return FEEDRATE;  	
  }
  
double arc_accel_feed_limits(double R,double ARC_DIST,double E_DIST,double FEEDRATE)
{
	double ue,div,Dux,Duy,Due,ae,Dax,Day,Dae,theta,a,ak;
	E_DIST=fabs(E_DIST);
	ak=pow(FEEDRATE,2)/R; //centripetal acceleration due to feedrate
	theta=atan(ACCELERATION/ak); //save the ratio=(angle) linear-centripetal acceleration
	a=sqrt(pow(ak,2)+pow(ACCELERATION,2)); //resultant acceleration 
	Dax=a-MAX_ACCX;
	Day=a-MAX_ACCY;	
	if(Dax>0 || Day>0){
        	if(Dax>Day){ //weaker axis is x and we must reduce (lnear and centripetal acceleration)=a must be < ax without changing the ratio
        		ACCEL_ERATION=sin(theta)*MAX_ACCX; 
        		ak=cos(theta)*MAX_ACCX;
        		FEEDRATE=sqrt(ak*R);
			}else{  //weaker axis is y and we must reduce (lnear and centripetal acceleration)=a must be <ay without changing the ratio
			    ACCEL_ERATION=sin(theta)*MAX_ACCY;
        		ak=cos(theta)*MAX_ACCY;
        		FEEDRATE=sqrt(ak*R);
			}
    }else{
    	ACCEL_ERATION=ACCELERATION; //into acceleration axes limits (good case)
	}
	
	if(ARC_DIST!=0){
	    div=E_DIST/ARC_DIST; 
    }else{
    	div=1;
	}

	ue=div*FEEDRATE;
	Dux=FEEDRATE-MAX_FX;
	Duy=FEEDRATE-MAX_FY;
	Due=ue-MAX_FE;
	if(Dux>0 || Duy>0 || Due>0){ //we dont want to exceed feedrate limits,in this case only the centripetal acceleration change => ratio change
    	if(Dux>Duy){
    		if(Dux>Due){
    			FEEDRATE=MAX_FX;
			}else{
				FEEDRATE=MAX_FE/div;
			}
        }else{
        	if(Duy>Due){
        		FEEDRATE=MAX_FY;
			}else{
				FEEDRATE=MAX_FE/div;
			}
		}
    }
    ae=div*ACCEL_ERATION;
	Dae=ae-MAX_ACCE;		
	if(Dae>0){ //check the linear acceleration limits in E axis, only linear acceleration change => ratio change
        ACCEL_ERATION=MAX_ACCE/div;
    }   
	return FEEDRATE;  
	
}  

double check_jfeed_limits(double dx,double dy,double dz,double E_DIST)
{
	double ux,uy,uz,ue,div,Dux,Duy,Duz,Due,FEEDRATE;
	dx=fabs(dx);
	dy=fabs(dy);
	dz=fabs(dz);
	if(sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))!=0){
		div=E_DIST/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	}else{
		div=1;
	}
	ux=dx*JMFEED/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uy=dy*JMFEED/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uz=dz*JMFEED/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	ue=div*JMFEED;
	Dux=ux-MAX_JFX;
	Duy=uy-MAX_JFY;
	Duz=uz-MAX_JFZ;
	Due=ue-MAX_JFE;
	if(Dux>0 || Duy>0 || Duz>0 || Due>0){
    	if(Dux>Duy){
    		if(Dux>Duz){
    			if(Dux>Due){
        			FEEDRATE=MAX_JFX*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dx;
     			}else{
    				FEEDRATE=MAX_JFE/div;
    			}
    		}else{
    			if(Duz>Due){
    				FEEDRATE=MAX_JFZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
    				FEEDRATE=MAX_JFE/div;
    			}
    		}
    	}else{
      		if(Duy>Duz){
    			if(Duy>Due){
    				FEEDRATE=MAX_JFY*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dy;
    			}else{
     				FEEDRATE=MAX_JFE/div;
    			}
    		}else{
    			if(Duz>Due){
     				FEEDRATE=MAX_JFZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
      				FEEDRATE=MAX_JFE/div;
    			}
    		}
     	}
    }else{
    	FEEDRATE=JMFEED;
	}   
	return FEEDRATE;  		
}


void curve_detection(unsigned long total_lines)
{
	int j=0,last_last_printing_move=0,last_printing_move=0,printing_move=0,breeak=0;
	double theta_temp,theta1,theta2,smart_case,last_smart_case=1,trash,FS,last_curve=0,new_curve=0,af,al,theta;
	double Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl,Sl,Pl,Rl,Tl;
	bool first_smart_line=true,next_line=false;
	FILE *coord;
	FILE *final;
    coord=_wfopen(coordinates_path,L"r");
    final=_wfopen(fcoordinates_path,L"w");
    fscanf(coord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gf,&Mf,&Xf,&Yf,&Zf,&If,&Jf,&Ef,&Ff,&Sf,&Pf,&Rf,&Tf,&trash); 
    j++;
    fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,trash);
    do{ //FIX BUG WHEN INITIAL GCODE HAS MANY NON MOVE GCODE
    	fscanf(coord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&trash);
    	if(Gl!=1 && Gl!=01 && Gl!=0 && Gl!=00 && Gl!=02 && Gl!=03 && Gl!=02 && Gl!=03){
    		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl,Sl,Pl,Rl,Tl,trash);
		}
		j++; 
	}while(Gl!=1 && Gl!=01 && Gl!=0 && Gl!=00 && Gl!=02 && Gl!=03 && Gl!=02 && Gl!=03 && j<total_lines);
	if(Gl==1 || Gl==01 || Gl==0 || Gl==00){
        if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
        	last_printing_move=1;
    	}else{
			last_printing_move=0;
    	}
    	last_curve=1;
    	theta1 = curve_lines_angles(Xf,Yf,Xl,Yl,Gl);
    }else if(Gl == 02 || Gl == 03 || Gl == 02|| Gl == 03){
       	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
       		printing_move=1;
   		}else{
   			printing_move=0;
   		}
		theta1 = curve_lines_angles(Il,Jl,Xl,Yl,Gl);
		last_curve=1;	    		
	}else{
		last_printing_move=0;
    	last_curve=0;	
	}
	if(Gl==91){// G91 Relative
		ABSOLUTE_POSITIONING=0;
		E_ABSOLUTE_POSITIONING=0;
	}
	if(Gl==90){// G91 Relative
		ABSOLUTE_POSITIONING=1;
		E_ABSOLUTE_POSITIONING=1;
	}
	if(Ml==82){// M82 E Absolute
		E_ABSOLUTE_POSITIONING=1;
	}
	if(Ml==83){// M83 E Absolute
		E_ABSOLUTE_POSITIONING=0;
	}
	Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    while(j<total_lines){
    	j++;
        fscanf(coord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&trash);    
    	if(Gl==91){// G91 Relative
    	    Xl=0; Yl=0; Zl=0; El=0;
    		ABSOLUTE_POSITIONING=0;
    		E_ABSOLUTE_POSITIONING=0;
    	}
    	if(Gl==90){// G90 Absolute
    	    Xl=Xf; Yl=Yf; Zl=Zf; El=Ef;
    		ABSOLUTE_POSITIONING=1;
    		E_ABSOLUTE_POSITIONING=1;
    	}
		if(Ml==82){// M82 E Absolute
		    El=0;
			E_ABSOLUTE_POSITIONING=1;
		}
		if(Ml==83){// M83 E Relative
		    El=Ef;
			E_ABSOLUTE_POSITIONING=0;
		}
		if(Gl==92 && ABSOLUTE_POSITIONING==0){ //G92 set position
			Xl=Xl-Xf;
            Yl=Yl-Yf;
            Zl=Zl-Zf;
		}
		if(Gl==92 && E_ABSOLUTE_POSITIONING==0){
			El=El-Ef;
		}
        if(ABSOLUTE_POSITIONING==0){
            Xl=Xf+Xl;
            Yl=Yf+Yl;
            Zl=Zf+Zl;
		}
	    if(E_ABSOLUTE_POSITIONING==0){
            El=Ef+El;
		}		
		if((Gl==0 || Gl==1 || Gl==2 || Gl==3 || Gl==00 || Gl==01 || Gl==02 || Gl==03) && (Gf==0 || Gf==1 || Gf==2 || Gf==3 || Gf==00 || Gf==01 || Gf==02 || Gf==03)){
			if(Gl==1){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
		    	theta2 = curve_lines_angles(Xf,Yf,Xl,Yl,Gl);
	    	}
	    	if(Gl == 2 || Gl == 3 || Gl == 02|| Gl == 03){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
				theta2 = curve_lines_angles(Il,Jl,Xf,Yf,Gl);
				theta_temp = curve_lines_angles(Il,Jl,Xl,Yl,Gl);	    		
			}
        	if(last_printing_move==printing_move && Fl==Ff){
                theta=fabs(theta2-theta1);
                if(theta>180){
                 	theta=360-theta;
		     	}
                if(theta<=ANGLE_THRESHOLD && ((Xf!=Xl || Yf!=Yl))){
		    	  new_curve=0;
		        }else{
	    	       new_curve=1;
		     	}
	    	}else{
	    	     new_curve=1;  	
			}
		    fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
		    last_last_printing_move=last_printing_move;
		    last_printing_move=printing_move;
		    last_curve=new_curve;
		    theta1=theta2;
		    if(Gl==2 || Gl==3 || Gl==02 || Gl==03){
		    	theta1=theta_temp;
			}
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle
    	}
		if((Gf==0 || Gf==1 || Gf==2 || Gf==3 || Gf==00 || Gf==01 || Gf==02 || Gf==03) && ((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=01 && Gl!=00 && Gl!=02 && Gl!=03))){
        	printing_move=0;
        	new_curve=0;
    		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
    		last_printing_move=printing_move;
    		last_curve=new_curve;
    		theta1=theta2;
    		if(Gl==2 || Gl==3 || Gl==02 || Gl==03){
    		    theta1=theta_temp;
    		}
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle    		
		}
		if(((Gf!=1 && Gf!=0 && Gf!=2 && Gf!=3) || (Gf!=01 && Gf!=00 && Gf!=02 && Gf!=03)) && (Gl==0 || Gl==1 || Gl==2 || Gl==3 || Gl==00 || Gl==01 || Gl==02 || Gl==03)){
			if(Gl==0 || Gl==00 || Gl==1 || Gl==01){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
		    	theta2 = curve_lines_angles(Xf,Yf,Xl,Yl,Gl);
	    	}
	    	if(Gl == 02 || Gl == 03 || Gl == 02|| Gl == 03){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
				theta2 = curve_lines_angles(Il,Jl,Xf,Yf,Gl);
				theta_temp = curve_lines_angles(Il,Jl,Xl,Yl,Gl);	    		
			}
			new_curve=1;
		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
		last_printing_move=printing_move;
		last_curve=new_curve;
		theta1=theta2;
		if(Gl==02 || Gl==03 || Gl==2 || Gl==3){
		    theta1=theta_temp;
		}
        Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle		
		}
    	
    }
    fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
	fclose(coord);
	fclose(final);
}


void curve_length(unsigned long total_lines)
{
	int j=0,loopa=0,a_f,a_l;
	double Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl=0,Sl,Pl,Rl,Tl,local_case=0,gen_feed,dx,dy,dz,de,gen_distance=0,reset_dist=1;
	bool first_time_tcoord=true,first_write=true;
	double xmin,ymin,zmin,emin,x1,x2,y1,y2,z1,z2,e1,e2;
	double x1_f,x2_f,y1_f,y2_f,k,l,R,theta_first,theta_last,dtheta;
	FILE *fcoord;
	FILE *gen;
	FILE *coord;
	
    fcoord=_wfopen(fcoordinates_path,L"r");
    gen=_wfopen(gen_path,L"w");
    
    while(j<total_lines){
    	if(first_time_tcoord){
    		first_time_tcoord=false;
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gf,&Mf,&Xf,&Yf,&Zf,&If,&Jf,&Ef,&Ff,&Sf,&Pf,&Rf,&Tf,&local_case);
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
    	    while(local_case!=1){
        	    Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
        	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
        	    j++;
			}
		}
		if((Gl==0 || Gl==00 || Gl==01 || Gl==02 || Gl==03 || Gl==1 || Gl==2 || Gl==3) && first_write==true){
			local_case=0;
			first_write=false;
		}
    	while(local_case==0 && (Gl==00  || Gl==01 || Gl==03 || Gl==02 || Gl==0  || Gl==1 || Gl==3 || Gl==2) && j<total_lines){
    		if(Gl==1 || Gl==01 || Gl==0 || Gl==00){
            	xmin=1/STPU_X;
                ymin=1/STPU_Y;
            	zmin=1/STPU_Z;
             	emin=1/STPU_E;
                x1=round(Xf/xmin); y1=round(Yf/ymin); z1=round(Zf/zmin); e1=round(Ef/emin);
                x2=round(Xl/xmin); y2=round(Yl/ymin); z2=round(Zl/zmin); e2=round(El/emin);
                dx = abs(x2 - x1);
                dy = abs(y2 - y1); 
                dz = abs(z2 - z1); 
                de = abs(e2 - e1);
                dx = dx*xmin;
                dy = dy*ymin;
                dz = dz*zmin;
                de = de*emin;
                if(dx!=0||dy!=0||dz!=0){
                	gen_distance=gen_distance+sqrt((pow(dx,2))+(pow(dy,2))+(pow(dz,2)));
    			}else if(de!=0){
    				gen_distance=gen_distance+fabs(de);
    			}
		    }
		    if(Gl==02 || Gl==03 || Gl==2 || Gl==3){
		    	k=Il;
		    	l=Jl;
                x1_f=Xf-k; x2_f=Xl-k;
                y1_f=Yf-l; y2_f=Yl-l;  
				k=0; l=0;      
             	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
            	a_f=casequartile(x1_f,y1_f,k,l,1);
            	a_l=casequartile(x2_f,y2_f,k,l,2);
             	theta_first=theta_func(x1_f,y1_f,k,l,a_f);
            	theta_last=theta_func(x2_f,y2_f,k,l,a_l);
            	dtheta=fabs(theta_last-theta_first);
        		if(dtheta>pi/2.0){
        			dtheta=2.0*pi-dtheta;
        		}
        		double THETA_STEP;
        		double ARC_DIST=dtheta*R; ///calculate total distance of the ar
        		ARC_DIST=roundf(ARC_DIST*100)/100;
         		gen_distance=gen_distance + ARC_DIST; ///calculate total distance of the ar		    	
			}
		   
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
    	    reset_dist=0;
    	    loopa=1;
    	    gen_feed=Ff;
    	}
    	if ((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=00 && Gl!=01 && Gl!=02 && Gl!=03)){
    		Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
		}
    	if(loopa==1){
    	   fprintf(gen,"%lf " "%lf\n",gen_distance,gen_feed);
		   gen_distance=0;
		   loopa=0;
		   local_case=0;
	    }else if(Gl==0 || Gl==00 || Gl==01 || Gl==02 || Gl==03 || Gl==1 || Gl==2 || Gl==3){
    		if(Gl==0 || Gl==00 || Gl==01 || Gl==1){
            	xmin=1/STPU_X;
                ymin=1/STPU_Y;
            	zmin=1/STPU_Z;
             	emin=1/STPU_E;
                x1=round(Xf/xmin); y1=round(Yf/ymin); z1=round(Zf/zmin); e1=round(Ef/emin);
                x2=round(Xl/xmin); y2=round(Yl/ymin); z2=round(Zl/zmin); e2=round(El/emin);
                dx = abs(x2 - x1);
                dy = abs(y2 - y1); 
                dz = abs(z2 - z1); 
                de = abs(e2 - e1);
                dx = dx*xmin;
                dy = dy*ymin;
                dz = dz*zmin;
                de = de*emin;
                if(dx!=0||dy!=0||dz!=0){
                	gen_distance=gen_distance+sqrt((pow(dx,2))+(pow(dy,2))+(pow(dz,2)));
    			}else if(de!=0){
    				gen_distance=gen_distance+fabs(de);
    			}
		    }
		    if(Gl==02 || Gl==03 || Gl==2 || Gl==3){
		    	k=Il;
		    	l=Jl;
                x1_f=Xf-k; x2_f=Xl-k;
                y1_f=Yf-l; y2_f=Yl-l;
             	double Xmin=1/STPU_X;
                double Ymin=1/STPU_Y;
            	double Emin=1/STPU_E;    
				k=0; l=0;      
             	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
            	a_f=casequartile(x1_f,y1_f,k,l,1);
            	a_l=casequartile(x2_f,y2_f,k,l,2);
             	theta_first=theta_func(x1_f,y1_f,k,l,a_f);
            	theta_last=theta_func(x2_f,y2_f,k,l,a_l);
            	dtheta=fabs(theta_last-theta_first);
        		if(dtheta>pi/2.0){
        			dtheta=2.0*pi-dtheta;
        		}
        		double THETA_STEP;
        		double ARC_DIST=dtheta*R; ///calculate total distance of the ar
        		ARC_DIST=roundf(ARC_DIST*100)/100;
         		gen_distance=gen_distance + ARC_DIST; ///calculate total distance of the ar		    	
			}
        	if((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=01 && Gl!=00 && Gl!=02 && Gl!=03)){
        		gen_distance=0;
	    	}
	    	gen_feed=Fl;
        	fprintf(gen,"%lf " "%lf\n",gen_distance,gen_feed);
    		gen_distance=0;
    		Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    		fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
    	}
	}
	fclose(fcoord);
	fclose(gen);
}


double curve_lines_angles(double xf,double yf,double xl,double yl, double Gl)
{
	int aadj;
	double theta_adj;
	
	if(Gl==0 || Gl==00 || Gl==01 || Gl==1){
    	if(fabs(xl-xf==0) && fabs(yl-yf==0)){
      		return 180;
    	}
        aadj=casequartile(xl,yl,xf,yf,1);
        theta_adj=theta_func(xl,yl,xf,yf,aadj)*180/pi;
        return theta_adj;
    }else{
        aadj=casequartile(xl,yl,xf,yf,1);
        theta_adj=theta_func(xl,yl,xf,yf,aadj)*180/pi;
        if(aadj==1){
        	if(Gl==2){
             	theta_adj=270+theta_adj;	
			}else if(Gl==3){
				theta_adj=90+theta_adj;
			}
		}else if(aadj==2){
        	if(Gl==2){
        		theta_adj=theta_adj-90;
			}else if(Gl==3){
				theta_adj=theta_adj+90;
			}			
		}else if(aadj==3){
        	if(Gl==2){
        		theta_adj=theta_adj-90;
			}else if(Gl==3){
				theta_adj=theta_adj+90;
			}			
		}else if(aadj==4){
        	if(Gl==2){
        		theta_adj=theta_adj-90;
			}else if(Gl==3){
				theta_adj=theta_adj-270;
			}			
		}
        return theta_adj;
	}
}
  
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
}

void hidecursor() //    https://stackoverflow.com/questions/30126490/how-to-hide-console-cursor-in-c
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

unsigned long gc2info(double flag_num)
{
    #define arrays_size 13
    char string [200],letters[arrays_size]={'G','M','X','Y','Z','I','J','E','F','S','P','R','T'},buf[10]; 
	unsigned long i=0,j,g,poslet=0,total_lines=0,n=0,dex=0,temp_pos=0;
    double line1[arrays_size],line2[arrays_size],trash=0;
    bool first_line1=true,first_line2=true,notfound=true,found_axis=false,write_value=false,write_axis_num=true,home=false;
    int threshold_pos=7,cur=0,emb=1;
	FILE *fp;
	fp=_wfopen(gcode_path,L"r");
	FILE *g1;
	FILE *g2;
    g1=_wfopen(gc2info_path,L"w");	//the results are written in this fille
    fprintf(g1,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash);
    total_lines++;
    while (fgets(string, 200, fp) != NULL){//TILL THE END OF FILE
	   if (string[0]=='G' || string[0]=='M'){
	        total_lines++;
			cur=0;	
			while (cur<emb){
			cur++;							  
		    for(poslet=0;poslet<arrays_size;poslet++){// check for each letter		  
		        while (string[i] != '\0' && string[i] != ';' ){ //till the end of line
		            if ((string[i]) == (letters[poslet])){ //
			            notfound=false; //=found letter
			            j=i+1;
			            write_value=false;
			            while(string[j] != ' ' && string[j] != '\0' && string[j] != ';' && string[j] != '\n'){ //until next character
			            	write_value=true; //number found
						    if (string[j] != '[' && string[j] !=']'){ //extract the number
			                    fprintf(g1,"%c",string[j]);
			                }
						    j++;	
					    }
					    if(write_value==false){ //no number extracted LETTERS XYZE 
                           //printf("%s\n","AXIS FOUND");
                           fprintf(g1,"%lf",axis_num); 
						}
			        }
			        i++;
				}
				if(notfound==true){ //if letter not found
		            if(string[0]=='G' && string[1]=='2' && string[2]=='8' && (letters[poslet]=='X' || letters[poslet]=='Y' || letters[poslet]=='Z')){ //G28
		                home=true;
		            	temp_pos=0;
		            	while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            		 if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z'){
		            		 	write_axis_num=false;
							 }
							 temp_pos++;
		            	}
		            	if(write_axis_num==true){
		            		fprintf(g1,"%lf",axis_num);
		            		write_axis_num=true;
						}else{
							fprintf(g1,"%lf",flag_num);
						}
					}else if((string[0]=='M' && string[1]=='2' && string[2]=='0' && string[3]=='5')){ //M205
					       if(letters[poslet]=='S' || letters[poslet]=='J'){
					       	fprintf(g1,"%lf",axis_num);
						}else{
							fprintf(g1,"%lf",flag_num);
						}
					}else if(string[0]=='G' && string[1]=='9' && string[2]=='2' && (letters[poslet]=='X' || letters[poslet]=='Y' || letters[poslet]=='Z' || letters[poslet]=='E')){ //G92
		            	temp_pos=0;
		            	while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            		 if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z' || letters[poslet]=='E'){
		            		 	write_axis_num=false;
							 }
							 temp_pos++;
		            	}
		            	if(write_axis_num==true){
		            		fprintf(g1,"%lf",axis_num);
		            		write_axis_num=true;
						}else{
							fprintf(g1,"%lf",flag_num);
						}							
					}else if(string[0]=='M' && ((string[1]=='1' && string[2]=='7') || (string[1]=='1' && string[2]=='8') || (string[1]=='8' && string[2]=='4')) && (letters[poslet]=='X' || letters[poslet]=='Y' || letters[poslet]=='Z' || letters[poslet]=='E')){ //G92
		            	temp_pos=0;
		            	while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            		 if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z' || string[temp_pos]=='E'){
		            		 	write_axis_num=false;
							 }
							 temp_pos++;
		            	}
		            	if(write_axis_num==true){
		            		fprintf(g1,"%lf",axis_num);
		            		write_axis_num=true;
						}else{
							fprintf(g1,"%lf",flag_num);
						}						
					}else{
							fprintf(g1,"%lf",flag_num);
					}			        
			    }	
	            j=0;
		        i=0;
		        notfound=true;
		        found_axis=false;
		        fprintf(g1,"%c",' ');
 	        }
 	        if (home==true){
				  emb=2;
				  home=false;
				  strcpy(string,"G92 X"); 
				  gcvt(T0_X_OFFSET, 7, buf);
				  strcat(string, buf);
				  strcat(string, " Y");
				  gcvt(T0_Y_OFFSET, 7, buf);
				  strcat(string, buf);
				  strcat(string, " Z0\n");
				  total_lines++;
			    }else{
			    	emb=1;
			}
 	        }
 	        first_line1=false;
            fprintf(g1, "%c\n",' ' );
        }
        if(string[0]=='G' && string[1]=='9' && string[2]=='1' ){
             ABSOLUTE_POSITIONING=0;
		}
        if(string[0]=='G' && string[1]=='9' && string[2]=='0' ){
             ABSOLUTE_POSITIONING=1;
		}
        if(string[0]=='G' && ((string[1]=='2' || string[1]=='3') || ((string[1]=='0' && string[1]=='2') || (string[2]=='0' && string[2]=='3'))) && ABSOLUTE_POSITIONING==0){
            //printf("%s\n","G2/G3 does not support relative move mode");
            exit(0);
		}
    }
    fclose(g1);
    fclose(fp);
    g1=_wfopen(gc2info_path,L"r");
    g2=_wfopen(coordinates_path,L"w");   
    i=0;
    for(j=1;j<total_lines;j++){
		if(first_line2==true){
		    first_line2=false;
	        fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&line1[i],&line1[i+1],&line1[i+2],&line1[i+3],&line1[i+4],&line1[i+5],\
		           &line1[i+6],&line1[i+7],&line1[i+8],&line1[i+9],&line1[i+10],&line1[i+11],&line1[i+12]);
				  	
	        fprintf(g2,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",line1[i],line1[i+1],line1[i+2],line1[i+3],line1[i+4],line1[i+5],\
	    	        line1[i+6],line1[i+7],line1[i+8],line1[i+9],line1[i+10],line1[i+11],line1[i+12],trash);	
		}
        fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&line2[i],&line2[i+1],&line2[i+2],&line2[i+3],&line2[i+4],&line2[i+5],\
		       &line2[i+6],&line2[i+7],&line2[i+8],&line2[i+9],&line2[i+10],&line2[i+11],&line2[i+12]);
	    if(line2[0]==91){ //Relative positioning
	    	ABSOLUTE_POSITIONING=0;
			E_ABSOLUTE_POSITIONING=0;
			threshold_pos=7;
		}
	    if(line2[0]==90){ //Absolute positioning
	    	ABSOLUTE_POSITIONING=1;
			E_ABSOLUTE_POSITIONING=1;
			threshold_pos=7;
		}
	    if(line2[1]==82){ //Absolute E positioning
			E_ABSOLUTE_POSITIONING=1;
			threshold_pos=6;
		}
	    if(line2[1]==83){ //Relative E positioning
			E_ABSOLUTE_POSITIONING=0;
			threshold_pos=7;
		}
		for(g=2;g<arrays_size;g++){ ////NOT SURE for this
		    if (line2[g]==flag_num && ABSOLUTE_POSITIONING==1 && E_ABSOLUTE_POSITIONING==0 && g==threshold_pos){ //Only E in relative
	 		   line2[g]=0;
		    }
	 	   if (line2[g]==flag_num && (ABSOLUTE_POSITIONING==1 || (ABSOLUTE_POSITIONING==0 && g>threshold_pos))){ //Absololute or Realtive without XYZE
	 		   line2[g]=line1[g];
		    }
		    if (line2[g]==flag_num && ABSOLUTE_POSITIONING==0 && g<=threshold_pos){ //Relative positioning for XYZE
	 		   line2[g]=0;
		    }
		    
	    }
	    fprintf(g2,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",line2[i],line2[i+1],line2[i+2],line2[i+3],line2[i+4],line2[i+5],\
	           line2[i+6],line2[i+7],line2[i+8],line2[i+9],line2[i+10],line2[i+11],line2[i+12],trash);
		for(g=0;g<arrays_size;g++){
			if(line2[g]!= axis_num){
				line1[g]=line2[g];
			}
		} 
    }
    fclose(g1);
    fclose(g2);	
    return total_lines;
}
 
void LINE(double xf, double yf, double zf, double ef, double xl, double yl, double zl, double el, double FEEDRATE)
	{

    int dx=0,dy=0,dz=0,de=0,xs=0,ys=0,zs=0,es=0,p1=0,p2=0,p3=0,x1_last,y1_last,z1_last,e1_last,period;
    int stepx=0,stepy=0,stepz=0,stepe=0,x1=0,y1=0,z1=0,e1=0,x2=0,y2=0,z2=0,e2=0;
    double xmin,ymin,zmin,emin,LINE_DIST,E_DIST,LINE_STEP=0,XY_PLANE_RAD,ZX_PLANE_RAD,ZY_PLANE_RAD;

    
    if(XY_PLANE!=0){
    	XY_PLANE_RAD=XY_PLANE*pi/180;
    	yl=yl-(sin(XY_PLANE_RAD)*xl);
    	yf=yf-(sin(XY_PLANE_RAD)*xf);
    	xl=cos(XY_PLANE_RAD)*xl;
        xf=cos(XY_PLANE_RAD)*xf;    	
	}
    if(ZX_PLANE!=0 && (zl-zf)!=0){
    	ZX_PLANE_RAD=ZX_PLANE*pi/180;
    	zl=zl+(zl*(1-cos(ZX_PLANE_RAD)));
        zf=zf+(zf*(1-cos(ZX_PLANE_RAD)));
    	xl=sin(ZX_PLANE_RAD)*zl;
    	xf=sin(ZX_PLANE_RAD)*zf;    	
	}
    if(ZY_PLANE!=0 && (zl-zf)!=0){
    	ZY_PLANE_RAD=ZY_PLANE*pi/180;
    	zl=zl+(zl*(1-cos(ZY_PLANE_RAD)));
        zf=zf+(zf*(1-cos(ZY_PLANE_RAD)));
    	yl=sin(ZY_PLANE_RAD)*zl;
    	yf=sin(ZY_PLANE_RAD)*zf;  	
	}
    dx=xl-xf;
    dy=yl-yf;
    dz=zl-zf;
    de=el-ef;
    if(CURVE_DETECTION==0){
    	FEEDRATE=line_accel_feed_limits(dx,dy,dz,de,FEEDRATE); //Only working when CURVE_DETECTION is disabled
	}else{
		ACCEL_ERATION=ACCELERATION;
	}
	
    //printf("%s %f\n","feed",FEEDRATE);
	xmin=1/STPU_X;
    ymin=1/STPU_Y;
	zmin=1/STPU_Z;
	emin=1/STPU_E;
    x1=round(xf/xmin); y1=round(yf/ymin); z1=round(zf/zmin); e1=round(ef/emin);
    x2=round(xl/xmin); y2=round(yl/ymin); z2=round(zl/zmin); e2=round(el/emin);
    ABL_X = x2/STPU_X;
    ABL_Y = y2/STPU_Y;
    x1_last=x1; y1_last=y1; z1_last=z1; e1_last=e1; ////BUG FIX
    dx = abs(x2 - x1);
    dy = abs(y2 - y1); 
    dz = abs(z2 - z1); 
    de = abs(e2 - e1);
    period=1000/CORE_FREQ;
	tmin=period*pow(10,-6);
    LINE_DIST=sqrt((pow(dx*xmin,2))+(pow(dy*ymin,2))+(pow(dz*zmin,2)));
    //printf("%s %f\n","LINE_traj",trajectory_POINT);
    //printf("%s %f\n","LINE",LINE_DIST);
    
    E_DIST=fabs(el-ef);
    if(LINE_DIST==0 && E_DIST==0){ //for safety
    	return;
	}
	if(LINE_DIST==0 && E_DIST!=0){
        LINE_DIST=E_DIST;
        E_DIST=0;
	}
	
	if(embedded_line==false){
    	if(CURVE_DETECTION==1){
    	    if(new_CURVE==1){
    	    	trajectory_POINT=0; //reset
    	    	if(JMFEED>=FEEDRATE){
                	cu=FEEDRATE;//for safety reasons
            		state=10; //NO ACCElERATION or DECELERATION	    		
    			}else{
    				//printf("%f %f\n",gen_DISTANCE,FEEDRATE);
    				time_momments(gen_DISTANCE,FEEDRATE);
    			}
    		}
    	}else{
        	if(JMFEED>=FEEDRATE){ //If this case is true then motion occurs with NO ACCElERATION or DECELERATION
            	cu=check_jfeed_limits(xl-xf,yl-yf,zl-zf,E_DIST);
        		state=10; //NO ACCElERATION or DECELERATION
        	}else{
    			time_momments(LINE_DIST,FEEDRATE);		
    		}
            trajectory_POINT=0; //reset		
    	}
    }
///////******  BRESENHAM LINE ALGORITHM    ****/////////
    if (x2 > x1){
        xs = 1;
    }else{ 
        xs = -1;
    }
    if (y2 > y1){ 
        ys = 1;
    }else{ 
        ys = -1;
    }
    if (z2 > z1){ 
        zs = 1;
    }else{ 
        zs = -1;
    }
    if (e2 > e1){
    	es=1;
	}else{
		es=-1;
	}
    if (dx >= dy && dx >= dz && dx>=de){  	
	    LINE_STEP=LINE_DIST/dx;
		if(embedded_line==true){
			LINE_STEP=0;
		}
        p1 = 2 * dy - dx; 
        p2 = 2 * dz - dx;
		p3 = 2 * de - dx; 
        while (x1 != x2){ 
            x1 += xs; 
            if (p1 >= 0){ 
                y1 += ys;
                p1 -= 2 * dx;
			}
            if (p2 >= 0){   
                z1 += zs;
                p2 -= 2 * dx; 
            }
            if (p3 >= 0){
            	e1 += es;
            	p3 -= 2 * dx;
			}
            p1 += 2 * dy; 
            p2 += 2 * dz; 
            p3 += 2 * de;
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point 
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        }
    }else if (dy >= dx && dy >= dz && dy >= de){ 
	    LINE_STEP=LINE_DIST/dy;
		if(embedded_line==true){
			LINE_STEP=0;
		}
        p1 = 2 * dx - dy;
        p2 = 2 * dz - dy;
		p3 = 2 * de - dy; 
        while (y1 != y2){ 
            y1 += ys;
            if (p1 >= 0){ 
                x1 += xs; 
                p1 -= 2 * dy; 
            }
            if (p2 >= 0){ 
                z1 += zs; 
                p2 -= 2 * dy; 
            }
            if (p3 >= 0){
            	e1 += es;
            	p3 -= 2 * dy;
			}
            p1 += 2 * dx; 
            p2 += 2 * dz;
			p3 += 2 * de; 
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;			
        } 
    }else if (dz >= dx && dz >= dy && dz >= de){
	    LINE_STEP=LINE_DIST/dz;         
        p1 = 2 * dy - dz; 
        p2 = 2 * dx - dz; 
        p3 = 2 * de - dz;
        while (z1 != z2){ 
            z1 += zs;
            if (p1 >= 0){ 
                y1 += ys; 
                p1 -= 2 * dz;
			}
            if (p2 >= 0){ 
                x1 += xs; 
                p2 -= 2 * dz; 
            }
            if (p3 >= 0){
            	e1 += es;
            	p3 -= 2 * dz;
			}
            p1 += 2 * dy;
            p2 += 2 * dx;
			p3 += 2 * de; 
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        }
    }else{
    	LINE_STEP=LINE_DIST/de;
        p1 = 2 * dx - de; 
        p2 = 2 * dy - de; 
        p3 = 2 * dz - de;
        while (e1 != e2){ 
            e1 += es;
            if (p1 >= 0){ 
                x1 += xs; 
                p1 -= 2 * de;
			}
            if (p2 >= 0){ 
                y1 += ys; 
                p2 -= 2 * de; 
            }
            if (p3 >= 0){
            	z1 += zs;
            	p3 -= 2 * de;
			}
            p1 += 2 * dx;
            p2 += 2 * dy;
			p3 += 2 * dz; 
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        } 	
	}	
 }

void ARC(bool clockwise,double k,double l,double x1_f,double y1_f,double Ef,double x2_f,double y2_f,double El,double FEEDRATE)
   {
        
    double R,a_f,a_l,theta_first,theta_last,dtheta,ARC_DIST,E_DIST,Xmin,Ymin,Emin,THETA_STEP,theta,DX,DY,x,y,Xlast,Ylast,emin;
    double temp,theta_prev,L=0,L_last,DL,L_STEP,FEED_TEMP,L_prev=0;
    int b,stepx=0,stepy=0,stepe=0,e_step=0,period;
    bool next_point_x=false,next_point_y=false,next_point_e=false;
        double yy,xx,check=0;
        int cor_counter=0;
		period=1000/CORE_FREQ;
    	tmin=period*pow(10,-6);
        x1_f=x1_f-k; x2_f=x2_f-k;
        y1_f=y1_f-l; y2_f=y2_f-l;
        yy=y1_f*STPU_Y;
        xx=x1_f*STPU_X;
        k=0; l=0;
     	Xmin=1/STPU_X;
        Ymin=1/STPU_Y;
    	Emin=1/STPU_E;         
     	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
      	a_f=casequartile(x1_f,y1_f,k,l,1);
     	a_l=casequartile(x2_f,y2_f,k,l,2);
     	theta_first=theta_func(x1_f,y1_f,k,l,a_f);
     	theta_last=theta_func(x2_f,y2_f,k,l,a_l);
     	dtheta=fabs(theta_last-theta_first);
 		if(dtheta>pi/2.0){
			dtheta=2.0*pi-dtheta;
		}
		ARC_DIST=dtheta*R; ///calculate total distance of the ar
		if(ARC_DIST==0){ //FIX A BUG
			return;
		}
		E_DIST=El-Ef;
		FEEDRATE=arc_accel_feed_limits(R,ARC_DIST,E_DIST,FEEDRATE);
		if(CURVE_DETECTION==1){
    	    if(new_CURVE==1){
    	    	trajectory_POINT=0; //reset
    	    	if(JMFEED>=FEEDRATE){
                	cu=FEEDRATE;//for safety reasons
            		state=10; //NO ACCElERATION or DECELERATION	    		
    			}else{
    				time_momments(gen_DISTANCE,FEEDRATE);
    			}
    		}
    	}else{
    		trajectory_POINT=0; //reset	
	        if(JMFEED>=FEEDRATE){
    	    	if(E_DIST!=0){
    	    		FEED_TEMP=min_n(MAX_JFX,MAX_JFY);
    	    		cu=min_n(FEED_TEMP,MAX_JFE);
    			}else{
    				cu=min_n(MAX_JFX,MAX_JFY);
    			}
    		    state=10;
    	    }else{
    		    time_momments(ARC_DIST,FEEDRATE);
     	    }
    	}
		if(Xmin<=Ymin){
            emin=(ARC_DIST*Emin)/fabs(E_DIST);
			if(clockwise==true){
				if(Xmin<=emin){
			        THETA_STEP=-dtheta*Xmin/ARC_DIST;
				}else{
					THETA_STEP=-dtheta*emin/ARC_DIST;
				}
    		}else{
				if(Xmin<=emin){
			        THETA_STEP=dtheta*Xmin/ARC_DIST;
				}else{
					THETA_STEP=dtheta*emin/ARC_DIST;
				}
    		}
		}else{
            emin=(ARC_DIST*Emin)/fabs(E_DIST);
			if(clockwise==true){
				if(Ymin<=emin){
			        THETA_STEP=-dtheta*Ymin/ARC_DIST;
				}else{
					THETA_STEP=-dtheta*emin/ARC_DIST;
				}
    		}else{
				if(Ymin<=emin){
			        THETA_STEP=dtheta*Ymin/ARC_DIST;
				}else{
					THETA_STEP=dtheta*emin/ARC_DIST;
				}
    		}
		}
		b=round(pi/(2.0*THETA_STEP));//BUG FIX...VALUE DRIFT BECAUSE OF PI
		THETA_STEP=pi/(2.0*b);
		theta=theta_first+THETA_STEP;
		theta_prev=theta;
		if (theta_first>theta_last && clockwise==false){
		    if(theta_last==0){
		    	theta_last=2*pi;
			}else{
    			temp=theta_last;
    			theta_last=2*pi;
    		}
		}else if(theta_first<theta_last && clockwise==true && y2_f!=0){
    			temp=theta_last;
    			theta_last=0;	
		}
		Xlast=x1_f;
		Ylast=y1_f;	
        theta_last=theta_last+(THETA_STEP/1.2); ///BUG FIX FOR THE LAST POINT
        L_STEP=Xmin;
		ARC_DIST = roundf(ARC_DIST*100)/100+(L_STEP/1.2); ///BUG FIX FOR THE LAST POINT
        L+=L_STEP;
		trajectory_POINT=trajectory_POINT+L_STEP;
		while(L<=ARC_DIST){
			x=R*cos(theta);
			y=R*sin(theta);	
			DX=x-Xlast;
			DY=y-Ylast;
			DL=L-L_last;
			if (fabs(DX)>=(Xmin/2.0)){
				next_point_x=true;
				if(DX>=0){
	    			x1_f += Xmin;
	    		}else{
	    			x1_f -= Xmin;
				}
			}
			if (fabs(DY)>=(Ymin/2.0)){
				next_point_y=true;
				if(DY>=0){
    				y1_f += Ymin;
        		}else{
        			y1_f -= Ymin;
				}
			}
			if(fabs(DL)>=(emin/2.0)){
				next_point_e=true;
				if(E_DIST>=0){
					e_step=1;
				}else{
				    e_step=-1;
				}
			}
			if(next_point_x==true || next_point_y==true || next_point_e==true){
                if (DX!=0 && next_point_x==true){ if(DX>0){stepx=1;}else{stepx=-1;}next_point_x=false;Xlast=x1_f;}else{ stepx=0; }
                if (DY!=0 && next_point_y==true){ if(DY>0){stepy=1;}else{stepy=-1;}next_point_y=false;Ylast=y1_f;}else{ stepy=0; }
                if (DL!=0 && next_point_e==true){ stepe=e_step;next_point_e=false;L_last+=emin;}else{ stepe=0; }
                wr2bin(stepx,stepy,0,stepe,trajectory_POINT);
				yy=yy+stepy; 
				xx=xx+stepx;
			}
			trajectory_POINT=trajectory_POINT+L_STEP;
			L+=L_STEP;
			theta_prev=theta;
			theta += THETA_STEP;
			if(clockwise==false){
    			if (theta>(pi/2.0) && theta_prev<(pi/2.0)){
    				theta=(pi/2.0);
    		    }else if(theta>pi && theta_prev<pi){
    				theta=(pi);
    			}else if(theta>(3.0*pi/2.0) && theta_prev<(3.0*pi/2.0)){
    				theta=(3*pi/2.0);				
    			}else if(theta>(2*pi) && y2_f!=0){
    				theta=0;
    				theta_last=temp+(THETA_STEP/2.0001);
    			}
	    	}else{
    			if (theta<(pi/2.0) && theta_prev>(pi/2.0)){
    				theta=(pi/2.0);
    		    }else if(theta<pi && theta_prev>pi){
      				theta=(pi);
    			}else if(theta<(3.0*pi/2.0) && theta_prev>(3.0*pi/2.0)){
    				theta=(3*pi/2.0);				
    			}else if(theta<0 && y2_f!=0){
    				theta=2*pi;
    				theta_last=temp+(THETA_STEP/2.0001);
    			}	    		
			}
		}
		trajectory_POINT -=L_STEP;
		//trajectory_POINT=trajectory_POINT-L_STEP;
		//printf("%s %f\n","TRAJECT",trajectory_POINT);
		xx=xx/STPU_X;
		yy=yy/STPU_Y;
		if((xx-x2_f!=0) || (yy-y2_f!=0)){
			embedded_line=true;
			LINE(xx,yy,0,0,x2_f,y2_f,0,0,FEEDRATE);
			embedded_line=false;
		}
   }
 
int casequartile(double x,double y,double K,double L, int point)
   {
	
    	int result;
    	if(x>=K && y>=L){
    		   result=1;
               if(y==L && point==1 && clockwise==true){
                 	result=4;
			   }
    			
    		}else if(x<=K && y>=L){
    			result=2;
    		}else if(x<=K && y<=L){
    			result=3;
    		}else if(x>=K && y<=L){
    			result=4;
    		}
    		return result;
    }
    
double theta_func(double x,double y,double k,double l,int a)
    {
    	double theta,dx,dy;
    	dy=fabs(y-l);
      	dx=fabs(x-k);
    	if(a==1){
      		theta=atan(dy/dx);
    	}else if(a==2){
    		theta=pi-atan(dy/dx);
    	}else if(a==3){
    		theta=(3.0*pi/2.0)-atan(dx/dy);
    	}else if(a==4){
    		theta=(2.0*pi)-atan(dy/dx);
    	}
    	return theta;
    }

double max_n(double num1, double num2) 
   {	
        double result;
        if (num1 > num2)
            result = num1;
        else
            result = num2;
        return result; 
    }
   
double min_n(double num1, double num2) 
   {

        double result;
        if (num1 < num2)
           result = num1;
        else
           result = num2;
        return result;
   }

void time_momments(double L,double umax){
		
	int loop=0,dek; 
    double t0,error,D,akr,xx,xa,xu,u,t,dt,dt1,dt2,b,E,t01,t02;

	if(JERK != 0){
     	dek=-12;
     	akr=0.5*pow(10,dek);
        t1=ACCEL_ERATION/JERK;
        ca=ACCEL_ERATION;
        cu=JMFEED+(JERK*pow(t1,2));
        xx=2*t1*(cu+JMFEED);
        if(xx<L){
            if(cu>=umax){
            	t1=sqrt((umax-JMFEED)/JERK);
            	ca=JERK*t1;
            	cu=umax;
            	xu=(2*JERK*pow(t1,3))+(4*JMFEED*t1);
            	dt=(L-xu)/umax;
    			state=3;
    			t2=2.0*t1;
        		t3=t2+dt;
        		t4=t3+t1;
    	    	t5=t4+t1;
        	}else{
         		dt1=(umax-((JERK*pow(t1,2))+JMFEED))/ACCEL_ERATION;
         		xa=((JMFEED+umax)*dt1)+(((2.0*JMFEED)+(2.0*umax))*t1);
         		ca=ACCEL_ERATION;
         		cu=umax;
        	 	if(xa>=L){
         			b=(2.0*JMFEED)+(JERK*pow(t1,2))+(2.0*ACCEL_ERATION*t1); 
         			D=pow(b,2)+(4.0*ACCEL_ERATION*(L-xx));
         			dt=(sqrt(D)-b)/(2.0*ACCEL_ERATION);
         			state=2;
         			t2=t1+dt;
         			t3=t2+t1;
         			t4=t3+t1;
         			t5=t4+dt;
         			t6=t5+t1;
         			cu=JMFEED+JERK*pow(t1,2)+(ACCEL_ERATION*dt);
         			xa=((JMFEED+cu)*dt)+(((2.0*JMFEED)+(2.0*cu))*t1);
        		}else{
         			dt2=(L-xa)/umax;
         			state=4;
         			t2=t1+dt1;
         			t3=t2+t1;
           			t4=t3+dt2;
         			t5=t4+t1;
         			t6=t5+dt1;
         			t7=t6+t1;
     	    	}
        	}
        }else{
            if(cu>=umax){
             	t1=sqrt((umax-JMFEED)/JERK);
          	    cu=umax;
        	    ca=JERK*t1;
         	    xx=2*t1*(cu+JMFEED);
         	    if(xx>=L){
            	    t0=t1/2.0;
        	        do{
         	            t=t0-(((2*JERK*pow(t0,3))+(4*JMFEED*t0)-L)/((6*JERK*pow(t0,2))+(4*JMFEED))); ////NEWTON-RAPHSON
                        error=fabs(t-t0);		
	                    t0=t;
                	    loop++;
                	    if(loop>50){ //increase calculation error if the root cannot be found
                	    	loop=0;
                	    	dek++;
                	    	akr=0.5*pow(10,dek);
	            		}
                    }while(error>=akr);
        	    	loop=0;
        			state=1;
        			t1=t;
        			t2=2.0*t1;
        			t3=3.0*t1;
        			t4=4.0*t1;
        			ca=JERK*t1;
	    	 		cu=JMFEED+(JERK*pow(t1,2));   	
	    		}else{
    			   	dt=(L-xx)/umax;
	    		   	state=3;
    			   	t2=2.0*t1;
        		 	t3=t2+dt;
        	 		t4=t3+t1;
	         		t5=t4+t1;
			   }
    	   }else{
         	    t0=t1*umax/cu;
         	    t0=50;
    	        do{
    	            t=t0-(((2.0*JERK*pow(t0,3))+(4.0*JMFEED*t0)-L)/((4.0*JERK*pow(t0,2))+(4.0*JMFEED))); ////NEWTON-RAPHSON
                    error=fabs(t-t0);		
	                t0=t;
            	    loop++;
            	    if(loop>50){ //increase calculation error if the root cannot be found
            	    	loop=0;
            	    	dek++;
            	    	akr=0.5*pow(10,dek);
	        		}
                }while(error>=akr);
    	    	loop=0;
    			state=1;
    			t1=t;
    			t2=2.0*t1;
    			t3=3.0*t1;
    			t4=4.0*t1;
    			ca=JERK*t1;
		 		cu=JMFEED+(JERK*pow(t1,2)); 
    	   }
    	}
    		if(state==1){ ///DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(t1*cu)+(JMFEED*t1);
                x3_t3=x2_t2+x2_t2-x1_t1;
    			x4_t4=2.0*x2_t2;
    		}else if(state==2){//DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			u2_t2=u1_t1+(ca*(t2-t1));
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(((u2_t2-u1_t1)*(t2-t1))/2.0)+(u1_t1*(t2-t1))+x1_t1;
    			x3_t3=(cu*(t3-t2))+(x2_t2-x1_t1)+(JMFEED*t1);
    			x4_t4=x3_t3+(x3_t3-x2_t2);
    			x5_t5=x4_t4+(x2_t2-x1_t1);
                x6_t6=2.0*x3_t3;
    		}else if(state==3){ ////DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(t1*cu)+(t1*JMFEED);
    			x3_t3=(cu*(t3-t2))+x2_t2;
    			x4_t4=x3_t3+x2_t2-x1_t1;
    			x5_t5=x3_t3+x2_t2;
    		}else if(state=4){ ///DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			u2_t2=u1_t1+(ca*(t2-t1));
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=x1_t1+(u1_t1*(t2-t1))+(ca*pow((t2-t1),2)/2.0);
    			x3_t3=(cu*t1)+(JMFEED*t1)+(x2_t2-x1_t1);
    			x4_t4=(cu*(t4-t3))+x3_t3;
    			x5_t5=x4_t4+(x3_t3-x2_t2);
    			x6_t6=x5_t5+(x2_t2-x1_t1);
    			x7_t7=x4_t4+x3_t3;
    		}
	    }else{
            t1=(umax-JMFEED)/ACCEL_ERATION;
      		t2=2*t1;
    		E=(2*JMFEED*t1)+(ACCEL_ERATION*pow(t1,2));
    		cu=umax;
    		state=1;
    		if(E<L){
    			t=(L-E)/umax;
    			t2=t1+t;
    			t3=t2+t1;
    			cu=umax;
    			state=2;
    		}else if(E>L){
    			D=pow(2.0*JMFEED,2)+(4.0*ACCEL_ERATION*L);
                t01=(-(2*JMFEED)+sqrt(D))/(2*ACCEL_ERATION);
                t02=(-(2*JMFEED)-sqrt(D))/(2*ACCEL_ERATION);
    			if(t01>0){
    				t1=t01;
				}else{
					t1=t02;
				}
    			t2=2*t1;
    			cu=L/(2*t1);
    			state=3;
    		}
    		if(state==1){
     			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2);
    			x2_t2=2*x1_t1;
    			//printf("%f %f %f\n",x1_t1,x2_t2,x3_t3);
    		}else if(state==2){
    			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2.0);
    			x2_t2=umax*(t2-t1)+x1_t1;
    			x3_t3=x2_t2+x1_t1;
     		}else if(state==3){
    			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2);
    			x2_t2=2*x1_t1;
    			//printf("%f %f %f\n",x1_t1,x2_t2,x3_t3);
    		}	    	
		}
}
	
double L_time_calc(double l)
{	   	
    int i=0,loop=0,dek; 
    double t0=2,t01,t02,t,error,D,akr;
 	if(JERK != 0){
    	dek=-9;
      	akr=0.5*pow(10,dek);
    	if(state==1){/// VIRTUALLY TESTED
    		if(l<=x1_t1){
        	    t0=t1/2.0;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;	
	    	}else if(l<=x2_t2){
            	t0=(t1+t2)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t1,2)*t0/2)-(JERK*pow(t1,3)/3)+(JERK*t2*pow(t0,2)/2)+\
					(JERK*t2*pow(t1,2)/2)-(JERK*t2*t1*t0)+(u1_t1*t0)-(u1_t1*t1)+x1_t1-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t1,2)/2)+(JERK*t2*t0)-(JERK*t2*t1)+u1_t1)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;  	    
    	    }else if(l<=x3_t3){
            	t0=(t2+t3)/2.0;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t2,3)/6)+(JERK*pow(t2,2)*t0/2)-(JERK*pow(t2,3)/2)+(JERK*t2*pow(t0,2)/2)-(JERK*pow(t2,3)/2)\
	    	          -(JERK*pow(t2,2)*t0)+(JERK*pow(t2,3))+(cu*t0)-(cu*t2)+x2_t2-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t2,2)/2)+(JERK*t2*t0)\
	    	          -(JERK*pow(t2,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
    		}else if(l<x4_t4){
        	    t0=(t3+t4)/2;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t3,3)/6)-(JERK*pow(t3,2)*t0/2)+(JERK*pow(t3,3)/2)-(JERK*t4*pow(t0,2)/2)+(JERK*t4*pow(t3,2)/2)\
	    	        +(JERK*t4*t3*t0)-(JERK*t4*pow(t3,2))+(u1_t1*t0)-(u1_t1*t3)+x3_t3-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t3,2)/2)-(JERK*t4*t0)\
	    	        +(JERK*t4*t3)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;					
    		}else{
    			t=t4;
    		}
    	}
    	if(state==2){ //// VIRTUALLY TESTED
            if(l<=x1_t1){
        	    t0=t1/2;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;
            }else if(l<=x2_t2){
                D=pow(u1_t1-(ca*t1),2)-(2*ca*((ca*pow(t1,2)/2)-(u1_t1*t1)+x1_t1-l));
                t01=((ca*t1)-u1_t1+sqrt(D))/ca;
                t02=((ca*t1)-u1_t1-sqrt(D))/ca;
                if(t01>=t1 && t01<=t2){
                	t=t01;
     			}else{
	    			t=t02;
	    		}
            }else if(l<=x3_t3){
        	    t0=(t2+t3)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t2,2)*t0/2)-(JERK*pow(t2,3)/3)+(JERK*t3*pow(t0,2)/2)+\
					(JERK*t3*pow(t2,2)/2)-(JERK*t3*t2*t0)+(u2_t2*t0)-(u2_t2*t2)+x2_t2-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t2,2)/2)+(JERK*t3*t0)-(JERK*t3*t2)+u2_t2)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;     	
    		}else if(l<=x4_t4){
        	    t0=(t3+t4)/2;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t3,3)/6)+(JERK*pow(t3,2)*t0/2)-(JERK*pow(t3,3)/2)+(JERK*t3*pow(t0,2)/2)-(JERK*pow(t3,3)/2)\
	    	          -(JERK*pow(t3,2)*t0)+(JERK*pow(t3,3))+(cu*t0)-(cu*t3)+x3_t3-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t3,2)/2)+(JERK*t3*t0)\
	    	          -(JERK*pow(t3,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
    	    }else if(l<=x5_t5){
    			D=pow(((ca*t4)+u2_t2),2)+(2.0*ca*((x4_t4)-(ca*pow(t4,2)/2)-(t4*u2_t2)-(l)));
    			t01=(((ca*t4)+u2_t2)+sqrt(D))/ca;
    			t02=(((ca*t4)+u2_t2)-sqrt(D))/ca;
    			if(t01>=t4 && t01<=t5){
    				t=t01;
  	    		}else{
	    			t=t02;
	    		}						    	
    		}else if(l<x6_t6){
        	    t0=(t5+t6)/2;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t5,3)/6)-(JERK*pow(t5,2)*t0/2)+(JERK*pow(t5,3)/2)-(JERK*t6*pow(t0,2)/2)+(JERK*t6*pow(t5,2)/2)\
	    	        +(JERK*t6*t5*t0)-(JERK*t6*pow(t5,2))+(u1_t1*t0)-(u1_t1*t5)+x5_t5-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t5,2)/2)-(JERK*t6*t0)\
	    	        +(JERK*t6*t5)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;				
    		}else{
    			t=t6;
    		}
    	}
 	
        if(state==3){//// TESTED
            if(l<=x1_t1){
        	    t0=t1/2.0;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found 
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;
            }else if(l<=x2_t2){
            	t0=(t1+t2)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t1,2)*t0/2)-(JERK*pow(t1,3)/3)+(JERK*t2*pow(t0,2)/2)+\
					(JERK*t2*pow(t1,2)/2)-(JERK*t2*t1*t0)+(u1_t1*t0)-(u1_t1*t1)+x1_t1-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t1,2)/2)+(JERK*t2*t0)-(JERK*t2*t1)+u1_t1)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;
        	}else if(l<=x3_t3){
                    t=(l-(x2_t2-(cu*t2)))/cu;	
        	}else if(l<=x4_t4){
            	t0=(t3+t4)/2.0;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t3,3)/6)+(JERK*pow(t3,2)*t0/2)-(JERK*pow(t3,3)/2)+(JERK*t3*pow(t0,2)/2)-(JERK*pow(t3,3)/2)\
	    	          -(JERK*pow(t3,2)*t0)+(JERK*pow(t3,3))+(cu*t0)-(cu*t3)+x3_t3-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t3,2)/2)+(JERK*t3*t0)\
	    	          -(JERK*pow(t3,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
        	}else if(l<x5_t5){
            	t0=(t4+t5)/2.0;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t4,3)/6)-(JERK*pow(t4,2)*t0/2)+(JERK*pow(t4,3)/2)-(JERK*t5*pow(t0,2)/2)+(JERK*t5*pow(t4,2)/2)\
	    	        +(JERK*t5*t4*t0)-(JERK*t5*pow(t4,2))+(u1_t1*t0)-(u1_t1*t4)+x4_t4-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t4,2)/2)-(JERK*t5*t0)\
	    	        +(JERK*t5*t4)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;		
         	}else{
         		t=t5;
    	    }	
        }
  
        if(state==4){/// TESTED
            if(l<=x1_t1){
        	    t0=t1/2;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;
            }else if(l<=x2_t2){
                D=pow(u1_t1-(ca*t1),2)-(2*ca*((ca*pow(t1,2)/2)-(u1_t1*t1)+x1_t1-l));
                t01=((ca*t1)-u1_t1+sqrt(D))/ca;
                t02=((ca*t1)-u1_t1-sqrt(D))/ca;
                if(t01>=t1 && t01<=t2){
                	t=t01;
     			}else{
	    			t=t02;
	    		}
            }else if(l<=x3_t3){
        	    t0=(t2+t3)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t2,2)*t0/2)-(JERK*pow(t2,3)/3)+(JERK*t3*pow(t0,2)/2)+\
					(JERK*t3*pow(t2,2)/2)-(JERK*t3*t2*t0)+(u2_t2*t0)-(u2_t2*t2)+x2_t2-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t2,2)/2)+(JERK*t3*t0)-(JERK*t3*t2)+u2_t2)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;     	
    		}else if(l<=x4_t4){
    			t=(l-(x3_t3-(cu*t3)))/cu;
    		}else if(l<=x5_t5){
        	    t0=(t4+t5)/2;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t4,3)/6)+(JERK*pow(t4,2)*t0/2)-(JERK*pow(t4,3)/2)+(JERK*t4*pow(t0,2)/2)-(JERK*pow(t4,3)/2)\
	    	          -(JERK*pow(t4,2)*t0)+(JERK*pow(t4,3))+(cu*t0)-(cu*t4)+x4_t4-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t4,2)/2)+(JERK*t4*t0)\
	    	          -(JERK*pow(t4,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
   	    	}else if(l<=x6_t6){
    			D=pow(((ca*t5)+u2_t2),2)+(2.0*ca*((x5_t5)-(ca*pow(t5,2)/2)-(t5*u2_t2)-(l)));
    			t01=(((ca*t5)+u2_t2)+sqrt(D))/ca;
    			t02=(((ca*t5)+u2_t2)-sqrt(D))/ca;
    			if(t01>=t5 && t01<=t6){
    				t=t01;
  	    		}else{
	    			t=t02;
	    		}
	    	}else if(l<x7_t7){
        	    t0=(t6+t7)/2;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t6,3)/6)-(JERK*pow(t6,2)*t0/2)+(JERK*pow(t6,3)/2)-(JERK*t7*pow(t0,2)/2)+(JERK*t7*pow(t6,2)/2)\
	    	        +(JERK*t7*t6*t0)-(JERK*t7*pow(t6,2))+(u1_t1*t0)-(u1_t1*t6)+x6_t6-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t6,2)/2)-(JERK*t7*t0)\
	    	        +(JERK*t7*t6)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;		
    		}else{
    			t=t7;
    		}
    	}
    	if(state==10){
    		t=l/cu;
		}
    }else{
    	if(state==1){///TESTED
    		if(l<=x1_t1){
    		    D=pow(JMFEED,2)+(2*ACCEL_ERATION*l);
    		    t01=((-JMFEED)+sqrt(D))/ACCEL_ERATION;
    		    t02=((-JMFEED)-sqrt(D))/ACCEL_ERATION;
    		    if(t01>0 && t01<=t1){
    		    	t=t01;
				}else{
					t=t02;
				}
    		}else if(l<=x2_t2){
    			D=pow(((ACCEL_ERATION*t2)+JMFEED),2)+(2*ACCEL_ERATION*((ACCEL_ERATION*pow(t1,2)/2)-(ACCEL_ERATION*t2*t1)-(JMFEED*t1)+x1_t1-l));
    			t01=(-((ACCEL_ERATION*t2)+JMFEED)+sqrt(D))/(-ACCEL_ERATION);
    			t02=(-((ACCEL_ERATION*t2)+JMFEED)-sqrt(D))/(-ACCEL_ERATION);
    			if(t01>=t1 && t01<=t2){
                	t=t01;
    			}else{
	    			t=t02;
	    		}
			}else{
				t=t2;
			}	    
    	}
    	if(state==2){ ////TESTED
            if(l<=x1_t1){
    		    D=pow(JMFEED,2)+(2*ACCEL_ERATION*l);
    		    t01=((-JMFEED)+sqrt(D))/ACCEL_ERATION;
    		    t02=((-JMFEED)-sqrt(D))/ACCEL_ERATION;
    		    if(t01>0 && t01<=t1){
    		    	t=t01;
				}else{
					t=t02;
				}
			}else if(l<=x2_t2){
			    t=((cu*t1)+(l-x1_t1))/cu;
			}else if(l<=x3_t3){
    			D=pow(((ACCEL_ERATION*t3)+JMFEED),2)+(2*ACCEL_ERATION*((ACCEL_ERATION*pow(t2,2)/2)-(ACCEL_ERATION*t3*t2)-(JMFEED*t2)+x2_t2-l));
    			t01=(-((ACCEL_ERATION*t3)+JMFEED)+sqrt(D))/(-ACCEL_ERATION);
    			t02=(-((ACCEL_ERATION*t3)+JMFEED)-sqrt(D))/(-ACCEL_ERATION);
    			if(t01>=t2 && t01<=t3){
                	t=t01;
    			}else{
	    			t=t02;
	    		}
			}else{
				t=t3;
			}
    	}
        if(state==3){//// TESTED
    		if(l<=x1_t1){
    		    D=pow(JMFEED,2)+(2*ACCEL_ERATION*l);
    		    t01=((-JMFEED)+sqrt(D))/ACCEL_ERATION;
    		    t02=((-JMFEED)-sqrt(D))/ACCEL_ERATION;
    		    if(t01>0 && t01<=t1){
    		    	t=t01;
				}else{
					t=t02;
				}
    		}else if(l<=x2_t2){
    			D=pow(((ACCEL_ERATION*t2)+JMFEED),2)+(2*ACCEL_ERATION*((ACCEL_ERATION*pow(t1,2)/2)-(ACCEL_ERATION*t2*t1)-(JMFEED*t1)+x1_t1-l));
    			t01=(-((ACCEL_ERATION*t2)+JMFEED)+sqrt(D))/(-ACCEL_ERATION);
    			t02=(-((ACCEL_ERATION*t2)+JMFEED)-sqrt(D))/(-ACCEL_ERATION);
    			if(t01>=t1 && t01<=t2){
                	t=t01;
    			}else{
	    			t=t02;
	    		}
			}else{
				t=t2;
			}
        }
	}
    if(state==10){
    	t=l/cu;
	}
    	return t;
    }


void wr2bin(int stepx, int stepy, int stepz, int stepe, double l)
    {
        int i=0,j,ABL_Coord_Position=0,ABL_Z_steps=0;
        char bits[9];
        uint8_t byte_num,null_byte=0,timeboxes;//50 fixe a buf at starting
    


	    for(j=0;j<=7;j++){ ///set all bits 0 for safety
       		bits[j]='0';
       	}
       	j=0;
        ///////////****set the suitable bits 0,1 to arduino output pins, step and direction for each axis****/////////////	
       	if(stepx==0){
            bits[6]='0';
            bits[7]='0';
       	}else if(stepx > 0){
       		X_GLOB=X_GLOB+X_GLOB_STEP;
            bits[6]='1';
            if(Invrt_X==0){
              	bits[7]='0';
			}else{
				bits[7]='1';
			}
       	}else if(stepx<0){
       		X_GLOB=X_GLOB-X_GLOB_STEP;
            bits[6]='1';  
			if(Invrt_X==0){
				bits[7]='1';
			}else{
				bits[7]='0';
			}	
       	}
       	if(stepy==0){
            bits[4]='0';
            bits[5]='0';
       	}else if(stepy>0){
       		Y_GLOB=Y_GLOB+Y_GLOB_STEP;
            bits[4]='1';
            if(Invrt_Y==0){
               	bits[5]='0';
			}else{
				bits[5]='1';
			}
       	}else if(stepy<0){
       		Y_GLOB=Y_GLOB-Y_GLOB_STEP;
            bits[4]='1';
            if(Invrt_Y==0){
              	bits[5]='1';
			}else{
				bits[5]='0';
    		}
       	}
       	if(stepz==0){
            bits[2]='0';
            bits[3]='0';
       	}else if(stepz>0){
            bits[2]='1';
            if(Invrt_Z==0){
               	bits[3]='0';
			}else{
    			bits[3]='1';
			}
       	}else if(stepz<0){
            bits[2]='1';
            if(Invrt_Z==0){
               	bits[3]='1';
			}else{
				bits[3]='0';
    		}	
        }
       	if(stepe==0){
            bits[1]='0';
            bits[0]='0';
       	}else if(stepe>0){
                bits[0]='1';
                if(Invrt_E==0){
                	bits[1]='0';
				}else{
					bits[1]='1';
				}
       	}else if(stepe<0){
               bits[0]='1';
			if(Invrt_E==0){
				bits[1]='1';
			}else{
				bits[1]='0';
			}	
   	    }        
	    bits[8]='e'; //end bit  
  	    byte_num=bits2val(bits); //byte num = the 8bit integer calculated from previous bits that arduino will port forward to his outputs 
  	    time=L_time_calc(l); //calculate time momment from the begining of motion for current point trajectory
  	    if (embedded_line==true){
  	    	timeboxes=LAST_TIME_BOXES;
		}else{
			timeboxes=(time-last_time)/tmin; //ARDUINO LOW PULSE DURATION BETWEEN STEPS = CURRENT STEP FREQUENCY = CURRENT SPEED
		}
        if(timeboxes<=1){
        	timeboxes=30;///FIX A TIME BUG
		}else{
			last_time=time;
		}
	    if(timeboxes==255){
        	timeboxes=254;///FIX A TIME BUG
		}
   	    write_hex2file(timeboxes);
   	    write_hex2file(byte_num);
   	    LAST_TIME_BOXES=timeboxes;

    	if(ABL_Include==1 && ABL_Data==true){
			for(j=0;j<=7;j++){ ///set all bits 0 for safety
				bits[j]='0';
			}
			j=0;
			//ABL_Coord_Position=(abs(Y_GLOB+0.00000001)*(BED_XSIZE+1))+abs(X_GLOB+0.5); //0.00000001 FIX A FLOAT BUG
			ABL_Coord_Position=(abs((Y_GLOB/GRID_RESOLUTION)+0.00000001)*((BED_XSIZE/GRID_RESOLUTION)+1))+abs((X_GLOB/GRID_RESOLUTION)+0.5); //0.00000001 FIX A FLOAT BUG
			ABL_Z_steps = (ABL_ZCOORD[ABL_Coord_Position]-ABL_Z_last);
			//printf("%s %f %f %f %i %i\n","X Y Z correction",X_GLOB,Y_GLOB,ABL_ZCOORD[ABL_Coord_Position],ABL_Coord_Position,abs(ABL_Z_steps));
			if(ABL_Z_steps==0){
				bits[2]='0';
				bits[3]='0';
			}else if(ABL_Z_steps>0){
				ABL_Z_last = ABL_ZCOORD[ABL_Coord_Position]; 
				bits[2]='1';
				if(Invrt_Z==0){
					bits[3]='0';
				}else{
					bits[3]='1';
				}
			}else if(ABL_Z_steps<0){
				ABL_Z_last = ABL_ZCOORD[ABL_Coord_Position]; 
				bits[2]='1';
				if(Invrt_Z==0){
					bits[3]='1';
				}else{
					bits[3]='0';
				}	
			}
			bits[8]='e'; //end bit 
			byte_num=bits2val(bits);
			for(j=0;j<abs(ABL_Z_steps);j++){
				write_hex2file(2);
				write_hex2file(byte_num);
			}
			j=0;
   		}

   	    //last_time=time;
		//storage_counter=storage_counter+storage_step;
    }

void write_hex2file(uint8_t hex_value) //https://stackoverflow.com/questions/35390912/proper-way-to-get-file-size-in-c?rq=1
{
	struct stat buffer;
	struct stat child_buffer;
    int         status;
    int   child_status;
    uint16_t trash_byte=100;
    
    if(PRINT_STATE==0){ //USB PRINTING
    	file_buffer_size++; //increase bytecounter
    	if(flag_file_state==false){ //write bytes to free file
         	fwrite(&hex_value, sizeof(uint8_t),1,buffer1_file);
    	}else{
    	    fwrite(&hex_value, sizeof(uint8_t),1,buffer2_file);
    	}
    	if(file_buffer_size==max_bufferfile_size){ //max file size has reached
        	if(flag_file_state==false){ //close the file
        		fclose(buffer1_file);
          		if(first_time_executed==true){ //write to flag///so python can start
        			first_time_executed=false;
        			startpy_file=_wfopen(startpy_path,L"wb");
        			fwrite(&trash_byte, sizeof(uint16_t),1,startpy_file);
                    fclose(startpy_file);
    			}
        	}else{
        		fclose(buffer2_file);
        	}
    		do{ 
    			status = wstat(flag_path, &buffer); //check flag state....stat replaced with wstat
                child_status = wstat(child_path, &child_buffer);
                if(child_buffer.st_size==0){ //check if Python GUI is terminated through a file signal
                    printf("%s\n","THE PRINTING PROCESS HAS BEEN TERMINATED BY EXTERNAL SIGNAL");
                	exit(0);
    			}
    		}while(buffer.st_size==0); //wait for a file to be free
    		if(flag_file_state==true){ //open the free file for writing
        		buffer1_file=_wfopen(buffer1_path,L"wb");
        	}else{
        		buffer2_file=_wfopen(buffer2_path,L"wb");
        	}
        	if(first_time_executed==false){
    		  flag_file=_wfopen(flag_path,L"wb"); //reset flag's file state (size) to 0
    		  fclose(flag_file);
        	}
    		file_buffer_size=0; //reset bytecounter
    		flag_file_state = !flag_file_state; 
    	}
    }else{ //SD CARD PRINTING
    	fwrite(&hex_value, sizeof(uint8_t),1,SD_binary_file);
    	storage_counter=storage_counter+storage_step;
		file_buffer_size++;
		if(file_buffer_size==max_bufferfile_size){
			file_buffer_size=0;
            child_status = wstat(child_path, &child_buffer);
            if(child_buffer.st_size==0){ //check if Python GUI is terminated through a file signal
                printf("%s\n","THE PRINTING PROCESS HAS BEEN TERMINATED BY EXTERNAL SIGNAL");
                exit(0);
    		}
		}
	}
}

uint8_t bits2val(char *bits) /// https://stackoverflow.com/questions/11310796/binary-to-unsigned-int-using-bitwise-operations-and-pointer-arithmetic-in-c
{
     uint8_t val;

    for (val = 0; *bits; bits++) {
        if (*bits == '1') 
            val = (val << 1) | 1;
        else if (*bits == '0' ) 
            val <<= 1;
        else 
            break;
    }

    return val;
}

void close_SD_binary_file()
{
    uint8_t stop_byte=0;
    int empty_space,i,x;
    empty_space=max_bufferfile_size-(file_buffer_size);
    for(x=0;x<=1;x++){
        for(i=0;i<empty_space;i++){ //fill with stopbyte until 2times file size;
        	write_hex2file(stop_byte);
    	}
    	empty_space=max_bufferfile_size;
    }
}


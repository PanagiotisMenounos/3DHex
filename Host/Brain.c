/*
----------------------------------------------------------------------
COPYRIGHT NOTICE FOR 3DHex:
----------------------------------------------------------------------

3DHex - 3D Printer Firmware

Copyright (c) 2019 Panagiotis Menounos
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

       ///////////////**************GLOBAL VARIABLES FOR SETTINGS******************/////////////////////
double STPU_X,STPU_Y,STPU_Z,STPU_E,MAX_FX,MAX_FY,MAX_FZ,MAX_FE,MAX_ACCX,MAX_ACCY,MAX_ACCZ,MAX_ACCE,ACCELERATION,JERK,MAX_JFX,\
       MAX_JFY,MAX_JFZ,MAX_JFE,JMFEED,PARK_X,PARK_Y,PARK_Z,PARK_FEED,MAX_FILE_SIZE;
int Invrt_X,Invrt_Y,Invrt_Z,Invrt_E,COM_PORT,UNITS;
int32_t BAUD_RATE;
uint16_t CORE_FREQ;
uint8_t X_ENABLE,Y_ENABLE,Z_ENABLE,E_ENABLE;

double P_NOZZ,I_NOZZ,D_NOZZ,P_BED,I_BED,D_BED;//NOT USED FOR NOW
uint16_t NOZZLE_TEMP,BED_TEMP,CYCLE_NOZZ,CYCLE_BED,P_NOZZ_MIRROR,I_NOZZ_MIRROR,D_NOZZ_MIRROR,P_BED_MIRROR,I_BED_MIRROR,D_BED_MIRROR;
uint8_t Wait_nozz,Wait_bed,THERMISTOR_TYPE_NOZZLE,THERMISTOR_TYPE_BED,HEATED_NOZZLE,HEATED_BED,PID_nozz,\
        PID_bed,DIFFERENTIAL_NOZZ,DIFFERENTIAL_BED;
        
int HOME_FEED_X,HOME_FEED_Y,HOME_FEED_Z;
uint16_t HOME_X_DURATION,HOME_Y_DURATION,HOME_Z_DURATION;
uint8_t HOME_X_ENABLE,HOME_Y_ENABLE,HOME_Z_ENABLE,HOME_X_STATE,HOME_Y_STATE,HOME_Z_STATE,HOME_X_DIR,HOME_Y_DIR,HOME_Z_DIR;

double XY_PLANE,ZX_PLANE,ZY_PLANE;
        
     ////////////////////////*****************/////////////////////////////////////

//////////////////////////////***********GLOBAL VARIABLES******//////////////////
int total_digi_lines=0,state,file_buffer_size=0,max_bufferfile_size=3300,PRINT_STATE, file_num=0;//number name of binary file
double storage_step=0.00000000000001,storage_counter=0,ACCEL_ERATION;
bool first=false,flag_file_state=false,first_time_executed=true; //for composite material;
double tmin,u1_t1,u2_t2,x1_t1,x2_t2,x3_t3,x4_t4,x5_t5,x6_t6,x7_t7,t1,t2,t3,t4,t5,t6,t7,cu,ca,time=0,last_time=0;


wchar_t savepath[13][100]={L"//3DHex//coordinates.txt\0",L"//3DHex//gc2info.txt\0",L"//3DHex//savepath.txt\0",\
                   L"//3DHex//general settings.txt\0",L"//3DHex//temp settings.txt\0",L"//3DHex//GCODE.txt\0",\
				   L"//3DHex//buffer_1.bin\0",L"//3DHex//buffer_2.bin\0",L"//3DHex//flag.bin\0",L"//3DHex//flag_py.bin\0",\
				   L"//3DHex//child.bin\0",L"//3DHex//homing settings.txt\0",L"//3DHex//angle settings.txt\0"};
wchar_t coordinates_path[150],gc2info_path[150],savepath_path[150],general_sett_path[150],temp_sett_path[150],
        gcode_path[150],buffer1_path[150],buffer2_path[150],flag_path[150],startpy_path[150],child_path[150],home_path[150],angle_path[150];
 /////////////////////////*******//////////////////////

FILE *SD_binary_file;//MUST BE GLOBAL
FILE *buffer1_file;
FILE *buffer2_file;
FILE *flag_file;
FILE *startpy_file;
//FILE *savepath_file;

void LINE(double xf, double yf, double zf, double ef, double xl, double yl, double zl, double el, double FEEDRATE); //G01,G00 calculations   
void ARC(bool input,double k,double l,double x1_f,double y1_f,double Ef,double x2_f,double y2_f,double El,double FEEDRATE); //GO2,G03 calculations 
int casequartile(double x,double y,double K,double L); //find the quartile (1-4) of a point in polar coordinate system
double max_n(double num1, double num2); //returns the max value
double min_n(double num1, double num2) ; //returns the min value
double theta_func(double x,double y,double k,double l,int a,bool input); //returns the angle of a point-line in polar coordinate system
void time_momments(double L,double umax);//returns the time momments of velocity profile
double L_time_calc(double l); //returns the time momment of points-pulses in XYZ axis
double line_accel_feed_limits(double dx,double dy,double dz,double E_DIST,double FEEDRATE); //check derivative limits of G00-G01 (line)
double arc_accel_feed_limits(double R,double ARC_DIST,double E_DIST,double FEEDRATE); //check derivativelimits of G02-G03 (arc)
double check_jfeed_limits(double dx,double dy,double dz,double E_DIST); //check jump feed rate limits (only for lines)
void composite(double Command,double Ef,double El); //finds when should be cut the fiber
void path_files(); //locates where the %APPDATA% is 
double check_units(double Fl); //convert to mm/sec everything
void crt_file(); //creates the output files
void wr2bin(int stepx, int stepy, int stepz, int stepe, double l); //writes the stop byte to output file
unsigned long gc2info(double flag_num); //write all the information clear to a file provided from Gcode
void read_general_settings(); //returns all the general settings  
void read_temp_settings(); //returns all the temperature settings 
void read_home_settings();
void read_angle_settings();
void close_SD_binary_file(); //close the output file
void hidecursor(); //hide the cursor on the console
void write_hex2file(uint8_t hex_value);
void mcu_settings_send();
int check_print_state(); //return 0 => USB return 1 => SD_CARD
void check_SD_file_size(double Xf, double Yf, double Zf, double Xl, double Yl, double Zl);
uint8_t bits2val(char *bits);///convert a serial of 0,1 to a number 16bit (0,1) = 2 bytes(unsigned integer=uint16_t)

///////////////////////****************//////////////////////////

int main()
    {
    	FILE *g1;
        double Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl,Sl,Pl,Rl,Tl,flag_num=101101101.101010,perc=0;
        unsigned long total_lines,j;
        bool input,first_line=true;

        path_files();
    	read_general_settings();
    	read_temp_settings(); 
    	read_home_settings();
    	read_angle_settings();
		hidecursor(); 
		PRINT_STATE = check_print_state(); 
		if(PRINT_STATE==0){
			printf("%s\n","Perfoming the necessary processes for proper operation...");
		    mcu_settings_send();
		}else{
			printf("%s\n","The Host will save all the computations...this may take a while...");
			crt_file();
		}        
        total_lines=gc2info(flag_num);
        g1=_wfopen(coordinates_path,L"r");
        printf("%s %.2f%%\r","Progress:",perc);
        for(j=1;j<=total_lines;j++){
		    if(first_line==true){
		        first_line=false;
	            fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gf,&Mf,&Xf,&Yf,&Zf,&If,&Jf,&Ef,&Ff,&Sf,&Pf,&Rf,&Tf);
	            j++;
	    	}
            fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl);
            Fl=check_units(Fl);
            if(Gl!=flag_num){ /// enter only of it is a G command
            	//composite(Gl,Ef,El);
            	if(Gl==1 || Gl==01 || Gl==0 || Gl==00){   //only G01 OR G1 =line 
				    if(XY_PLANE!=0 && (ZX_PLANE!=0 || ZY_PLANE!=0)){
				    	LINE(0,0,Zf,0,0,0,Zl,0,0);
				    	LINE(Xf,Yf,0,Ef,Xl,Yl,0,El,Fl);
					}else{
						LINE(Xf,Yf,Zf,Ef,Xl,Yl,Zl,El,Fl);
					}     		
                }else if (Gl==2 || Gl==02 || Gl==3 || Gl==03){ //only arc command = G02 or G03
				    if(Gl==2 || Gl==02){
		    			input=true; //clockwise
		    		}else{
		    			input=false; //counterclockwise
		    		}
		    		ARC(input,Il,Jl,Xf,Yf,Ef,Xl,Yl,El,Fl);                	
				}
			}
			Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle 
			perc=100.0*j/total_lines; //calculate the completed percent
			check_SD_file_size(Xf,Yf,Zf,Xl,Yl,Zl);			
			printf("%s %.2f%%\r","Progress:",perc);
        } 
		printf("%s %.2f%%\n","Progress:",perc);  
		printf("%s\n","COMPLETED!");
        close_SD_binary_file();
        fclose(buffer1_file);
        fclose(buffer2_file);
        fclose(SD_binary_file);
        system("pause");   //wait the user to press enter key at the end
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
    fwrite(&HOME_Z_STATE, sizeof(uint8_t),1,buffer1_file);//MUST BE WRITTEN TWICE
    file_buffer_size=file_buffer_size+52;
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
   	    coordinates_path[j]=appdata_path[i];gc2info_path[j]=appdata_path[i];savepath_path[j]=appdata_path[i];
		general_sett_path[j]=appdata_path[i];temp_sett_path[j]=appdata_path[i];gcode_path[j]=appdata_path[i];
		buffer1_path[j]=appdata_path[i];buffer2_path[j]=appdata_path[i];flag_path[j]=appdata_path[i];
		startpy_path[j]=appdata_path[i];child_path[j]=appdata_path[i];home_path[j]=appdata_path[i];angle_path[j]=appdata_path[i];
   		j++;
		i++;	   
    }
    for(f=0;f<13;f++){
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
             	gc2info_path[p]=savepath[f][i];
            	p++;
             	i++;
            }     		
		}		
    	if(f==2){
		    while(savepath[f][i]!=L'\0'){
             	savepath_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==3){
		    while(savepath[f][i]!=L'\0'){
             	general_sett_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==4){
		    while(savepath[f][i]!=L'\0'){
             	temp_sett_path[p]=savepath[f][i];
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
             	buffer1_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==7){
		    while(savepath[f][i]!=L'\0'){
             	buffer2_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==8){
		    while(savepath[f][i]!=L'\0'){
             	flag_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==9){
		    while(savepath[f][i]!=L'\0'){
             	startpy_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}	
    	if(f==10){
		    while(savepath[f][i]!=L'\0'){
             	child_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==11){
		    while(savepath[f][i]!=L'\0'){
             	home_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==12){
		    while(savepath[f][i]!=L'\0'){
             	angle_path[p]=savepath[f][i];
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
	//printf("%f %f\n",ACCEL_ERATION,FEEDRATE);
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


  
void read_general_settings()
{
    FILE *gen_sett;
    char a[45][30],temp_str[30];  
    int i;
		
    gen_sett=_wfopen(general_sett_path,L"r");	
    
    for (i=0;i<=44;i++){
        fgets (temp_str, 30, gen_sett);
        strcpy(a[i],temp_str);
        if(i==1 || i==2 || i==3 || i==4 || i==6 || i==7 || i==8 || i==9 || i==10 || i==11 || i==12 || i==13 || i==14 || i==15 ||\
		   i==20 || i==21 || i==22 || i==23 || i==24 || i==25 || i==26 || i==27 || i==28 || i==30 || i==31 || i==32 || i==32){
             if(a[i][0]==' '){
		    	printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
		    	system("pause");
	    		exit(0);
        	}		  	
		}
    }
    sscanf(a[1], "%lf", &STPU_X); sscanf(a[2], "%lf", &STPU_Y); sscanf(a[3], "%lf", &STPU_Z); sscanf(a[4], "%lf", &STPU_E);
    sscanf(a[6], "%lf", &MAX_FX); sscanf(a[7], "%lf", &MAX_FY); sscanf(a[8], "%lf", &MAX_FZ); sscanf(a[9], "%lf", &MAX_FE);
    sscanf(a[10], "%lf", &ACCELERATION); sscanf(a[11], "%lf", &MAX_ACCX); sscanf(a[12], "%lf", &MAX_ACCY); sscanf(a[13], "%lf", &MAX_ACCZ); 
	sscanf(a[14], "%lf", &MAX_ACCE); sscanf(a[15], "%lf", &JERK); sscanf(a[20], "%lf", &JMFEED); sscanf(a[21], "%lf", &MAX_JFX); 
	sscanf(a[22], "%lf", &MAX_JFY); sscanf(a[23], "%lf", &MAX_JFZ); sscanf(a[24], "%lf", &MAX_JFE); sscanf(a[25], "%lf", &PARK_FEED);
    sscanf(a[26], "%lf", &PARK_X); sscanf(a[27], "%lf", &PARK_Y); sscanf(a[28], "%lf", &PARK_Z); CORE_FREQ=atoi(a[30]);
	sscanf(a[31], "%lf", &MAX_FILE_SIZE); COM_PORT=atoi(a[32]); BAUD_RATE=atoi(a[33]);
    Invrt_X=atoi(a[35]); Invrt_Y=atoi(a[36]); Invrt_Z=atoi(a[37]); Invrt_E=atoi(a[38]);UNITS=atoi(a[39]);
    X_ENABLE=atoi(a[41]); Y_ENABLE=atoi(a[42]); Z_ENABLE=atoi(a[43]); E_ENABLE=atoi(a[44]);
    MAX_FILE_SIZE=MAX_FILE_SIZE*pow(10,-5);
    fclose(gen_sett);
}

void read_temp_settings()
{
    FILE *temp_sett;
    char a[20][30],temp_str[30];  
    int i;	
    
    temp_sett=_wfopen(temp_sett_path,L"r");
    
    for (i=0;i<=19;i++){
        fgets (temp_str, 30, temp_sett);
        strcpy(a[i],temp_str);	  	
    } 
    NOZZLE_TEMP=atoi(a[0]); sscanf(a[1], "%lf", &P_NOZZ); sscanf(a[2], "%lf", &I_NOZZ); sscanf(a[3], "%lf", &D_NOZZ);
    THERMISTOR_TYPE_NOZZLE=atoi(a[4]); DIFFERENTIAL_NOZZ=atoi(a[5]); CYCLE_NOZZ=atoi(a[6]); BED_TEMP=atoi(a[7]);
	sscanf(a[8], "%lf", &P_BED); sscanf(a[9], "%lf", &I_BED); sscanf(a[10], "%lf", &D_BED); THERMISTOR_TYPE_BED=atoi(a[11]);
    DIFFERENTIAL_BED=atoi(a[12]);  CYCLE_BED=atoi(a[13]);Wait_nozz=atoi(a[14]); HEATED_NOZZLE=atoi(a[15]); HEATED_BED=atoi(a[16]);\
	PID_nozz=atoi(a[17]); Wait_bed=atoi(a[18]); PID_bed=atoi(a[19]);
	P_NOZZ_MIRROR=P_NOZZ*100; I_NOZZ_MIRROR=I_NOZZ*100; D_NOZZ_MIRROR=D_NOZZ*100;
	P_BED_MIRROR=P_BED*100; I_BED_MIRROR=I_BED*100; D_BED_MIRROR=D_BED*100;
	if(HEATED_NOZZLE==1){
		if(PID_nozz==1){
			if(a[0][0]==' ' || a[1][0]==' ' || a[2][0]==' ' || a[3][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}
		}else{
			if(a[4][0]==' ' || a[5][0]==' ' || a[6][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}
		}
		if(PID_bed==1){
			if(a[7][0]==' ' || a[8][0]==' ' || a[9][0]==' ' || a[10][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}else{
			if(a[11][0]==' ' || a[12][0]==' ' || a[13][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}				
			}			
		}
	} 
	fclose(temp_sett);
}

void read_home_settings(){
    FILE *home_sett;
    char a[12][30],temp_str[30];  
    int i;	
    
    home_sett=_wfopen(home_path,L"r");
    
    for (i=0;i<=11;i++){
        fgets (temp_str, 30, home_sett);
        strcpy(a[i],temp_str);
		if(i==0){
			if(a[3][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}
		}else if(i==1){
			if(a[4][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}	
		}else if(i==2){
			if(a[5][0]==' '){
				printf("%s\n","NON VALID SETTINGS!!! ...PLEASE FILL ALL THE SETTINGS");
				system("pause");
				exit(0);
			}			
		}		
	}
	 HOME_FEED_X=atoi(a[0]); HOME_FEED_Y=atoi(a[1]); HOME_FEED_Z=atoi(a[2]);
	 HOME_X_ENABLE=atoi(a[3]); HOME_Y_ENABLE=atoi(a[4]); HOME_Z_ENABLE=atoi(a[5]);
	 HOME_X_STATE=atoi(a[6]); HOME_Y_STATE=atoi(a[7]); HOME_Z_STATE=atoi(a[8]);
	 HOME_X_DIR=atoi(a[9]); HOME_Y_DIR=atoi(a[10]); HOME_Z_DIR=atoi(a[11]);
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
	 fclose(home_sett);
}

void read_angle_settings(){
    FILE *angle_sett;
    char a[3][30],temp_str[30];  
    int i;
    
    angle_sett=_wfopen(angle_path,L"r");
    
    for (i=0;i<=2;i++){
    	fgets (temp_str, 30, angle_sett);
        strcpy(a[i],temp_str);
    }
    sscanf(a[0], "%lf", &XY_PLANE);
	sscanf(a[1], "%lf", &ZX_PLANE);
	sscanf(a[2], "%lf", &ZY_PLANE);
    fclose(angle_sett);
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
    char string [200],letters[arrays_size]={'G','M','X','Y','Z','I','J','E','F','S','P','R','T'}; 
	unsigned long i=0,j,g,poslet=0,total_lines=0;
    double line1[arrays_size],line2[arrays_size];
    bool first_line1=true,first_line2=true,notfound=true;
	FILE *fp;
	fp=_wfopen(gcode_path,L"r"); //here its the gcode
	FILE *g1;
	FILE *g2;
    g1=_wfopen(gc2info_path,L"w");	//the results are written in this fille
    while (fgets(string, 200, fp) != NULL){//TILL THE END OF FILE
	   if (string[0]=='G' || string[0]=='M'){
	        total_lines++;								  
		    for(poslet=0;poslet<arrays_size;poslet++){		  
		        while (string[i] != '\0' && string[i] != ';' ){
		            if ((string[i]) == (letters[poslet])){
			            notfound=false;
			            j=i+1;
			            while(string[j] != ' ' && string[j] != '\0' && string[j] != ';' && string[j] != '\n'){
						    if (string[j] != '[' && string[j] !=']'){
			                    fprintf(g1,"%c",string[j]);
			                }
						    j++;	
					    }
			        }
			        i++;
				}
		        if(notfound==true){
		            if(first_line1==false){
				        fprintf(g1,"%lf",flag_num);
			        }else{
			        	fprintf(g1,"%lf",0);
			       }
			    }	
	            j=0;
		        i=0;
		        notfound=true;
		        fprintf(g1,"%c",' ');
 	        }
 	        first_line1=false;
            fprintf(g1, "%c\n",' ' );
        }
    }
    fclose(g1);
    fclose(fp);
    //printf("%s %i","Pharsing is done...result are written to the txt,total lines= ",total_lines);
    g1=_wfopen(gc2info_path,L"r");
    g2=_wfopen(coordinates_path,L"w");
    i=0;
    for(j=1;j<total_lines;j++){
		if(first_line2==true){
		    first_line2=false;
	        fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&line1[i],&line1[i+1],&line1[i+2],&line1[i+3],&line1[i+4],&line1[i+5],\
		           &line1[i+6],&line1[i+7],&line1[i+8],&line1[i+9],&line1[i+10],&line1[i+11],&line1[i+12]);
				  	
	        fprintf(g2,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",line1[i],line1[i+1],line1[i+2],line1[i+3],line1[i+4],line1[i+5],\
	    	        line1[i+6],line1[i+7],line1[i+8],line1[i+9],line1[i+10],line1[i+11],line1[i+12]);	
		}
        fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&line2[i],&line2[i+1],&line2[i+2],&line2[i+3],&line2[i+4],&line2[i+5],\
		       &line2[i+6],&line2[i+7],&line2[i+8],&line2[i+9],&line2[i+10],&line2[i+11],&line2[i+12]); 
		for(g=2;g<arrays_size;g++){
	 	   if (line2[g]==flag_num){
	 		   line2[g]=line1[g];
		    }
	    } 
	    fprintf(g2,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",line2[i],line2[i+1],line2[i+2],line2[i+3],line2[i+4],line2[i+5],\
	           line2[i+6],line2[i+7],line2[i+8],line2[i+9],line2[i+10],line2[i+11],line2[i+12]);
		for(g=0;g<arrays_size;g++){
	 		line1[g]=line2[g];
		} 
    }
    fclose(g1);
    fclose(g2);	
    return total_lines;
}

/*
void composite(double Command,double Ef,double El)
{
	double E;
	uint8_t comp_byte=207;
	if (Comp_material==1){
        if (Command == 1 || Command == 01 || Command == 0 || Command == 00 || Command==2 || Command==02 || Command==3 || Command==03){
            E=El-Ef;
        	if(first==false && E>0){
        		first=true; 
    		}
        	if(E==0 && first==true){
         		//fwrite(&comp_byte, sizeof(uint16_t),1,SD_binary_file);
         		write_hex2file(comp_byte);
        		first=false;
    		}
    	}
   }
	
}
*/
 
void LINE(double xf, double yf, double zf, double ef, double xl, double yl, double zl, double el, double FEEDRATE)
	{

    int dx=0,dy=0,dz=0,de=0,xs=0,ys=0,zs=0,es=0,p1=0,p2=0,p3=0,x1_last,y1_last,z1_last,e1_last,period;
    int stepx=0,stepy=0,stepz=0,stepe=0,x1=0,y1=0,z1=0,e1=0,x2=0,y2=0,z2=0,e2=0;
    double xmin,ymin,zmin,emin,LINE_DIST,E_DIST,LINE_STEP=0,l=0,XY_PLANE_RAD,ZX_PLANE_RAD,ZY_PLANE_RAD;
    
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
    FEEDRATE=line_accel_feed_limits(dx,dy,dz,de,FEEDRATE);
	xmin=1/STPU_X;
    ymin=1/STPU_Y;
	zmin=1/STPU_Z;
	emin=1/STPU_E;
    x1=round(xf/xmin); y1=round(yf/ymin); z1=round(zf/zmin); e1=round(ef/emin);
    x2=round(xl/xmin); y2=round(yl/ymin); z2=round(zl/zmin); e2=round(el/emin);
    x1_last=x1; y1_last=y1; z1_last=z1; e1_last=e1; ////BUG FIX
    dx = abs(x2 - x1);
    dy = abs(y2 - y1); 
    dz = abs(z2 - z1); 
    de = abs(e2 - e1);
    period=1000/CORE_FREQ;
	tmin=period*pow(10,-6);
    LINE_DIST=sqrt((pow(dx*xmin,2))+(pow(dy*xmin,2))+(pow(dz*xmin,2)));
    E_DIST=fabs(el-ef);
    if(LINE_DIST==0 && E_DIST==0){
    	return;
	}
	if(JMFEED>=FEEDRATE){
		cu=check_jfeed_limits(xl-xf,yl-yf,zl-zf,E_DIST);
		state=10;
	    if(LINE_DIST==0 && E_DIST!=0){
        	LINE_DIST=E_DIST;
        	E_DIST=0;
	    }
	}else{
		if(LINE_DIST==0 && E_DIST!=0){
        	LINE_DIST=E_DIST;
        	E_DIST=0;
	    }
		time_momments(LINE_DIST,FEEDRATE);
	}
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
            l += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            //printf("%i %i %i\n",stepx,x1,x1_last);
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,l);
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        }
    }else if (dy >= dx && dy >= dz && dy >= de){ 
	    LINE_STEP=LINE_DIST/dy;
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
            l += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,l);
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
            l += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,l);
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
            l += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,l);
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        }   	
	}		
 }

void ARC(bool input,double k,double l,double x1_f,double y1_f,double Ef,double x2_f,double y2_f,double El,double FEEDRATE)
   {
        
    double R,a_f,a_l,theta_first,theta_last,dtheta,ARC_DIST,E_DIST,Xmin,Ymin,Emin,THETA_STEP,theta,DX,DY,x,y,Xlast,Ylast,emin;
    double temp,theta_prev,L=0,L_last,DL,L_STEP,FEED_TEMP;
    int b,stepx=0,stepy=0,stepe=0,e_step=0,period;
    bool next_point_x=false,next_point_y=false,next_point_e=false;
    
		period=1000/CORE_FREQ;
    	tmin=period*pow(10,-6);
        x1_f=x1_f-k; x2_f=x2_f-k;
        y1_f=y1_f-l; y2_f=y2_f-l;
        k=0; l=0;
     	Xmin=1/STPU_X;
        Ymin=1/STPU_Y;
    	Emin=1/STPU_E;          
     	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
      	a_f=casequartile(x1_f,y1_f,k,l);
     	a_l=casequartile(x2_f,y2_f,k,l);
     	theta_first=theta_func(x1_f,y1_f,k,l,a_f,input);
     	theta_last=theta_func(x2_f,y2_f,k,l,a_l,input);
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
		//printf("%f %f\n",ACCEL_ERATION,FEEDRATE);
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
		if(Xmin<=Ymin){
            emin=(ARC_DIST*Emin)/fabs(E_DIST);
			if(input==true){
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
			if(input==true){
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
		THETA_STEP=pi/(2*b);
		L_STEP=fabs(THETA_STEP)*R;
		theta=theta_first+THETA_STEP;
		theta_prev=theta;
		if (theta_first>theta_last && input==false){
		    if(theta_last==0){
		    	theta_last=2*pi;
			}else{
    			temp=theta_last;
    			theta_last=2*pi;
    		}
		}else if(theta_first<theta_last && input==true && y2_f!=0){
    			temp=theta_last;
    			theta_last=0;	
		}
		Xlast=x1_f;
		Ylast=y1_f;	
        theta_last=theta_last+(THETA_STEP/2.0001); ///BUG FIX FOR THE LAST POINT
		while((theta<=theta_last && input==false) || (theta>=theta_last && input==true)){
			//L = fabs(theta-theta_first)*R;
			L+=L_STEP;
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
                wr2bin(stepx,stepy,0,stepe,L); 
			}
			theta_prev=theta;
			theta += THETA_STEP;
			if(input==false){
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
   }
 
int casequartile(double x,double y,double K,double L)
   {
	
    	int result;
    	if(x>=K && y>=L){
    			result=1;
    		}else if(x<=K && y>=L){
    			result=2;
    		}else if(x<=K && y<=L){
    			result=3;
    		}else if(x>=K && y<=L){
    			result=4;
    		}
    		return result;
    }
    
double theta_func(double x,double y,double k,double l,int a,bool input)
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
     	dek=-9;
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
         		//dt1=(umax-cu)/ACCEL_ERATION;
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
         			//printf("%s\n","TEEESTED!!...DONE");
        		}else{
         			dt2=(L-xa)/umax;
         			state=4;
         			t2=t1+dt1;
         			t3=t2+t1;
           			t4=t3+dt2;
         			t5=t4+t1;
         			t6=t5+dt1;
         			t7=t6+t1;
    			//printf("%s\n","TEEESTED!!...DONE");
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
                	    if(loop>50){ //reduce accuracy if the root cannot be calculated
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
	    			//printf("%s\n","TEEESTED!!...DONE");	
	    		}else{
    			   	dt=(L-xx)/umax;
	    		   	state=3;
    			   	t2=2.0*t1;
        		 	t3=t2+dt;
        	 		t4=t3+t1;
	         		t5=t4+t1;
	         		//printf("%s\n","TEEESTED!!...DONE");
			   }
    	   }else{
         	    t0=t1*umax/cu;
         	    t0=50;
    	        do{
    	            t=t0-(((2.0*JERK*pow(t0,3))+(4.0*JMFEED*t0)-L)/((4.0*JERK*pow(t0,2))+(4.0*JMFEED))); ////NEWTON-RAPHSON
                    error=fabs(t-t0);		
	                t0=t;
            	    loop++;
            	    if(loop>50){ //reduce accuracy if the root cannot be calculated
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
    				//printf("%s\n","TEEESTED!!...DONE");
    	   }
    	}
    		if(state==1){ ///DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(t1*cu)+(JMFEED*t1);
                x3_t3=x2_t2+x2_t2-x1_t1;
    			x4_t4=2.0*x2_t2;
    			//printf("%s\n","STATE1");
    		}else if(state==2){//DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			u2_t2=u1_t1+(ca*(t2-t1));
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(((u2_t2-u1_t1)*(t2-t1))/2.0)+(u1_t1*(t2-t1))+x1_t1;
    			x3_t3=(cu*(t3-t2))+(x2_t2-x1_t1)+(JMFEED*t1);
    			x4_t4=x3_t3+(x3_t3-x2_t2);
    			x5_t5=x4_t4+(x2_t2-x1_t1);
                x6_t6=2.0*x3_t3;
                 //printf("%s\n","STATE2");
    		}else if(state==3){ ////DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(t1*cu)+(t1*JMFEED);
    			x3_t3=(cu*(t3-t2))+x2_t2;
    			x4_t4=x3_t3+x2_t2-x1_t1;
    			x5_t5=x3_t3+x2_t2;
    		//	printf("%s\n","STATE3");
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
    			//printf("%s\n","STATE4");
    			//printf("%s %d %f %f %f %f %f %f %f\n","state=",state,x1_t1,x2_t2,x3_t3,x4_t4,x5_t5,x6_t6,x7_t7);
    			//x7_t7=(JERK*pow(t7,3)/6)-(JERK*pow(t7,3)/2)-(JERK*t7*pow(t6,2))+(JERK*t6*pow(t7,2))+(JERK*pow(t6,3)/3)-(JERK*t7*pow(t6,2)/2)+(u1_t1*(t7-t6))+x6_t6;
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
    		}else if(state==2){
    			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2);
    			x2_t2=umax*(t2-t1)+x1_t1;
    			x3_t3=x2_t2+x1_t1;
     		}else if(state==3){
    			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2);
    			x2_t2=2*x1_t1;
    		}	    	
		}
		//printf("%s %d %f %f %f %f %f %f %f\n","state=",state,t1,t2,t3,t4,t5,t6,t7);
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated 
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated 
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated 
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated 
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        		    if(loop>30){ //reduce accuracy if the root cannot be calculated
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
        int i=0,j,b;
        char bits[9];
        uint8_t byte_num,null_byte=0,timeboxes;
      	
       	for(j=0;j<=7;j++){ ///complete some bits with 0 for safety
       		bits[j]='0';
       	}
       	j=0;
        ///////////****complete the suitable bits 0,1 for step and direction in each axis****/////////////	
       	if(stepx==0){
            bits[6]='0';
            bits[7]='0';
       	}else if(stepx > 0){
            bits[6]='1';
            if(Invrt_X==0){
              	bits[7]='0';
			}else{
				bits[7]='1';
			}
       	}else if(stepx<0){
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
            bits[4]='1';
            if(Invrt_Y==0){
               	bits[5]='0';
			}else{
				bits[5]='1';
			}
       	}else if(stepy<0){
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
	    bits[8]='e';  
  	    byte_num=bits2val(bits); ///return the integer number of the previous calculated 8 bits
  	    time=L_time_calc(l);
        timeboxes=(time-last_time)/tmin; //total bytes in that period of time between this and the next pulse
        if(timeboxes<=0){
        	timeboxes=1;///FIX A TIME BUG
		}
   	    write_hex2file(timeboxes);
   	    write_hex2file(byte_num);
		//storage_counter=storage_counter+storage_step;
   	    b=0;
   	    last_time=time;
    }

void write_hex2file(uint8_t hex_value) //https://stackoverflow.com/questions/35390912/proper-way-to-get-file-size-in-c?rq=1
{
	struct stat buffer;
	struct stat child_buffer;
    int         status;
    int   child_status;
    uint16_t trash_byte=100;
    
    if(PRINT_STATE==0){
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
    		do{ //wait for a file to be free
    			status = wstat(flag_path, &buffer); //check flag state....stat replaced with wstat
                child_status = wstat(child_path, &child_buffer);
                if(child_buffer.st_size==0){ //check if Python GUI is terminated through a file signal
                    printf("%s\n","THE PRINTING PROCESS HAS BEEN TERMINATED BY EXTERNAL SIGNAL");
                	exit(0);
    			}
    		}while(buffer.st_size==0);
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
    }else{
    	fwrite(&hex_value, sizeof(uint8_t),1,SD_binary_file);
    	storage_counter=storage_counter+storage_step;
		file_buffer_size++;
		if(file_buffer_size==max_bufferfile_size){
			file_buffer_size=0;
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


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

#include "global_vars/globals.h"


double check_units(double Fl);                                                                                                  //convert to mm/sec everything
void crt_file();                                                                                                                //creates the output files
void wr2bin(int stepx, int stepy, int stepz, int stepe, double l);                                                              //writes the stop byte to output file
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

int main(){
	ABL_XCOORD = (float*)malloc(sizeof(float));
	ABL_YCOORD = (float*)malloc(sizeof(float));
	ABL_ZCOORD = (float*)malloc(sizeof(float));
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
    	
    	MG_data.test1=4;
    	MG_data.test2=4;
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
					if(HOME_X_ENABLE==1 && Xl==axis_num){MG_data.C=1;MG_data.R=1;Xl=T0_X_OFFSET;}else{MG_data.C=0;MG_data.R=0;}
					if(HOME_Y_ENABLE==1 && Yl==axis_num){MG_data.D=1;MG_data.S=1;Yl=T0_Y_OFFSET;}else{MG_data.D=0;MG_data.S=0;}
					if(HOME_Z_ENABLE==1 && Zl==axis_num){MG_data.E=1;MG_data.T=1;Zl=T0_Z_OFFSET;}else{MG_data.E=0;MG_data.T=0;}
					MG_data.F=HOME_X_STATE;
					MG_data.G=HOME_Y_STATE;
					MG_data.H=HOME_Z_STATE;
					MG_data.O=HOME_X_DURATION;
					MG_data.P=HOME_Y_DURATION;
					MG_data.Q=HOME_Z_DURATION;
					MG_data.U=T0_X_OFFSET*STPU_X;
					MG_data.V=T0_Y_OFFSET*STPU_Y;
					MG_data.W=T0_Z_OFFSET*STPU_Z;
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
                    if(e_space<sizeof(MG_data) && e_space!=0){
                    	//e_space=30;//fix a bug arduino side
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
				if(Ml==83){// M83 E Relative
					E_ABSOLUTE_POSITIONING=0;
				}
				if(Ml==226){// M226 pause until iteraction
					MG_data.A=6;
				}

				write_hex2file(GP);
				write_hex2file(GP);
                e_space=max_bufferfile_size-(file_buffer_size-1); ///-1 fix a bug arduino side
                if(e_space<sizeof(MG_data) && e_space!=0){
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
    file_buffer_size=file_buffer_size+sizeof(MG_data);
    buffer1_file=_wfopen(buffer1_path,L"wb");
    fwrite(&X_ENABLE_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&X_STEP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&X_DIR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&X_ENDSTOP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Y_ENABLE_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Y_STEP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Y_DIR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Y_ENDSTOP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Z_ENABLE_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Z_STEP_PIN, sizeof(int16_t),1,buffer1_file);
	fwrite(&Z_DIR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Z_ENDSTOP_PIN, sizeof(int16_t),1,buffer1_file);
	fwrite(&Z1_ENABLE_PIN, sizeof(int16_t),1,buffer1_file);
	fwrite(&Z1_STEP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Z1_DIR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&Z1_ENDSTOP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&E_ENABLE_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&E_STEP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&E_DIR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&E_ENDSTOP_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&N_HEATER_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&N_SENSOR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&N_FAN_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&B_HEATER_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&B_SENSOR_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&FAN_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&SERVO1_PIN, sizeof(int16_t),1,buffer1_file);
    fwrite(&SERVO2_PIN, sizeof(int16_t),1,buffer1_file);
    
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
    /*
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
    
  
double check_units(double Fl)
{
	if (UNITS==1){
        Fl=Fl/60;
	}
	return Fl;
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

void hidecursor() //    https://stackoverflow.com/questions/30126490/how-to-hide-console-cursor-in-c
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
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


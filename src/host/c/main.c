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


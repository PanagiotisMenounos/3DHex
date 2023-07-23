#include "../global_vars/globals.h"

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


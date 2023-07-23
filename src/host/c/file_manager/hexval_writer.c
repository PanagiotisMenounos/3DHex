#include "../global_vars/globals.h"

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


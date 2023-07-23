#include "../global_vars/globals.h"

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

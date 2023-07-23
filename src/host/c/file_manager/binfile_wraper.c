#include "../global_vars/globals.h"

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

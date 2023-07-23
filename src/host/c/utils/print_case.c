#include "../global_vars/globals.h"

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

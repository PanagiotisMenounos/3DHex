#include "../global_vars/globals.h"

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

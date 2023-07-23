#include "../global_vars/globals.h"

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


#include "../global_vars/globals.h"

void path_files()
{
    int i=0,j=0,f,p;
    wchar_t *appdata_path;
    appdata_path=_wgetenv(L"LOCALAPPDATA"); //https://msdn.microsoft.com/en-us/library/tehxacec(v=VS.100).aspx
    //SHGetFolderPath (0, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata_path);
	  
    while(appdata_path[i] != L'\0'){
   	    coordinates_path[j]=appdata_path[i];
		fcoordinates_path[j]=appdata_path[i];
		gc2info_path[j]=appdata_path[i];
		savepath_path[j]=appdata_path[i];
		gen_path[j]=appdata_path[i];
		gcode_path[j]=appdata_path[i];
		boxes_path[j]=appdata_path[i];
		cboxes_path[j]=appdata_path[i];
		dboxes_path[j]=appdata_path[i];
		buffer1_path[j]=appdata_path[i];
		buffer2_path[j]=appdata_path[i];
		flag_path[j]=appdata_path[i];
		startpy_path[j]=appdata_path[i];
		child_path[j]=appdata_path[i];
		fly_path[j]=appdata_path[i];
		abl_x_path[j]=appdata_path[i];
		abl_y_path[j]=appdata_path[i];
		abl_xyz_path[j]=appdata_path[i];
		pins_path[j]=appdata_path[i];
   		j++;
		i++;	   
    }
    for(f=0;f<19;f++){
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
             	fcoordinates_path[p]=savepath[f][i];
            	p++;
             	i++;
            }   		
		}
    	if(f==2){
		    while(savepath[f][i]!=L'\0'){
             	gc2info_path[p]=savepath[f][i];
            	p++;
             	i++;
            }     		
		}		
    	if(f==3){
		    while(savepath[f][i]!=L'\0'){
             	savepath_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==4){
		    while(savepath[f][i]!=L'\0'){
             	gen_path[p]=savepath[f][i];
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
             	boxes_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==7){
		    while(savepath[f][i]!=L'\0'){
             	cboxes_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==8){
		    while(savepath[f][i]!=L'\0'){
             	dboxes_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==9){
		    while(savepath[f][i]!=L'\0'){
             	buffer1_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
    	if(f==10){
		    while(savepath[f][i]!=L'\0'){
             	buffer2_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}	
    	if(f==11){
		    while(savepath[f][i]!=L'\0'){
             	flag_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==12){
		    while(savepath[f][i]!=L'\0'){
             	startpy_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==13){
		    while(savepath[f][i]!=L'\0'){
             	child_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==14){
		    while(savepath[f][i]!=L'\0'){
             	fly_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==15){
		    while(savepath[f][i]!=L'\0'){
             	abl_x_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==16){
		    while(savepath[f][i]!=L'\0'){
             	abl_y_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==17){
		    while(savepath[f][i]!=L'\0'){
             	abl_xyz_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}
		if(f==18){
		    while(savepath[f][i]!=L'\0'){
             	pins_path[p]=savepath[f][i];
            	p++;
             	i++;
            }        		
		}																										
	}
}
  

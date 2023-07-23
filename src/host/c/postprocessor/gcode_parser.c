#include "../global_vars/globals.h"

unsigned long gc2info(double flag_num)
{
    #define arrays_size 13
    char string [200],letters[arrays_size]={'G','M','X','Y','Z','I','J','E','F','S','P','R','T'},buf[10]; 
	unsigned long i=0,j,g,poslet=0,total_lines=0,n=0,dex=0,temp_pos=0;
    double line1[arrays_size],line2[arrays_size],trash=0;
    bool first_line1=true,first_line2=true,notfound=true,found_axis=false,write_value=false,write_axis_num=true,home=false,abl_proc=false;
    int threshold_pos=7,cur=0,emb=1;
    bool home_x=false,home_y=false,home_z=false;
	FILE *fp;
	fp=_wfopen(gcode_path,L"r");
	FILE *g1;
	FILE *g2;
    g1=_wfopen(gc2info_path,L"w");	//the results are written in this fille
    fprintf(g1,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash,trash);
    total_lines++;
    while (fgets(string, 200, fp) != NULL){//TILL THE END OF FILE
	   if (string[0]=='G' || string[0]=='M'){
	        total_lines++;
			cur=0;	
			while (cur<emb){
			cur++;							  
		    for(poslet=0;poslet<arrays_size;poslet++){// check for each letter		  
		        while (string[i] != '\0' && string[i] != ';' ){ //till the end of line
		            if ((string[i]) == (letters[poslet])){ //
			            notfound=false; //=found letter
			            j=i+1;
			            write_value=false;
			            while(string[j] != ' ' && string[j] != '\0' && string[j] != ';' && string[j] != '\n'){ //until next character
			            	write_value=true; //number found
						    if (string[j] != '[' && string[j] !=']'){ //extract the number
			                    fprintf(g1,"%c",string[j]);
			                }
						    j++;	
					    }
					    if(write_value==false){ //no number extracted LETTERS XYZE 
                           //printf("%s\n","AXIS FOUND");
                           fprintf(g1,"%lf",axis_num); 
						}
			        }
			        i++;
				}
				if(notfound==true){ //if letter not found
		            if(string[0]=='G' && string[1]=='2' && string[2]=='8' && (letters[poslet]=='X' || letters[poslet]=='Y' || letters[poslet]=='Z')){ //this is valid when G28 X || Y || Z
		                home=true;
		            	temp_pos=0;
		            	while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            		 if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z'){
		            		 	if(string[temp_pos]=='X'){ //This is true only for G92 to know which axis to zero
		            		 		home_x =true;
								 }else if(string[temp_pos]=='Y'){ //This is true only for G92 to know which axis to zero
								 	home_y =true;
								 }else if(string[temp_pos]=='Z'){ //This is true only for G92 to know which axis to zero
								 	home_z =true;
								 }
		            		 	write_axis_num=false;
							 }
							 temp_pos++;
		            	}
		            	if(write_axis_num==true){
		            		fprintf(g1,"%lf",axis_num);
		            		write_axis_num=true;
						}else{
							fprintf(g1,"%lf",flag_num);
						}
					}else if((string[0]=='M' && string[1]=='2' && string[2]=='0' && string[3]=='5')){ //M205
					       if(letters[poslet]=='S' || letters[poslet]=='J'){
					       	fprintf(g1,"%lf",axis_num);
						}else{
							fprintf(g1,"%lf",flag_num);
						}
					}else if(string[0]=='G' && string[1]=='9' && string[2]=='2' && (letters[poslet]=='X' || letters[poslet]=='Y' || letters[poslet]=='Z' || letters[poslet]=='E')){ //G92
		            	temp_pos=0;
		            	while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            		 if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z' || letters[poslet]=='E'){
		            		 	write_axis_num=false;
							 }
							 temp_pos++;
		            	}
		            	if(write_axis_num==true){
		            		fprintf(g1,"%lf",axis_num);
		            		write_axis_num=true;
						}else{
							fprintf(g1,"%lf",flag_num);
						}							
					}else if(string[0]=='M' && ((string[1]=='1' && string[2]=='7') || (string[1]=='1' && string[2]=='8') || (string[1]=='8' && string[2]=='4')) && (letters[poslet]=='X' || letters[poslet]=='Y' || letters[poslet]=='Z' || letters[poslet]=='E')){ //G92
		            	temp_pos=0;
		            	while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            		 if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z' || string[temp_pos]=='E'){
		            		 	write_axis_num=false;
							 }
							 temp_pos++;
		            	}
		            	if(write_axis_num==true){
		            		fprintf(g1,"%lf",axis_num);
		            		write_axis_num=true;
						}else{
							fprintf(g1,"%lf",flag_num);
						}						
					}else{
							fprintf(g1,"%lf",flag_num);
					}			        
			    }
				if(string[0]=='G' && string[1]=='2'&& string[2]=='9' && string[3]=='2'&& string[4]=='9'){
					   abl_proc=true;	
		    	}
	            j=0;
		        i=0;
		        notfound=true;
		        found_axis=false;
		        fprintf(g1,"%c",' ');
 	        }
 	        if(string[0]=='G' && string[1]=='2' && string[2]=='8' && home==false){ //this is valid when G28 X && Y && Z
		        home=true;
		        temp_pos=0;
		        while(string[temp_pos] != '\0' && string[temp_pos] != ';' && string[temp_pos] != '\n'){
		            if(string[temp_pos]=='X' || string[temp_pos]=='Y' || string[temp_pos]=='Z'){
		            	if(string[temp_pos]=='X'){ //This only for G92 to know which axis to zero
		            		home_x =true;
						}else if(string[temp_pos]=='Y'){ //This only for G92 to know which axis to zero
							home_y =true;
						}else if(string[temp_pos]=='Z'){ //This only for G92 to know which axis to zero
							home_z =true;
						}
		            	write_axis_num=false;
					}
					temp_pos++;
		        }
		        if(write_axis_num==true){
		            fprintf(g1,"%lf",axis_num);
		            write_axis_num=true;
				}else{
					fprintf(g1,"%lf",flag_num);
				}
			}
 	        if (home==true){
 	        	total_lines++;
				emb=2;
				home=false;				
				if(home_x==false && home_y==false && home_z==false){
			        fprintf(g1, "%c\n",' ' );
				  	strcpy(string,"G92 X"); 
				  	gcvt(T0_X_OFFSET, 7, buf);
				 	strcat(string, buf);
				  	strcat(string, " Y");
				  	gcvt(T0_Y_OFFSET, 7, buf);
				  	strcat(string, buf);
				  	strcat(string, " Z");
				  	gcvt(T0_Z_OFFSET, 7, buf);
				 	strcat(string, buf);
				  	home_x =false;
					home_y =false;
					home_z =false;
                }
                if(home_x==true){
                	fprintf(g1, "%c\n",' ' );
				  	strcpy(string,"G92 X"); 
				  	gcvt(T0_X_OFFSET, 7, buf);
				 	strcat(string, buf);
				}
				if(home_y==true){
					if (home_x==false){
						fprintf(g1, "%c\n",' ' );
				  	    strcpy(string,"G92 Y"); 
					}else{
						strcat(string," Y");
					}
				  	gcvt(T0_Y_OFFSET, 7, buf);
				 	strcat(string, buf);	
				}
				if(home_z==true){
					if(home_x==false && home_y==false){
					    fprintf(g1, "%c\n",' ' );
				  	    strcpy(string,"G92 Z"); 
					}else{
						strcat(string," Z");	
					}
				  	gcvt(T0_Z_OFFSET, 7, buf);
				 	strcat(string, buf);
				 							
				}
				strcat(string, " \n");
				home_x =false;
				home_y =false;
				home_z =false;
			}else{
			    emb=1;
			}
 	        }
 	        first_line1=false;
            fprintf(g1, "%c\n",' ' );
        }
        if(string[0]=='G' && string[1]=='9' && string[2]=='1' ){
             ABSOLUTE_POSITIONING=0;
		}
        if(string[0]=='G' && string[1]=='9' && string[2]=='0' ){
             ABSOLUTE_POSITIONING=1;
		}
        if(string[0]=='G' && ((string[1]=='2' || string[1]=='3') || ((string[1]=='0' && string[1]=='2') || (string[2]=='0' && string[2]=='3'))) && ABSOLUTE_POSITIONING==0){
            //printf("%s\n","G2/G3 does not support relative move mode");
            exit(0);
		}
    }
    fclose(g1);
    fclose(fp);
    g1=_wfopen(gc2info_path,L"r");
    g2=_wfopen(coordinates_path,L"w");   
    i=0;
    for(j=1;j<total_lines;j++){
		if(first_line2==true){
		    first_line2=false;
	        fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&line1[i],&line1[i+1],&line1[i+2],&line1[i+3],&line1[i+4],&line1[i+5],\
		           &line1[i+6],&line1[i+7],&line1[i+8],&line1[i+9],&line1[i+10],&line1[i+11],&line1[i+12]);
				  	
	        fprintf(g2,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",line1[i],line1[i+1],line1[i+2],line1[i+3],line1[i+4],line1[i+5],\
	    	        line1[i+6],line1[i+7],line1[i+8],line1[i+9],line1[i+10],line1[i+11],line1[i+12],trash);	
		}
        fscanf(g1,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&line2[i],&line2[i+1],&line2[i+2],&line2[i+3],&line2[i+4],&line2[i+5],\
		       &line2[i+6],&line2[i+7],&line2[i+8],&line2[i+9],&line2[i+10],&line2[i+11],&line2[i+12]);
	    if(line2[0]==91){ //Relative positioning
	    	ABSOLUTE_POSITIONING=0;
			E_ABSOLUTE_POSITIONING=0;
			threshold_pos=7;
		}
	    if(line2[0]==90){ //Absolute positioning
	    	ABSOLUTE_POSITIONING=1;
			E_ABSOLUTE_POSITIONING=1;
			threshold_pos=7;
		}
	    if(line2[1]==82){ //Absolute E positioning
			E_ABSOLUTE_POSITIONING=1;
			threshold_pos=6;
		}
	    if(line2[1]==83){ //Relative E positioning
			E_ABSOLUTE_POSITIONING=0;
			threshold_pos=7;
		}
		for(g=2;g<arrays_size;g++){ ////NOT SURE for this
		    if (line2[g]==flag_num && ABSOLUTE_POSITIONING==1 && E_ABSOLUTE_POSITIONING==0 && g==threshold_pos){ //Only E in relative
	 		   line2[g]=0;
		    }
	 	   if (line2[g]==flag_num && (ABSOLUTE_POSITIONING==1 || (ABSOLUTE_POSITIONING==0 && g>threshold_pos))){ //Absololute or Realtive without XYZE
	 		   line2[g]=line1[g];
		    }
		    if (line2[g]==flag_num && ABSOLUTE_POSITIONING==0 && g<=threshold_pos){ //Relative positioning for XYZE
	 		   line2[g]=0;
		    }
		    
	    }
	    fprintf(g2,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",line2[i],line2[i+1],line2[i+2],line2[i+3],line2[i+4],line2[i+5],\
	           line2[i+6],line2[i+7],line2[i+8],line2[i+9],line2[i+10],line2[i+11],line2[i+12],trash);
		for(g=0;g<arrays_size;g++){
			if(line2[g]!= axis_num){
				line1[g]=line2[g];
			}
		} 
    }
    fclose(g1);
    fclose(g2);	
    return total_lines;
}
 

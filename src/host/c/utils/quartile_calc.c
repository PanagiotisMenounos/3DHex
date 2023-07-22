#include "../global_vars/globals.h"

int casequartile(double x,double y,double K,double L, int point)
   {
    	int result;
    	if(x>=K && y>=L){
    		   result=1;
               if(y==L && point==1 && clockwise==true){
                 	result=4;
			   }
    			
    		}else if(x<=K && y>=L){
    			result=2;
    		}else if(x<=K && y<=L){
    			result=3;
    		}else if(x>=K && y<=L){
    			result=4;
    		}
    		return result;
    }

#include "../global_vars/globals.h"

double theta_func(double x,double y,double k,double l,int a)
    {
    	double theta,dx,dy;
    	dy=fabs(y-l);
      	dx=fabs(x-k);
    	if(a==1){
      		theta=atan(dy/dx);
    	}else if(a==2){
    		theta=pi-atan(dy/dx);
    	}else if(a==3){
    		theta=(3.0*pi/2.0)-atan(dx/dy);
    	}else if(a==4){
    		theta=(2.0*pi)-atan(dy/dx);
    	}
    	return theta;
    }

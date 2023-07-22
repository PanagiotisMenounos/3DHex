#include "../global_vars/globals.h"

double line_accel_feed_limits(double dx,double dy,double dz,double E_DIST,double FEEDRATE)
  {
	double ux,uy,uz,ue,div,Dux,Duy,Duz,Due,ax,ay,az,ae,Dax,Day,Daz,Dae;
	dx=fabs(dx);
	dy=fabs(dy);
	dz=fabs(dz);
	if((sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2)))!=0){
		div=E_DIST/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	}else{
		div=1;
	}
	ux=dx*FEEDRATE/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uy=dy*FEEDRATE/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uz=dz*FEEDRATE/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	ue=div*FEEDRATE;
	Dux=ux-MAX_FX;
	Duy=uy-MAX_FY;
	Duz=uz-MAX_FZ;
	Due=ue-MAX_FE;
	if(Dux>0 || Duy>0 || Duz>0 || Due>0){
    	if(Dux>Duy){
    		if(Dux>Duz){
    			if(Dux>Due){
        			FEEDRATE=MAX_FX*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dx;
     			}else{
    				FEEDRATE=MAX_FE/div;
    			}
    		}else{
    			if(Duz>Due){
    				FEEDRATE=MAX_FZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
    				FEEDRATE=MAX_FE/div;
    			}
    		}
    	}else{
      		if(Duy>Duz){
    			if(Duy>Due){
    				FEEDRATE=MAX_FY*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dy;
    			}else{
     				FEEDRATE=MAX_FE/div;
    			}
    		}else{
    			if(Duz>Due){
     				FEEDRATE=MAX_FZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
      				FEEDRATE=MAX_FE/div;
    			}
    		}
     	}
    }

	ax=ux*ACCELERATION/sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2));
	ay=uy*ACCELERATION/sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2));
	az=uz*ACCELERATION/sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2));
	ae=div*ACCELERATION;
	Dax=ax-MAX_ACCX;
	Day=ay-MAX_ACCY;
	Daz=az-MAX_ACCZ;
	Dae=ae-MAX_ACCE;		
	if(Dax>0 || Day>0 || Daz>0 || Dae>0){
    	if(Dax>Day){
    		if(Dax>Daz){
    			if(Dax>Dae){
        			ACCEL_ERATION=MAX_ACCX*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/ux;
     			}else{
    				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}else{
    			if(Daz>Dae){
    				ACCEL_ERATION=MAX_ACCZ*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/uz;
    			}else{
    				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}
    	}else{
      		if(Day>Daz){
    			if(Day>Dae){
    				ACCEL_ERATION=MAX_ACCY*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/uy;
    			}else{
     				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}else{
    			if(Daz>Dae){
     				ACCEL_ERATION=MAX_ACCZ*sqrt(pow(ux,2)+pow(uy,2)+pow(uz,2))/uz;
    			}else{
      				ACCEL_ERATION=MAX_ACCE/div;
    			}
    		}
     	}
    }else{
    	ACCEL_ERATION=ACCELERATION;
	}
	return FEEDRATE;  	
  }
  
double arc_accel_feed_limits(double R,double ARC_DIST,double E_DIST,double FEEDRATE)
{
	double ue,div,Dux,Duy,Due,ae,Dax,Day,Dae,theta,a,ak;
	E_DIST=fabs(E_DIST);
	ak=pow(FEEDRATE,2)/R; //centripetal acceleration due to feedrate
	theta=atan(ACCELERATION/ak); //save the ratio=(angle) linear-centripetal acceleration
	a=sqrt(pow(ak,2)+pow(ACCELERATION,2)); //resultant acceleration 
	Dax=a-MAX_ACCX;
	Day=a-MAX_ACCY;	
	if(Dax>0 || Day>0){
        	if(Dax>Day){ //weaker axis is x and we must reduce (lnear and centripetal acceleration)=a must be < ax without changing the ratio
        		ACCEL_ERATION=sin(theta)*MAX_ACCX; 
        		ak=cos(theta)*MAX_ACCX;
        		FEEDRATE=sqrt(ak*R);
			}else{  //weaker axis is y and we must reduce (lnear and centripetal acceleration)=a must be <ay without changing the ratio
			    ACCEL_ERATION=sin(theta)*MAX_ACCY;
        		ak=cos(theta)*MAX_ACCY;
        		FEEDRATE=sqrt(ak*R);
			}
    }else{
    	ACCEL_ERATION=ACCELERATION; //into acceleration axes limits (good case)
	}
	
	if(ARC_DIST!=0){
	    div=E_DIST/ARC_DIST; 
    }else{
    	div=1;
	}

	ue=div*FEEDRATE;
	Dux=FEEDRATE-MAX_FX;
	Duy=FEEDRATE-MAX_FY;
	Due=ue-MAX_FE;
	if(Dux>0 || Duy>0 || Due>0){ //we dont want to exceed feedrate limits,in this case only the centripetal acceleration change => ratio change
    	if(Dux>Duy){
    		if(Dux>Due){
    			FEEDRATE=MAX_FX;
			}else{
				FEEDRATE=MAX_FE/div;
			}
        }else{
        	if(Duy>Due){
        		FEEDRATE=MAX_FY;
			}else{
				FEEDRATE=MAX_FE/div;
			}
		}
    }
    ae=div*ACCEL_ERATION;
	Dae=ae-MAX_ACCE;		
	if(Dae>0){ //check the linear acceleration limits in E axis, only linear acceleration change => ratio change
        ACCEL_ERATION=MAX_ACCE/div;
    }   
	return FEEDRATE;  
	
}  

double check_jfeed_limits(double dx,double dy,double dz,double E_DIST)
{
	double ux,uy,uz,ue,div,Dux,Duy,Duz,Due,FEEDRATE;
	dx=fabs(dx);
	dy=fabs(dy);
	dz=fabs(dz);
	if(sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))!=0){
		div=E_DIST/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	}else{
		div=1;
	}
	ux=dx*JMFEED/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uy=dy*JMFEED/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	uz=dz*JMFEED/sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
	ue=div*JMFEED;
	Dux=ux-MAX_JFX;
	Duy=uy-MAX_JFY;
	Duz=uz-MAX_JFZ;
	Due=ue-MAX_JFE;
	if(Dux>0 || Duy>0 || Duz>0 || Due>0){
    	if(Dux>Duy){
    		if(Dux>Duz){
    			if(Dux>Due){
        			FEEDRATE=MAX_JFX*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dx;
     			}else{
    				FEEDRATE=MAX_JFE/div;
    			}
    		}else{
    			if(Duz>Due){
    				FEEDRATE=MAX_JFZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
    				FEEDRATE=MAX_JFE/div;
    			}
    		}
    	}else{
      		if(Duy>Duz){
    			if(Duy>Due){
    				FEEDRATE=MAX_JFY*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dy;
    			}else{
     				FEEDRATE=MAX_JFE/div;
    			}
    		}else{
    			if(Duz>Due){
     				FEEDRATE=MAX_JFZ*sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/dz;
    			}else{
      				FEEDRATE=MAX_JFE/div;
    			}
    		}
     	}
    }else{
    	FEEDRATE=JMFEED;
	}   
	return FEEDRATE;  		
}



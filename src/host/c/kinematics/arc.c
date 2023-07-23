#include "../global_vars/globals.h"
#include "../kinematics/velocity_profile.h"
#include "../kinematics/limits.h"
#include "../utils/angle_calc.h"
#include "../utils/min_max.h"
#include "../utils/quartile_calc.h"
#include "../file_manager/axes_bits_calc.h"

void ARC(bool clockwise,double k,double l,double x1_f,double y1_f,double Ef,double x2_f,double y2_f,double El,double FEEDRATE)
   {
        
    double R,a_f,a_l,theta_first,theta_last,dtheta,ARC_DIST,E_DIST,Xmin,Ymin,Emin,THETA_STEP,theta,DX,DY,x,y,Xlast,Ylast,emin;
    double temp,theta_prev,L=0,L_last,DL,L_STEP,FEED_TEMP,L_prev=0;
    int b,stepx=0,stepy=0,stepe=0,e_step=0,period;
    bool next_point_x=false,next_point_y=false,next_point_e=false;
        double yy,xx,check=0;
        int cor_counter=0;
		period=1000/CORE_FREQ;
    	tmin=period*pow(10,-6);
        x1_f=x1_f-k; x2_f=x2_f-k;
        y1_f=y1_f-l; y2_f=y2_f-l;
        yy=y1_f*STPU_Y;
        xx=x1_f*STPU_X;
        k=0; l=0;
     	Xmin=1/STPU_X;
        Ymin=1/STPU_Y;
    	Emin=1/STPU_E;         
     	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
      	a_f=casequartile(x1_f,y1_f,k,l,1);
     	a_l=casequartile(x2_f,y2_f,k,l,2);
     	theta_first=theta_func(x1_f,y1_f,k,l,a_f);
     	theta_last=theta_func(x2_f,y2_f,k,l,a_l);
     	dtheta=fabs(theta_last-theta_first);
 		if(dtheta>pi/2.0){
			dtheta=2.0*pi-dtheta;
		}
		ARC_DIST=dtheta*R; ///calculate total distance of the ar
		if(ARC_DIST==0){ //FIX A BUG
			return;
		}
		E_DIST=El-Ef;
		FEEDRATE=arc_accel_feed_limits(R,ARC_DIST,E_DIST,FEEDRATE);
		if(CURVE_DETECTION==1){
    	    if(new_CURVE==1){
    	    	trajectory_POINT=0; //reset
    	    	if(JMFEED>=FEEDRATE){
                	cu=FEEDRATE;//for safety reasons
            		state=10; //NO ACCElERATION or DECELERATION	    		
    			}else{
    				time_momments(gen_DISTANCE,FEEDRATE);
    			}
    		}
    	}else{
    		trajectory_POINT=0; //reset	
	        if(JMFEED>=FEEDRATE){
    	    	if(E_DIST!=0){
    	    		FEED_TEMP=min_n(MAX_JFX,MAX_JFY);
    	    		cu=min_n(FEED_TEMP,MAX_JFE);
    			}else{
    				cu=min_n(MAX_JFX,MAX_JFY);
    			}
    		    state=10;
    	    }else{
    		    time_momments(ARC_DIST,FEEDRATE);
     	    }
    	}
		if(Xmin<=Ymin){
            emin=(ARC_DIST*Emin)/fabs(E_DIST);
			if(clockwise==true){
				if(Xmin<=emin){
			        THETA_STEP=-dtheta*Xmin/ARC_DIST;
				}else{
					THETA_STEP=-dtheta*emin/ARC_DIST;
				}
    		}else{
				if(Xmin<=emin){
			        THETA_STEP=dtheta*Xmin/ARC_DIST;
				}else{
					THETA_STEP=dtheta*emin/ARC_DIST;
				}
    		}
		}else{
            emin=(ARC_DIST*Emin)/fabs(E_DIST);
			if(clockwise==true){
				if(Ymin<=emin){
			        THETA_STEP=-dtheta*Ymin/ARC_DIST;
				}else{
					THETA_STEP=-dtheta*emin/ARC_DIST;
				}
    		}else{
				if(Ymin<=emin){
			        THETA_STEP=dtheta*Ymin/ARC_DIST;
				}else{
					THETA_STEP=dtheta*emin/ARC_DIST;
				}
    		}
		}
		b=round(pi/(2.0*THETA_STEP));//BUG FIX...VALUE DRIFT BECAUSE OF PI
		THETA_STEP=pi/(2.0*b);
		theta=theta_first+THETA_STEP;
		theta_prev=theta;
		if (theta_first>theta_last && clockwise==false){
		    if(theta_last==0){
		    	theta_last=2*pi;
			}else{
    			temp=theta_last;
    			theta_last=2*pi;
    		}
		}else if(theta_first<theta_last && clockwise==true && y2_f!=0){
    			temp=theta_last;
    			theta_last=0;	
		}
		Xlast=x1_f;
		Ylast=y1_f;	
        theta_last=theta_last+(THETA_STEP/1.2); ///BUG FIX FOR THE LAST POINT
        L_STEP=Xmin;
		ARC_DIST = roundf(ARC_DIST*100)/100+(L_STEP/1.2); ///BUG FIX FOR THE LAST POINT
        L+=L_STEP;
		trajectory_POINT=trajectory_POINT+L_STEP;
		while(L<=ARC_DIST){
			x=R*cos(theta);
			y=R*sin(theta);	
			DX=x-Xlast;
			DY=y-Ylast;
			DL=L-L_last;
			if (fabs(DX)>=(Xmin/2.0)){
				next_point_x=true;
				if(DX>=0){
	    			x1_f += Xmin;
	    		}else{
	    			x1_f -= Xmin;
				}
			}
			if (fabs(DY)>=(Ymin/2.0)){
				next_point_y=true;
				if(DY>=0){
    				y1_f += Ymin;
        		}else{
        			y1_f -= Ymin;
				}
			}
			if(fabs(DL)>=(emin/2.0)){
				next_point_e=true;
				if(E_DIST>=0){
					e_step=1;
				}else{
				    e_step=-1;
				}
			}
			if(next_point_x==true || next_point_y==true || next_point_e==true){
                if (DX!=0 && next_point_x==true){ if(DX>0){stepx=1;}else{stepx=-1;}next_point_x=false;Xlast=x1_f;}else{ stepx=0; }
                if (DY!=0 && next_point_y==true){ if(DY>0){stepy=1;}else{stepy=-1;}next_point_y=false;Ylast=y1_f;}else{ stepy=0; }
                if (DL!=0 && next_point_e==true){ stepe=e_step;next_point_e=false;L_last+=emin;}else{ stepe=0; }
                wr2bin(stepx,stepy,0,stepe,trajectory_POINT);
				yy=yy+stepy; 
				xx=xx+stepx;
			}
			trajectory_POINT=trajectory_POINT+L_STEP;
			L+=L_STEP;
			theta_prev=theta;
			theta += THETA_STEP;
			if(clockwise==false){
    			if (theta>(pi/2.0) && theta_prev<(pi/2.0)){
    				theta=(pi/2.0);
    		    }else if(theta>pi && theta_prev<pi){
    				theta=(pi);
    			}else if(theta>(3.0*pi/2.0) && theta_prev<(3.0*pi/2.0)){
    				theta=(3*pi/2.0);				
    			}else if(theta>(2*pi) && y2_f!=0){
    				theta=0;
    				theta_last=temp+(THETA_STEP/2.0001);
    			}
	    	}else{
    			if (theta<(pi/2.0) && theta_prev>(pi/2.0)){
    				theta=(pi/2.0);
    		    }else if(theta<pi && theta_prev>pi){
      				theta=(pi);
    			}else if(theta<(3.0*pi/2.0) && theta_prev>(3.0*pi/2.0)){
    				theta=(3*pi/2.0);				
    			}else if(theta<0 && y2_f!=0){
    				theta=2*pi;
    				theta_last=temp+(THETA_STEP/2.0001);
    			}	    		
			}
		}
		trajectory_POINT -=L_STEP;
		//trajectory_POINT=trajectory_POINT-L_STEP;
		//printf("%s %f\n","TRAJECT",trajectory_POINT);
		xx=xx/STPU_X;
		yy=yy/STPU_Y;
		if((xx-x2_f!=0) || (yy-y2_f!=0)){
			embedded_line=true;
			LINE(xx,yy,0,0,x2_f,y2_f,0,0,FEEDRATE);
			embedded_line=false;
		}
   }
 

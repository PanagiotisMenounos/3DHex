#include "../global_vars/globals.h"

void LINE(double xf, double yf, double zf, double ef, double xl, double yl, double zl, double el, double FEEDRATE)
	{

    int dx=0,dy=0,dz=0,de=0,xs=0,ys=0,zs=0,es=0,p1=0,p2=0,p3=0,x1_last,y1_last,z1_last,e1_last,period;
    int stepx=0,stepy=0,stepz=0,stepe=0,x1=0,y1=0,z1=0,e1=0,x2=0,y2=0,z2=0,e2=0;
    double xmin,ymin,zmin,emin,LINE_DIST,E_DIST,LINE_STEP=0,XY_PLANE_RAD,ZX_PLANE_RAD,ZY_PLANE_RAD;

    
    if(XY_PLANE!=0){
    	XY_PLANE_RAD=XY_PLANE*pi/180;
    	yl=yl-(sin(XY_PLANE_RAD)*xl);
    	yf=yf-(sin(XY_PLANE_RAD)*xf);
    	xl=cos(XY_PLANE_RAD)*xl;
        xf=cos(XY_PLANE_RAD)*xf;    	
	}
    if(ZX_PLANE!=0 && (zl-zf)!=0){
    	ZX_PLANE_RAD=ZX_PLANE*pi/180;
    	zl=zl+(zl*(1-cos(ZX_PLANE_RAD)));
        zf=zf+(zf*(1-cos(ZX_PLANE_RAD)));
    	xl=sin(ZX_PLANE_RAD)*zl;
    	xf=sin(ZX_PLANE_RAD)*zf;    	
	}
    if(ZY_PLANE!=0 && (zl-zf)!=0){
    	ZY_PLANE_RAD=ZY_PLANE*pi/180;
    	zl=zl+(zl*(1-cos(ZY_PLANE_RAD)));
        zf=zf+(zf*(1-cos(ZY_PLANE_RAD)));
    	yl=sin(ZY_PLANE_RAD)*zl;
    	yf=sin(ZY_PLANE_RAD)*zf;  	
	}
    dx=xl-xf;
    dy=yl-yf;
    dz=zl-zf;
    de=el-ef;
    
    if(CURVE_DETECTION==0){
    	FEEDRATE=line_accel_feed_limits(dx,dy,dz,de,FEEDRATE); //Only working when CURVE_DETECTION is disabled
	}else{
		ACCEL_ERATION=ACCELERATION;
	}
	
    //printf("%s %f\n","feed",FEEDRATE);
	xmin=1/STPU_X;
    ymin=1/STPU_Y;
	zmin=1/STPU_Z;
	emin=1/STPU_E;
    x1=round(xf/xmin); y1=round(yf/ymin); z1=round(zf/zmin); e1=round(ef/emin);
    x2=round(xl/xmin); y2=round(yl/ymin); z2=round(zl/zmin); e2=round(el/emin);
    ABL_X = x2/STPU_X;
    ABL_Y = y2/STPU_Y;
    x1_last=x1; y1_last=y1; z1_last=z1; e1_last=e1; ////BUG FIX
    dx = abs(x2 - x1);
    dy = abs(y2 - y1); 
    dz = abs(z2 - z1); 
    de = abs(e2 - e1);
    printf("%s %i\n","EEE=",de);
    period=1000/CORE_FREQ;
	tmin=period*pow(10,-6);
    LINE_DIST=sqrt((pow(dx*xmin,2))+(pow(dy*ymin,2))+(pow(dz*zmin,2)));
    //printf("%s %f\n","LINE_traj",trajectory_POINT);
    //printf("%s %f\n","LINE",LINE_DIST);
    
    E_DIST=fabs(el-ef);
    if(LINE_DIST==0 && E_DIST==0){ //for safety
    	return;
	}
	if(LINE_DIST==0 && E_DIST!=0){
        LINE_DIST=E_DIST;
        E_DIST=0;
	}
	
	if(embedded_line==false){
    	if(CURVE_DETECTION==1){
    	    if(new_CURVE==1){
    	    	trajectory_POINT=0; //reset
    	    	if(JMFEED>=FEEDRATE){
                	cu=FEEDRATE;//for safety reasons
            		state=10; //NO ACCElERATION or DECELERATION	    		
    			}else{
    				//printf("%f %f\n",gen_DISTANCE,FEEDRATE);
    				time_momments(gen_DISTANCE,FEEDRATE);
    			}
    		}
    	}else{
        	if(JMFEED>=FEEDRATE){ //If this case is true then motion occurs with NO ACCElERATION or DECELERATION
            	cu=check_jfeed_limits(xl-xf,yl-yf,zl-zf,E_DIST);
        		state=10; //NO ACCElERATION or DECELERATION
        	}else{
    			time_momments(LINE_DIST,FEEDRATE);		
    		}
            trajectory_POINT=0; //reset		
    	}
    }
///////******  BRESENHAM LINE ALGORITHM    ****/////////
    if (x2 > x1){
        xs = 1;
    }else{ 
        xs = -1;
    }
    if (y2 > y1){ 
        ys = 1;
    }else{ 
        ys = -1;
    }
    if (z2 > z1){ 
        zs = 1;
    }else{ 
        zs = -1;
    }
    if (e2 > e1){
    	es=1;
	}else{
		es=-1;
	}
    if (dx >= dy && dx >= dz && dx>=de){  	
	    LINE_STEP=LINE_DIST/dx;
		if(embedded_line==true){
			LINE_STEP=0;
		}
        p1 = 2 * dy - dx; 
        p2 = 2 * dz - dx;
		p3 = 2 * de - dx; 
        while (x1 != x2){ 
            x1 += xs; 
            if (p1 >= 0){ 
                y1 += ys;
                p1 -= 2 * dx;
			}
            if (p2 >= 0){   
                z1 += zs;
                p2 -= 2 * dx; 
            }
            if (p3 >= 0){
            	e1 += es;
            	p3 -= 2 * dx;
			}
            p1 += 2 * dy; 
            p2 += 2 * dz; 
            p3 += 2 * de;
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point 
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        }
    }else if (dy >= dx && dy >= dz && dy >= de){ 
	    LINE_STEP=LINE_DIST/dy;
		if(embedded_line==true){
			LINE_STEP=0;
		}
        p1 = 2 * dx - dy;
        p2 = 2 * dz - dy;
		p3 = 2 * de - dy; 
        while (y1 != y2){ 
            y1 += ys;
            if (p1 >= 0){ 
                x1 += xs; 
                p1 -= 2 * dy; 
            }
            if (p2 >= 0){ 
                z1 += zs; 
                p2 -= 2 * dy; 
            }
            if (p3 >= 0){
            	e1 += es;
            	p3 -= 2 * dy;
			}
            p1 += 2 * dx; 
            p2 += 2 * dz;
			p3 += 2 * de; 
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;			
        } 
    }else if (dz >= dx && dz >= dy && dz >= de){
	    LINE_STEP=LINE_DIST/dz;         
        p1 = 2 * dy - dz; 
        p2 = 2 * dx - dz; 
        p3 = 2 * de - dz;
        while (z1 != z2){ 
            z1 += zs;
            if (p1 >= 0){ 
                y1 += ys; 
                p1 -= 2 * dz;
			}
            if (p2 >= 0){ 
                x1 += xs; 
                p2 -= 2 * dz; 
            }
            if (p3 >= 0){
            	e1 += es;
            	p3 -= 2 * dz;
			}
            p1 += 2 * dy;
            p2 += 2 * dx;
			p3 += 2 * de; 
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        }
    }else{
    	LINE_STEP=LINE_DIST/de;
        p1 = 2 * dx - de; 
        p2 = 2 * dy - de; 
        p3 = 2 * dz - de;
        while (e1 != e2){ 
            e1 += es;
            if (p1 >= 0){ 
                x1 += xs; 
                p1 -= 2 * de;
			}
            if (p2 >= 0){ 
                y1 += ys; 
                p2 -= 2 * de; 
            }
            if (p3 >= 0){
            	z1 += zs;
            	p3 -= 2 * de;
			}
            p1 += 2 * dx;
            p2 += 2 * dy;
			p3 += 2 * dz; 
            trajectory_POINT += LINE_STEP;
            stepx = x1 - x1_last; stepy = y1 - y1_last;
            stepz = z1 - z1_last; stepe = e1 - e1_last;
            if(dx==0){stepx=0;}if(dy==0){stepy=0;}if(dz==0){stepz=0;}if(de==0){stepe=0;} //FIXES A BUG
            wr2bin(stepx,stepy,stepz,stepe,trajectory_POINT); //resolve time calculation for current trajectory point
            x1_last = x1; y1_last = y1; z1_last = z1; e1_last = e1;
        } 	
	}	
 }

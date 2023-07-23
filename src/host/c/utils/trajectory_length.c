#include "../global_vars/globals.h"
#include "../utils/angle_calc.h"
#include "../utils/quartile_calc.h"

void curve_length(unsigned long total_lines)
{
	int j=0,loopa=0,a_f,a_l;
	double Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl=0,Sl,Pl,Rl,Tl,local_case=0,gen_feed,dx,dy,dz,de,gen_distance=0,reset_dist=1;
	bool first_time_tcoord=true,first_write=true;
	double xmin,ymin,zmin,emin,x1,x2,y1,y2,z1,z2,e1,e2;
	double x1_f,x2_f,y1_f,y2_f,k,l,R,theta_first,theta_last,dtheta;
	FILE *fcoord;
	FILE *gen;
	FILE *coord;
	
    fcoord=_wfopen(fcoordinates_path,L"r");
    gen=_wfopen(gen_path,L"w");
    while(j<total_lines){
    	if(first_time_tcoord){
    		first_time_tcoord=false;
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gf,&Mf,&Xf,&Yf,&Zf,&If,&Jf,&Ef,&Ff,&Sf,&Pf,&Rf,&Tf,&local_case);
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
    	    while(local_case!=1){
        	    Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
        	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
        	    j++;
			}
		}
		if((Gl==0 || Gl==00 || Gl==01 || Gl==02 || Gl==03 || Gl==1 || Gl==2 || Gl==3) && first_write==true){
			local_case=0;
			first_write=false;
		}
    	while(local_case==0 && (Gl==00  || Gl==01 || Gl==03 || Gl==02 || Gl==0  || Gl==1 || Gl==3 || Gl==2) && j<total_lines){
    		if(Gl==1 || Gl==01 || Gl==0 || Gl==00){
            	xmin=1/STPU_X;
                ymin=1/STPU_Y;
            	zmin=1/STPU_Z;
             	emin=1/STPU_E;
                x1=round(Xf/xmin); y1=round(Yf/ymin); z1=round(Zf/zmin); e1=round(Ef/emin);
                x2=round(Xl/xmin); y2=round(Yl/ymin); z2=round(Zl/zmin); e2=round(El/emin);
                dx = abs(x2 - x1);
                dy = abs(y2 - y1); 
                dz = abs(z2 - z1); 
                de = abs(e2 - e1);
                dx = dx*xmin;
                dy = dy*ymin;
                dz = dz*zmin;
                de = de*emin;
                if(dx!=0||dy!=0||dz!=0){
                	gen_distance=gen_distance+sqrt((pow(dx,2))+(pow(dy,2))+(pow(dz,2)));
    			}else if(de!=0){
    				gen_distance=gen_distance+fabs(de);
    			}
		    }
		    if(Gl==02 || Gl==03 || Gl==2 || Gl==3){
		    	k=Il;
		    	l=Jl;
                x1_f=Xf-k; x2_f=Xl-k;
                y1_f=Yf-l; y2_f=Yl-l;  
				k=0; l=0;      
             	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
            	a_f=casequartile(x1_f,y1_f,k,l,1);
            	a_l=casequartile(x2_f,y2_f,k,l,2);
             	theta_first=theta_func(x1_f,y1_f,k,l,a_f);
            	theta_last=theta_func(x2_f,y2_f,k,l,a_l);
            	dtheta=fabs(theta_last-theta_first);
        		if(dtheta>pi/2.0){
        			dtheta=2.0*pi-dtheta;
        		}
        		double THETA_STEP;
        		double ARC_DIST=dtheta*R; ///calculate total distance of the ar
        		ARC_DIST=roundf(ARC_DIST*100)/100;
         		gen_distance=gen_distance + ARC_DIST; ///calculate total distance of the ar		    	
			}
		   
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
    	    reset_dist=0;
    	    loopa=1;
    	    gen_feed=Ff;
    	}
    	if ((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=00 && Gl!=01 && Gl!=02 && Gl!=03)){
    		Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    	    fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
		}
    	if(loopa==1){
    	   fprintf(gen,"%lf " "%lf\n",gen_distance,gen_feed);
		   gen_distance=0;
		   loopa=0;
		   local_case=0;
	    }else if(Gl==0 || Gl==00 || Gl==01 || Gl==02 || Gl==03 || Gl==1 || Gl==2 || Gl==3){
    		if(Gl==0 || Gl==00 || Gl==01 || Gl==1){
            	xmin=1/STPU_X;
                ymin=1/STPU_Y;
            	zmin=1/STPU_Z;
             	emin=1/STPU_E;
                x1=round(Xf/xmin); y1=round(Yf/ymin); z1=round(Zf/zmin); e1=round(Ef/emin);
                x2=round(Xl/xmin); y2=round(Yl/ymin); z2=round(Zl/zmin); e2=round(El/emin);
                dx = abs(x2 - x1);
                dy = abs(y2 - y1); 
                dz = abs(z2 - z1); 
                de = abs(e2 - e1);
                dx = dx*xmin;
                dy = dy*ymin;
                dz = dz*zmin;
                de = de*emin;
                if(dx!=0||dy!=0||dz!=0){
                	gen_distance=gen_distance+sqrt((pow(dx,2))+(pow(dy,2))+(pow(dz,2)));
    			}else if(de!=0){
    				gen_distance=gen_distance+fabs(de);
    			}
		    }
		    if(Gl==02 || Gl==03 || Gl==2 || Gl==3){
		    	k=Il;
		    	l=Jl;
                x1_f=Xf-k; x2_f=Xl-k;
                y1_f=Yf-l; y2_f=Yl-l;
             	double Xmin=1/STPU_X;
                double Ymin=1/STPU_Y;
            	double Emin=1/STPU_E;    
				k=0; l=0;      
             	R=sqrt(pow((x2_f-k),2) + pow((y2_f-l),2)); ////calculate radius of circle
            	a_f=casequartile(x1_f,y1_f,k,l,1);
            	a_l=casequartile(x2_f,y2_f,k,l,2);
             	theta_first=theta_func(x1_f,y1_f,k,l,a_f);
            	theta_last=theta_func(x2_f,y2_f,k,l,a_l);
            	dtheta=fabs(theta_last-theta_first);
        		if(dtheta>pi/2.0){
        			dtheta=2.0*pi-dtheta;
        		}
        		double THETA_STEP;
        		double ARC_DIST=dtheta*R; ///calculate total distance of the ar
        		ARC_DIST=roundf(ARC_DIST*100)/100;
         		gen_distance=gen_distance + ARC_DIST; ///calculate total distance of the ar		    	
			}
        	if((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=01 && Gl!=00 && Gl!=02 && Gl!=03)){
        		gen_distance=0;
	    	}
	    	gen_feed=Fl;
        	fprintf(gen,"%lf " "%lf\n",gen_distance,gen_feed);
    		gen_distance=0;
    		Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    		fscanf(fcoord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&local_case);
    	    j++;
    	}
	}
	fclose(fcoord);
	fclose(gen);
}


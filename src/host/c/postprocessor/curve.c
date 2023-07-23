#include "../global_vars/globals.h"

void curve_detection(unsigned long total_lines)
{
	int j=0,last_last_printing_move=0,last_printing_move=0,printing_move=0,breeak=0;
	double theta_temp,theta1,theta2,smart_case,last_smart_case=1,trash,FS,last_curve=0,new_curve=0,af,al,theta;
	double Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl,Sl,Pl,Rl,Tl;
	bool first_smart_line=true,next_line=false;
	FILE *coord;
	FILE *final;
    coord=_wfopen(coordinates_path,L"r");
    final=_wfopen(fcoordinates_path,L"w");
    fscanf(coord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gf,&Mf,&Xf,&Yf,&Zf,&If,&Jf,&Ef,&Ff,&Sf,&Pf,&Rf,&Tf,&trash); 
    j++;
    fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,trash);
    do{ //FIX BUG WHEN INITIAL GCODE HAS MANY NON MOVE GCODE
    	fscanf(coord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&trash);
    	if(Gl!=1 && Gl!=01 && Gl!=0 && Gl!=00 && Gl!=02 && Gl!=03 && Gl!=02 && Gl!=03){
    		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gl,Ml,Xl,Yl,Zl,Il,Jl,El,Fl,Sl,Pl,Rl,Tl,trash);
		}
		j++; 
	}while(Gl!=1 && Gl!=01 && Gl!=0 && Gl!=00 && Gl!=02 && Gl!=03 && Gl!=02 && Gl!=03 && j<total_lines);
	if(Gl==1 || Gl==01 || Gl==0 || Gl==00){
        if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
        	last_printing_move=1;
    	}else{
			last_printing_move=0;
    	}
    	last_curve=1;
    	theta1 = curve_lines_angles(Xf,Yf,Xl,Yl,Gl);
    }else if(Gl == 02 || Gl == 03 || Gl == 02|| Gl == 03){
       	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
       		printing_move=1;
   		}else{
   			printing_move=0;
   		}
		theta1 = curve_lines_angles(Il,Jl,Xl,Yl,Gl);
		last_curve=1;	    		
	}else{
		last_printing_move=0;
    	last_curve=0;	
	}
	if(Gl==91){// G91 Relative
		ABSOLUTE_POSITIONING=0;
		E_ABSOLUTE_POSITIONING=0;
	}
	if(Gl==90){// G90 Absolute
		ABSOLUTE_POSITIONING=1;
		E_ABSOLUTE_POSITIONING=1;
	}
	if(Ml==82){// M82 E Absolute
		E_ABSOLUTE_POSITIONING=1;
	}
	if(Ml==83){// M83 E Relative
		E_ABSOLUTE_POSITIONING=0;
	}
	Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl;
    while(j<total_lines){
    	j++;
        fscanf(coord,"%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf" "%lf",&Gl,&Ml,&Xl,&Yl,&Zl,&Il,&Jl,&El,&Fl,&Sl,&Pl,&Rl,&Tl,&trash);    
    	if(Gl==91){// G91 Relative
    	    Xl=0; Yl=0; Zl=0; El=0;
    		ABSOLUTE_POSITIONING=0;
    		E_ABSOLUTE_POSITIONING=0;
    	}
    	if(Gl==90){// G90 Absolute
    	    Xl=Xf; Yl=Yf; Zl=Zf; El=Ef;
    		ABSOLUTE_POSITIONING=1;
    		E_ABSOLUTE_POSITIONING=1;
    	}
		if(Ml==82){// M82 E Absolute
		    El=0;
			E_ABSOLUTE_POSITIONING=1;
		}
		if(Ml==83){// M83 E Relative
		    El=Ef;
			E_ABSOLUTE_POSITIONING=0;
		}
		if(Gl==92 && ABSOLUTE_POSITIONING==0){ //G92 set position
			Xl=Xl-Xf;
            Yl=Yl-Yf;
            Zl=Zl-Zf;
		}
		//if(Gl==92 && E_ABSOLUTE_POSITIONING==0){
			//El=El-Ef;
		//}
        if(ABSOLUTE_POSITIONING==0){
            Xl=Xf+Xl;
            Yl=Yf+Yl;
            Zl=Zf+Zl;
		}
	    //if(E_ABSOLUTE_POSITIONING==0){
            //El=Ef+El;
		//}		
		if((Gl==0 || Gl==1 || Gl==2 || Gl==3 || Gl==00 || Gl==01 || Gl==02 || Gl==03) && (Gf==0 || Gf==1 || Gf==2 || Gf==3 || Gf==00 || Gf==01 || Gf==02 || Gf==03)){
			if(Gl==1){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
		    	theta2 = curve_lines_angles(Xf,Yf,Xl,Yl,Gl);
	    	}
	    	if(Gl == 2 || Gl == 3 || Gl == 02|| Gl == 03){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
				theta2 = curve_lines_angles(Il,Jl,Xf,Yf,Gl);
				theta_temp = curve_lines_angles(Il,Jl,Xl,Yl,Gl);	    		
			}
        	if(last_printing_move==printing_move && Fl==Ff){
                theta=fabs(theta2-theta1);
                if(theta>180){
                 	theta=360-theta;
		     	}
                if(theta<=ANGLE_THRESHOLD && ((Xf!=Xl || Yf!=Yl))){
		    	  new_curve=0;
		        }else{
	    	       new_curve=1;
		     	}
	    	}else{
	    	     new_curve=1;  	
			}
		    fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
		    last_last_printing_move=last_printing_move;
		    last_printing_move=printing_move;
		    last_curve=new_curve;
		    theta1=theta2;
		    if(Gl==2 || Gl==3 || Gl==02 || Gl==03){
		    	theta1=theta_temp;
			}
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle
    	}else if((Gf==0 || Gf==1 || Gf==2 || Gf==3 || Gf==00 || Gf==01 || Gf==02 || Gf==03) && ((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=01 && Gl!=00 && Gl!=02 && Gl!=03))){
        	printing_move=0;
        	new_curve=0;
    		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
    		last_printing_move=printing_move;
    		last_curve=new_curve;
    		theta1=theta2;
    		if(Gl==2 || Gl==3 || Gl==02 || Gl==03){
    		    theta1=theta_temp;
    		}
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle    		
		}else if(((Gf!=1 && Gf!=0 && Gf!=2 && Gf!=3) || (Gf!=01 && Gf!=00 && Gf!=02 && Gf!=03)) && (Gl==0 || Gl==1 || Gl==2 || Gl==3 || Gl==00 || Gl==01 || Gl==02 || Gl==03)){
			if(Gl==0 || Gl==00 || Gl==1 || Gl==01){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
		    	theta2 = curve_lines_angles(Xf,Yf,Xl,Yl,Gl);
	    	}
	    	if(Gl == 02 || Gl == 03 || Gl == 02|| Gl == 03){
            	if((Xf!=Xl || Yf!=Yl) && Ef!=El && Zf==Zl){
            		printing_move=1;
	    		}else{
	    			printing_move=0;
	    		}
				theta2 = curve_lines_angles(Il,Jl,Xf,Yf,Gl);
				theta_temp = curve_lines_angles(Il,Jl,Xl,Yl,Gl);	    		
			}
			new_curve=1;
		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
		last_printing_move=printing_move;
		last_curve=new_curve;
		theta1=theta2;
		if(Gl==02 || Gl==03 || Gl==2 || Gl==3){
		    theta1=theta_temp;
		}
        Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle		
		}else if((Gl!=1 && Gl!=01 && Gl!=0 && Gl!=00 && Gl!=02 && Gl!=03 && Gl!=02 && Gl!=03) && ((Gl!=1 && Gl!=0 && Gl!=2 && Gl!=3) || (Gl!=01 && Gl!=00 && Gl!=02 && Gl!=03))){
	        printing_move=0;
        	new_curve=0;
    		fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
    		last_printing_move=printing_move;
    		last_curve=new_curve;
    		theta1=theta2;
    		if(Gl==2 || Gl==3 || Gl==02 || Gl==03){
    		    theta1=theta_temp;
    		}
            Gf=Gl;Mf=Ml;Xf=Xl;Yf=Yl;Zf=Zl;If=Il;Jf=Jl;Ef=El;Ff=Fl;Sf=Sl;Pf=Pl;Rf=Rl;Tf=Tl; //the new line becomes the old one each read circle   
		}
    	
    }
    fprintf(final,"%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf " "%lf\n",Gf,Mf,Xf,Yf,Zf,If,Jf,Ef,Ff,Sf,Pf,Rf,Tf,last_curve);
	fclose(coord);
	fclose(final);
}


double curve_lines_angles(double xf,double yf,double xl,double yl, double Gl)
{
	int aadj;
	double theta_adj;
	
	if(Gl==0 || Gl==00 || Gl==01 || Gl==1){
    	if(fabs(xl-xf==0) && fabs(yl-yf==0)){
      		return 180;
    	}
        aadj=casequartile(xl,yl,xf,yf,1);
        theta_adj=theta_func(xl,yl,xf,yf,aadj)*180/pi;
        return theta_adj;
    }else{
        aadj=casequartile(xl,yl,xf,yf,1);
        theta_adj=theta_func(xl,yl,xf,yf,aadj)*180/pi;
        if(aadj==1){
        	if(Gl==2){
             	theta_adj=270+theta_adj;	
			}else if(Gl==3){
				theta_adj=90+theta_adj;
			}
		}else if(aadj==2){
        	if(Gl==2){
        		theta_adj=theta_adj-90;
			}else if(Gl==3){
				theta_adj=theta_adj+90;
			}			
		}else if(aadj==3){
        	if(Gl==2){
        		theta_adj=theta_adj-90;
			}else if(Gl==3){
				theta_adj=theta_adj+90;
			}			
		}else if(aadj==4){
        	if(Gl==2){
        		theta_adj=theta_adj-90;
			}else if(Gl==3){
				theta_adj=theta_adj-270;
			}			
		}
        return theta_adj;
	}
}


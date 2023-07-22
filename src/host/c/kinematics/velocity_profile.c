#include "../global_vars/globals.h"

void time_momments(double L,double umax){
		
	int loop=0,dek; 
    double t0,error,D,akr,xx,xa,xu,u,t,dt,dt1,dt2,b,E,t01,t02;

	if(JERK != 0){
     	dek=-12;
     	akr=0.5*pow(10,dek);
        t1=ACCEL_ERATION/JERK;
        ca=ACCEL_ERATION;
        cu=JMFEED+(JERK*pow(t1,2));
        xx=2*t1*(cu+JMFEED);
        if(xx<L){
            if(cu>=umax){
            	t1=sqrt((umax-JMFEED)/JERK);
            	ca=JERK*t1;
            	cu=umax;
            	xu=(2*JERK*pow(t1,3))+(4*JMFEED*t1);
            	dt=(L-xu)/umax;
    			state=3;
    			t2=2.0*t1;
        		t3=t2+dt;
        		t4=t3+t1;
    	    	t5=t4+t1;
        	}else{
         		dt1=(umax-((JERK*pow(t1,2))+JMFEED))/ACCEL_ERATION;
         		xa=((JMFEED+umax)*dt1)+(((2.0*JMFEED)+(2.0*umax))*t1);
         		ca=ACCEL_ERATION;
         		cu=umax;
        	 	if(xa>=L){
         			b=(2.0*JMFEED)+(JERK*pow(t1,2))+(2.0*ACCEL_ERATION*t1); 
         			D=pow(b,2)+(4.0*ACCEL_ERATION*(L-xx));
         			dt=(sqrt(D)-b)/(2.0*ACCEL_ERATION);
         			state=2;
         			t2=t1+dt;
         			t3=t2+t1;
         			t4=t3+t1;
         			t5=t4+dt;
         			t6=t5+t1;
         			cu=JMFEED+JERK*pow(t1,2)+(ACCEL_ERATION*dt);
         			xa=((JMFEED+cu)*dt)+(((2.0*JMFEED)+(2.0*cu))*t1);
        		}else{
         			dt2=(L-xa)/umax;
         			state=4;
         			t2=t1+dt1;
         			t3=t2+t1;
           			t4=t3+dt2;
         			t5=t4+t1;
         			t6=t5+dt1;
         			t7=t6+t1;
     	    	}
        	}
        }else{
            if(cu>=umax){
             	t1=sqrt((umax-JMFEED)/JERK);
          	    cu=umax;
        	    ca=JERK*t1;
         	    xx=2*t1*(cu+JMFEED);
         	    if(xx>=L){
            	    t0=t1/2.0;
        	        do{
         	            t=t0-(((2*JERK*pow(t0,3))+(4*JMFEED*t0)-L)/((6*JERK*pow(t0,2))+(4*JMFEED))); ////NEWTON-RAPHSON
                        error=fabs(t-t0);		
	                    t0=t;
                	    loop++;
                	    if(loop>50){ //increase calculation error if the root cannot be found
                	    	loop=0;
                	    	dek++;
                	    	akr=0.5*pow(10,dek);
	            		}
                    }while(error>=akr);
        	    	loop=0;
        			state=1;
        			t1=t;
        			t2=2.0*t1;
        			t3=3.0*t1;
        			t4=4.0*t1;
        			ca=JERK*t1;
	    	 		cu=JMFEED+(JERK*pow(t1,2));   	
	    		}else{
    			   	dt=(L-xx)/umax;
	    		   	state=3;
    			   	t2=2.0*t1;
        		 	t3=t2+dt;
        	 		t4=t3+t1;
	         		t5=t4+t1;
			   }
    	   }else{
         	    t0=t1*umax/cu;
         	    t0=50;
    	        do{
    	            t=t0-(((2.0*JERK*pow(t0,3))+(4.0*JMFEED*t0)-L)/((4.0*JERK*pow(t0,2))+(4.0*JMFEED))); ////NEWTON-RAPHSON
                    error=fabs(t-t0);		
	                t0=t;
            	    loop++;
            	    if(loop>50){ //increase calculation error if the root cannot be found
            	    	loop=0;
            	    	dek++;
            	    	akr=0.5*pow(10,dek);
	        		}
                }while(error>=akr);
    	    	loop=0;
    			state=1;
    			t1=t;
    			t2=2.0*t1;
    			t3=3.0*t1;
    			t4=4.0*t1;
    			ca=JERK*t1;
		 		cu=JMFEED+(JERK*pow(t1,2)); 
    	   }
    	}
    		if(state==1){ ///DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(t1*cu)+(JMFEED*t1);
                x3_t3=x2_t2+x2_t2-x1_t1;
    			x4_t4=2.0*x2_t2;
    		}else if(state==2){//DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			u2_t2=u1_t1+(ca*(t2-t1));
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(((u2_t2-u1_t1)*(t2-t1))/2.0)+(u1_t1*(t2-t1))+x1_t1;
    			x3_t3=(cu*(t3-t2))+(x2_t2-x1_t1)+(JMFEED*t1);
    			x4_t4=x3_t3+(x3_t3-x2_t2);
    			x5_t5=x4_t4+(x2_t2-x1_t1);
                x6_t6=2.0*x3_t3;
    		}else if(state==3){ ////DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=(t1*cu)+(t1*JMFEED);
    			x3_t3=(cu*(t3-t2))+x2_t2;
    			x4_t4=x3_t3+x2_t2-x1_t1;
    			x5_t5=x3_t3+x2_t2;
    		}else if(state=4){ ///DONE
    			u1_t1=(JERK*pow(t1,2)/2.0)+JMFEED;
    			u2_t2=u1_t1+(ca*(t2-t1));
    			x1_t1=(JERK*pow(t1,3)/6.0)+(JMFEED*t1);
    			x2_t2=x1_t1+(u1_t1*(t2-t1))+(ca*pow((t2-t1),2)/2.0);
    			x3_t3=(cu*t1)+(JMFEED*t1)+(x2_t2-x1_t1);
    			x4_t4=(cu*(t4-t3))+x3_t3;
    			x5_t5=x4_t4+(x3_t3-x2_t2);
    			x6_t6=x5_t5+(x2_t2-x1_t1);
    			x7_t7=x4_t4+x3_t3;
    		}
	    }else{
            t1=(umax-JMFEED)/ACCEL_ERATION;
      		t2=2*t1;
    		E=(2*JMFEED*t1)+(ACCEL_ERATION*pow(t1,2));
    		cu=umax;
    		state=1;
    		if(E<L){
    			t=(L-E)/umax;
    			t2=t1+t;
    			t3=t2+t1;
    			cu=umax;
    			state=2;
    		}else if(E>L){
    			D=pow(2.0*JMFEED,2)+(4.0*ACCEL_ERATION*L);
                t01=(-(2*JMFEED)+sqrt(D))/(2*ACCEL_ERATION);
                t02=(-(2*JMFEED)-sqrt(D))/(2*ACCEL_ERATION);
    			if(t01>0){
    				t1=t01;
				}else{
					t1=t02;
				}
    			t2=2*t1;
    			cu=L/(2*t1);
    			state=3;
    		}
    		if(state==1){
     			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2);
    			x2_t2=2*x1_t1;
    			//printf("%f %f %f\n",x1_t1,x2_t2,x3_t3);
    		}else if(state==2){
    			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2.0);
    			x2_t2=umax*(t2-t1)+x1_t1;
    			x3_t3=x2_t2+x1_t1;
     		}else if(state==3){
    			x1_t1=(JMFEED*t1)+(ACCEL_ERATION*pow(t1,2)/2);
    			x2_t2=2*x1_t1;
    			//printf("%f %f %f\n",x1_t1,x2_t2,x3_t3);
    		}	    	
		}
}
	

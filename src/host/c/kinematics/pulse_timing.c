#include "../global_vars/globals.h"

double L_time_calc(double l)
{	   	
    int i=0,loop=0,dek; 
    double t0=2,t01,t02,t,error,D,akr;
 	if(JERK != 0){
    	dek=-9;
      	akr=0.5*pow(10,dek);
    	if(state==1){/// VIRTUALLY TESTED
    		if(l<=x1_t1){
        	    t0=t1/2.0;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;	
	    	}else if(l<=x2_t2){
            	t0=(t1+t2)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t1,2)*t0/2)-(JERK*pow(t1,3)/3)+(JERK*t2*pow(t0,2)/2)+\
					(JERK*t2*pow(t1,2)/2)-(JERK*t2*t1*t0)+(u1_t1*t0)-(u1_t1*t1)+x1_t1-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t1,2)/2)+(JERK*t2*t0)-(JERK*t2*t1)+u1_t1)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;  	    
    	    }else if(l<=x3_t3){
            	t0=(t2+t3)/2.0;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t2,3)/6)+(JERK*pow(t2,2)*t0/2)-(JERK*pow(t2,3)/2)+(JERK*t2*pow(t0,2)/2)-(JERK*pow(t2,3)/2)\
	    	          -(JERK*pow(t2,2)*t0)+(JERK*pow(t2,3))+(cu*t0)-(cu*t2)+x2_t2-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t2,2)/2)+(JERK*t2*t0)\
	    	          -(JERK*pow(t2,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
    		}else if(l<x4_t4){
        	    t0=(t3+t4)/2;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t3,3)/6)-(JERK*pow(t3,2)*t0/2)+(JERK*pow(t3,3)/2)-(JERK*t4*pow(t0,2)/2)+(JERK*t4*pow(t3,2)/2)\
	    	        +(JERK*t4*t3*t0)-(JERK*t4*pow(t3,2))+(u1_t1*t0)-(u1_t1*t3)+x3_t3-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t3,2)/2)-(JERK*t4*t0)\
	    	        +(JERK*t4*t3)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;					
    		}else{
    			t=t4;
    		}
    	}
    	if(state==2){ //// VIRTUALLY TESTED
            if(l<=x1_t1){
        	    t0=t1/2;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;
            }else if(l<=x2_t2){
                D=pow(u1_t1-(ca*t1),2)-(2*ca*((ca*pow(t1,2)/2)-(u1_t1*t1)+x1_t1-l));
                t01=((ca*t1)-u1_t1+sqrt(D))/ca;
                t02=((ca*t1)-u1_t1-sqrt(D))/ca;
                if(t01>=t1 && t01<=t2){
                	t=t01;
     			}else{
	    			t=t02;
	    		}
            }else if(l<=x3_t3){
        	    t0=(t2+t3)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t2,2)*t0/2)-(JERK*pow(t2,3)/3)+(JERK*t3*pow(t0,2)/2)+\
					(JERK*t3*pow(t2,2)/2)-(JERK*t3*t2*t0)+(u2_t2*t0)-(u2_t2*t2)+x2_t2-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t2,2)/2)+(JERK*t3*t0)-(JERK*t3*t2)+u2_t2)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;     	
    		}else if(l<=x4_t4){
        	    t0=(t3+t4)/2;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t3,3)/6)+(JERK*pow(t3,2)*t0/2)-(JERK*pow(t3,3)/2)+(JERK*t3*pow(t0,2)/2)-(JERK*pow(t3,3)/2)\
	    	          -(JERK*pow(t3,2)*t0)+(JERK*pow(t3,3))+(cu*t0)-(cu*t3)+x3_t3-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t3,2)/2)+(JERK*t3*t0)\
	    	          -(JERK*pow(t3,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
    	    }else if(l<=x5_t5){
    			D=pow(((ca*t4)+u2_t2),2)+(2.0*ca*((x4_t4)-(ca*pow(t4,2)/2)-(t4*u2_t2)-(l)));
    			t01=(((ca*t4)+u2_t2)+sqrt(D))/ca;
    			t02=(((ca*t4)+u2_t2)-sqrt(D))/ca;
    			if(t01>=t4 && t01<=t5){
    				t=t01;
  	    		}else{
	    			t=t02;
	    		}						    	
    		}else if(l<x6_t6){
        	    t0=(t5+t6)/2;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t5,3)/6)-(JERK*pow(t5,2)*t0/2)+(JERK*pow(t5,3)/2)-(JERK*t6*pow(t0,2)/2)+(JERK*t6*pow(t5,2)/2)\
	    	        +(JERK*t6*t5*t0)-(JERK*t6*pow(t5,2))+(u1_t1*t0)-(u1_t1*t5)+x5_t5-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t5,2)/2)-(JERK*t6*t0)\
	    	        +(JERK*t6*t5)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;				
    		}else{
    			t=t6;
    		}
    	}
 	
        if(state==3){//// TESTED
            if(l<=x1_t1){
        	    t0=t1/2.0;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found 
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;
            }else if(l<=x2_t2){
            	t0=(t1+t2)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t1,2)*t0/2)-(JERK*pow(t1,3)/3)+(JERK*t2*pow(t0,2)/2)+\
					(JERK*t2*pow(t1,2)/2)-(JERK*t2*t1*t0)+(u1_t1*t0)-(u1_t1*t1)+x1_t1-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t1,2)/2)+(JERK*t2*t0)-(JERK*t2*t1)+u1_t1)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;
        	}else if(l<=x3_t3){
                    t=(l-(x2_t2-(cu*t2)))/cu;	
        	}else if(l<=x4_t4){
            	t0=(t3+t4)/2.0;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t3,3)/6)+(JERK*pow(t3,2)*t0/2)-(JERK*pow(t3,3)/2)+(JERK*t3*pow(t0,2)/2)-(JERK*pow(t3,3)/2)\
	    	          -(JERK*pow(t3,2)*t0)+(JERK*pow(t3,3))+(cu*t0)-(cu*t3)+x3_t3-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t3,2)/2)+(JERK*t3*t0)\
	    	          -(JERK*pow(t3,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
        	}else if(l<x5_t5){
            	t0=(t4+t5)/2.0;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t4,3)/6)-(JERK*pow(t4,2)*t0/2)+(JERK*pow(t4,3)/2)-(JERK*t5*pow(t0,2)/2)+(JERK*t5*pow(t4,2)/2)\
	    	        +(JERK*t5*t4*t0)-(JERK*t5*pow(t4,2))+(u1_t1*t0)-(u1_t1*t4)+x4_t4-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t4,2)/2)-(JERK*t5*t0)\
	    	        +(JERK*t5*t4)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;		
         	}else{
         		t=t5;
    	    }	
        }
  
        if(state==4){/// TESTED
            if(l<=x1_t1){
        	    t0=t1/2;
    	        do{
     		        t=t0-(((JERK*pow(t0,3)/6)+(JMFEED*t0)-l)/((JERK*pow(t0,2)/2)+JMFEED));  ////NEWTON-RAPHSON
	    	        error=fabs(t-t0); 
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
	            loop=0;
            }else if(l<=x2_t2){
                D=pow(u1_t1-(ca*t1),2)-(2*ca*((ca*pow(t1,2)/2)-(u1_t1*t1)+x1_t1-l));
                t01=((ca*t1)-u1_t1+sqrt(D))/ca;
                t02=((ca*t1)-u1_t1-sqrt(D))/ca;
                if(t01>=t1 && t01<=t2){
                	t=t01;
     			}else{
	    			t=t02;
	    		}
            }else if(l<=x3_t3){
        	    t0=(t2+t3)/2.0;
	            do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t2,2)*t0/2)-(JERK*pow(t2,3)/3)+(JERK*t3*pow(t0,2)/2)+\
					(JERK*t3*pow(t2,2)/2)-(JERK*t3*t2*t0)+(u2_t2*t0)-(u2_t2*t2)+x2_t2-l)/\
					((-JERK*pow(t0,2)/3)+(JERK*pow(t2,2)/2)+(JERK*t3*t0)-(JERK*t3*t2)+u2_t2)); ////NEWTON-RAPHSON
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	  
	            }while(error>=akr);
				loop=0;     	
    		}else if(l<=x4_t4){
    			t=(l-(x3_t3-(cu*t3)))/cu;
    		}else if(l<=x5_t5){
        	    t0=(t4+t5)/2;
    	        do{
	    	        t=t0-(((-JERK*pow(t0,3)/6)+(JERK*pow(t4,3)/6)+(JERK*pow(t4,2)*t0/2)-(JERK*pow(t4,3)/2)+(JERK*t4*pow(t0,2)/2)-(JERK*pow(t4,3)/2)\
	    	          -(JERK*pow(t4,2)*t0)+(JERK*pow(t4,3))+(cu*t0)-(cu*t4)+x4_t4-l)/((-JERK*pow(t0,2)/2)+(JERK*pow(t4,2)/2)+(JERK*t4*t0)\
	    	          -(JERK*pow(t4,2))+cu));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}	    	        
    	        }while(error>=akr);
				loop=0;
   	    	}else if(l<=x6_t6){
    			D=pow(((ca*t5)+u2_t2),2)+(2.0*ca*((x5_t5)-(ca*pow(t5,2)/2)-(t5*u2_t2)-(l)));
    			t01=(((ca*t5)+u2_t2)+sqrt(D))/ca;
    			t02=(((ca*t5)+u2_t2)-sqrt(D))/ca;
    			if(t01>=t5 && t01<=t6){
    				t=t01;
  	    		}else{
	    			t=t02;
	    		}
	    	}else if(l<x7_t7){
        	    t0=(t6+t7)/2;
	            do{
	    	        t=t0-(((JERK*pow(t0,3)/6)-(JERK*pow(t6,3)/6)-(JERK*pow(t6,2)*t0/2)+(JERK*pow(t6,3)/2)-(JERK*t7*pow(t0,2)/2)+(JERK*t7*pow(t6,2)/2)\
	    	        +(JERK*t7*t6*t0)-(JERK*t7*pow(t6,2))+(u1_t1*t0)-(u1_t1*t6)+x6_t6-l)/((JERK*pow(t0,2)/2)-(JERK*pow(t6,2)/2)-(JERK*t7*t0)\
	    	        +(JERK*t7*t6)+u1_t1));
	    	        error=fabs(t-t0);
	    	        i=i+1;		
	    	        t0=t;
        		    loop++;
        		    if(loop>30){ //increase calculation error if the root cannot be found
        		    	loop=0;
        		    	dek++;
        		    	akr=0.5*pow(10,dek);
	    			}
         	    }while(error>=akr);
	    		loop=0;		
    		}else{
    			t=t7;
    		}
    	}
    	if(state==10){
    		t=l/cu;
		}
    }else{
    	if(state==1){///TESTED
    		if(l<=x1_t1){
    		    D=pow(JMFEED,2)+(2*ACCEL_ERATION*l);
    		    t01=((-JMFEED)+sqrt(D))/ACCEL_ERATION;
    		    t02=((-JMFEED)-sqrt(D))/ACCEL_ERATION;
    		    if(t01>0 && t01<=t1){
    		    	t=t01;
				}else{
					t=t02;
				}
    		}else if(l<=x2_t2){
    			D=pow(((ACCEL_ERATION*t2)+JMFEED),2)+(2*ACCEL_ERATION*((ACCEL_ERATION*pow(t1,2)/2)-(ACCEL_ERATION*t2*t1)-(JMFEED*t1)+x1_t1-l));
    			t01=(-((ACCEL_ERATION*t2)+JMFEED)+sqrt(D))/(-ACCEL_ERATION);
    			t02=(-((ACCEL_ERATION*t2)+JMFEED)-sqrt(D))/(-ACCEL_ERATION);
    			if(t01>=t1 && t01<=t2){
                	t=t01;
    			}else{
	    			t=t02;
	    		}
			}else{
				t=t2;
			}	    
    	}
    	if(state==2){ ////TESTED
            if(l<=x1_t1){
    		    D=pow(JMFEED,2)+(2*ACCEL_ERATION*l);
    		    t01=((-JMFEED)+sqrt(D))/ACCEL_ERATION;
    		    t02=((-JMFEED)-sqrt(D))/ACCEL_ERATION;
    		    if(t01>0 && t01<=t1){
    		    	t=t01;
				}else{
					t=t02;
				}
			}else if(l<=x2_t2){
			    t=((cu*t1)+(l-x1_t1))/cu;
			}else if(l<=x3_t3){
    			D=pow(((ACCEL_ERATION*t3)+JMFEED),2)+(2*ACCEL_ERATION*((ACCEL_ERATION*pow(t2,2)/2)-(ACCEL_ERATION*t3*t2)-(JMFEED*t2)+x2_t2-l));
    			t01=(-((ACCEL_ERATION*t3)+JMFEED)+sqrt(D))/(-ACCEL_ERATION);
    			t02=(-((ACCEL_ERATION*t3)+JMFEED)-sqrt(D))/(-ACCEL_ERATION);
    			if(t01>=t2 && t01<=t3){
                	t=t01;
    			}else{
	    			t=t02;
	    		}
			}else{
				t=t3;
			}
    	}
        if(state==3){//// TESTED
    		if(l<=x1_t1){
    		    D=pow(JMFEED,2)+(2*ACCEL_ERATION*l);
    		    t01=((-JMFEED)+sqrt(D))/ACCEL_ERATION;
    		    t02=((-JMFEED)-sqrt(D))/ACCEL_ERATION;
    		    if(t01>0 && t01<=t1){
    		    	t=t01;
				}else{
					t=t02;
				}
    		}else if(l<=x2_t2){
    			D=pow(((ACCEL_ERATION*t2)+JMFEED),2)+(2*ACCEL_ERATION*((ACCEL_ERATION*pow(t1,2)/2)-(ACCEL_ERATION*t2*t1)-(JMFEED*t1)+x1_t1-l));
    			t01=(-((ACCEL_ERATION*t2)+JMFEED)+sqrt(D))/(-ACCEL_ERATION);
    			t02=(-((ACCEL_ERATION*t2)+JMFEED)-sqrt(D))/(-ACCEL_ERATION);
    			if(t01>=t1 && t01<=t2){
                	t=t01;
    			}else{
	    			t=t02;
	    		}
			}else{
				t=t2;
			}
        }
	}
    if(state==10){
    	t=l/cu;
	}
    	return t;
    }


#include "../global_vars/globals.h"

double max_n(double num1, double num2) 
   {	
        double result;
        if (num1 > num2)
            result = num1;
        else
            result = num2;
        return result; 
    }
   
double min_n(double num1, double num2) 
   {

        double result;
        if (num1 < num2)
           result = num1;
        else
           result = num2;
        return result;
   }

#include "../global_vars/globals.h"

double check_units(double Fl)
{
	if (UNITS==1){
        Fl=Fl/60;
	}
	return Fl;
}


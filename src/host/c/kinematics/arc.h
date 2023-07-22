#ifndef ARC_H
#define ARC_H

#include <stdio.h>
#include <stdint.h>
#include <process.h> 
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <malloc.h> 
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <shlobj.h> //to get %APPDATA% path

#include "../global_vars/globals.h"

void ARC(bool clockwise,double k,double l,double x1_f,double y1_f,double Ef,double x2_f,double y2_f,double El,double FEEDRATE); //GO2,G03 calculations 

#endif // LINE_H

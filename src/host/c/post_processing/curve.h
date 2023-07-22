#ifndef CURVE_H
#define  CURVE_H

//finds tangent moves, prevent decelaration
void curve_detection(unsigned long total_lines);
double curve_lines_angles(double xf,double yf,double xl,double yl, double Gl); //check angle for speed adjustment

#endif 


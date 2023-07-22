#ifndef LIMITS_H
#define LIMITS_H

double line_accel_feed_limits(double dx,double dy,double dz,double E_DIST,double FEEDRATE); //check derivative limits of G00-G01 (line)
double arc_accel_feed_limits(double R,double ARC_DIST,double E_DIST,double FEEDRATE); //check derivativelimits of G02-G03 (arc)
double check_jfeed_limits(double dx,double dy,double dz,double E_DIST); //check jump feed rate limits (only for lines)

#endif 

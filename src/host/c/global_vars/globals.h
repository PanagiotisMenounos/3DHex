#ifndef GLOBALS_H
#define GLOBALS_H

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

#include "../kinematics/line.h"
#include "../kinematics/arc.h"
#include "../kinematics/arc.h"
#include "../kinematics/velocity_profile.h"
#include "../kinematics/pulse_timing.h"
#include "../kinematics/limits.h"
#include "../post_processing/curve.h"
#include "../post_processing/gcode_parser.h"
#include "../utils/angle_calc.h"
#include "../utils/min_max.h"
#include "../utils/quartile_calc.h"
#include "../file_manager/path_maker.h"

#define pi 3.14159265358979323846
#define max_bufferfile_size 3000
#define pipename "\\\\.\\pipe\\Foo"

typedef unsigned char byte; // Define the 'byte' data type

extern union {
    int progress;
    byte temp_array[4];
} u;

extern union {
    int M;
    byte fly_array[4];
} y;

extern union {
    float S;
    byte fly_array[4];
} t;

/////////////////////////////// GLOBAL VARIABLES FOR SETTINGS ///////////////////////////////

extern double STPU_X, STPU_Y, STPU_Z, STPU_E, MAX_FX, MAX_FY, MAX_FZ, MAX_FE, MAX_ACCX, MAX_ACCY, MAX_ACCZ, MAX_ACCE, ACCELERATION, T_ACCEL_ERATION, \
       JERK, T_JERK, MAX_JFX, MAX_JFY, MAX_JFZ, MAX_JFE, JMFEED, T_JMFEED, PARK_X, PARK_Y, PARK_Z, PARK_FEED, MAX_FILE_SIZE, T0_X_OFFSET, T0_Y_OFFSET, T0_Z_OFFSET;

extern int Invrt_X, Invrt_Y, Invrt_Z, Invrt_E, COM_PORT, UNITS, ABL_ITERATIONS;
extern int32_t BAUD_RATE;
extern uint16_t CORE_FREQ;
extern uint8_t X_ENABLE, Y_ENABLE, Z_ENABLE, E_ENABLE;

extern double P_NOZZ, I_NOZZ, D_NOZZ, P_BED, I_BED, D_BED;
extern uint16_t NOZZLE_TEMP, BED_TEMP, CYCLE_NOZZ, CYCLE_BED, P_NOZZ_MIRROR, I_NOZZ_MIRROR, D_NOZZ_MIRROR, P_BED_MIRROR, I_BED_MIRROR, D_BED_MIRROR;
extern uint8_t Wait_nozz, Wait_bed, THERMISTOR_TYPE_NOZZLE, THERMISTOR_TYPE_BED, HEATED_NOZZLE, HEATED_BED, PID_nozz, PID_bed, DIFFERENTIAL_NOZZ, DIFFERENTIAL_BED;

extern int HOME_FEED_X, HOME_FEED_Y, HOME_FEED_Z;
extern uint16_t HOME_X_DURATION, HOME_Y_DURATION, HOME_Z_DURATION;
extern uint8_t HOME_X_ENABLE, HOME_Y_ENABLE, HOME_Z_ENABLE, HOME_X_STATE, HOME_Y_STATE, HOME_Z_STATE, HOME_X_DIR, HOME_Y_DIR, HOME_Z_DIR;

extern double XY_PLANE, ZX_PLANE, ZY_PLANE;
extern int CURVE_DETECTION;
extern double ANGLE_THRESHOLD;

extern uint8_t GP, GC, LAST_TIME_BOXES;
extern bool embedded_line;

extern double JM_PRC, FD_PRC, AC_PRC, JR_PRC;

extern double ABL_X, ABL_Y, BED_WIDTH, BED_LENGTH, ABL_Z_Fade;
extern double BED_XSIZE, BED_YSIZE, ABL_Z_last, GRID_RESOLUTION;
extern bool ABL_Process, ABL_Data, HOME_ALL;
extern int ABL_COORD_SIZE, ABL_resolution, ABL_Include;

extern int16_t X_ENABLE_PIN, X_STEP_PIN, X_DIR_PIN, X_ENDSTOP_PIN, Y_ENABLE_PIN, Y_STEP_PIN, Y_DIR_PIN, Y_ENDSTOP_PIN, \
    	Z_ENABLE_PIN, Z_STEP_PIN, Z_DIR_PIN, Z_ENDSTOP_PIN, Z1_ENABLE_PIN, Z1_STEP_PIN, Z1_DIR_PIN, Z1_ENDSTOP_PIN, \
		E_ENABLE_PIN, E_STEP_PIN, E_DIR_PIN, E_ENDSTOP_PIN, N_HEATER_PIN, N_SENSOR_PIN, N_FAN_PIN, B_HEATER_PIN, B_SENSOR_PIN, \
    	FAN_PIN, SERVO1_PIN, SERVO2_PIN, EXP1_1_PIN, EXP1_3_PIN, EXP1_5_PIN, EXP1_7_PIN, EXP1_9_PIN, EXP1_2_PIN, EXP1_4_PIN, \
		EXP1_6_PIN, EXP1_8_PIN, EXP1_10_PIN, EXP2_1_PIN, EXP2_3_PIN, EXP2_5_PIN, EXP2_7_PIN, EXP2_9_PIN, EXP2_2_PIN, \
		EXP2_4_PIN, EXP2_6_PIN, EXP2_8_PIN, EXP2_10_PIN;

extern int total_digi_lines, state, file_buffer_size, PRINT_STATE, file_num;
extern double storage_step, storage_counter, ACCEL_ERATION, theta_adj_last, flag_num, axis_num;
extern bool first, flag_file_state, first_time_executed, f_adj, clockwise, ABSOLUTE_POSITIONING, E_ABSOLUTE_POSITIONING;

extern double tmin, u1_t1, u2_t2, x1_t1, x2_t2, x3_t3, x4_t4, x5_t5, x6_t6, x7_t7, t1, t2, t3, t4, t5, t6, t7, cu, ca, time, last_time;
extern double LOC_CASE, new_CURVE, gen_DISTANCE, gen_FEED, trajectory_POINT;
extern double X_GLOB, Y_GLOB, X_GLOB_STEP, Y_GLOB_STEP, Z_GLOB;
extern wchar_t savepath[19][100];

extern wchar_t coordinates_path[150], fcoordinates_path[150], gc2info_path[150], savepath_path[150], gen_path[150], gcode_path[150], boxes_path[150], \
    cboxes_path[150], dboxes_path[150], buffer1_path[150], buffer2_path[150], flag_path[150], startpy_path[150], child_path[150], \
    fly_path[150], abl_x_path[150], abl_y_path[150], abl_xyz_path[150], pins_path[150];

extern FILE *SD_binary_file, *buffer1_file, *buffer2_file, *flag_file, *startpy_file, *child_file, *fly_file, *abl_x_file, *abl_y_file;

extern float *ABL_XCOORD;
extern float *ABL_YCOORD;
extern float *ABL_ZCOORD;

struct data {
   volatile int8_t test1;
   volatile int8_t test2;
   volatile int16_t X_ENABLE_PIN;
   volatile int16_t X_STEP_PIN;
   volatile int16_t X_DIR_PIN;
   volatile int16_t X_ENDSTOP_PIN;
   volatile int16_t Y_ENABLE_PIN;
   volatile int16_t Y_STEP_PIN;
   volatile int16_t Y_DIR_PIN;
   volatile int16_t Y_ENDSTOP_PIN;
   volatile int16_t Z_ENABLE_PIN;
   volatile int16_t Z_STEP_PIN;
   volatile int16_t Z_DIR_PIN;
   volatile int16_t Z_ENDSTOP_PIN;
   volatile int16_t Z1_ENABLE_PIN;
   volatile int16_t Z1_STEP_PIN;
   volatile int16_t Z1_DIR_PIN;
   volatile int16_t Z1_ENDSTOP_PIN;
   volatile int16_t E_ENABLE_PIN;
   volatile int16_t E_STEP_PIN;
   volatile int16_t E_DIR_PIN;
   volatile int16_t E_ENDSTOP_PIN;
   volatile int16_t N_HEATER_PIN;
   volatile int16_t N_SENSOR_PIN;
   volatile int16_t N_FAN_PIN;
   volatile int16_t B_HEATER_PIN;
   volatile int16_t B_SENSOR_PIN;
   volatile int16_t FAN_PIN;
   //volatile int16_t SERVO1_PIN;
   //volatile int16_t SERVO2_PIN;
   volatile uint8_t A;
   volatile uint8_t B;
   volatile uint8_t C;
   volatile uint8_t D;
   volatile uint8_t E;
   volatile uint8_t F;
   volatile uint8_t G;
   volatile uint8_t H;
   volatile uint16_t I;
   volatile uint16_t J;
   volatile uint16_t K;
   volatile uint16_t L;
   volatile uint8_t M;
   volatile uint8_t N;
   volatile uint16_t O;
   volatile uint16_t P;
   volatile uint16_t Q;
   volatile uint16_t R;
   volatile uint16_t S;
   volatile uint16_t T;
   volatile int16_t U;
   volatile int16_t V;
   volatile int16_t W;
} MG_data;

#endif // GLOBALS_H


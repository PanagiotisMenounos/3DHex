#include "../global_vars/globals.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Define the global variables and initialize them here
typedef unsigned char byte;

// Definitions of the union variables
union {
    int progress;
    byte temp_array[4];
} u;

union {
    int M;
    byte fly_array[4];
} y;

union {
    float S;
    byte fly_array[4];
} t;


double STPU_X = 0, STPU_Y = 0, STPU_Z = 0, STPU_E = 0, MAX_FX = 0, MAX_FY = 0, MAX_FZ = 0, MAX_FE = 0, MAX_ACCX = 0, MAX_ACCY = 0, MAX_ACCZ = 0, MAX_ACCE = 0, ACCELERATION = 0, T_ACCEL_ERATION = 0, \
       JERK = 0, T_JERK = 0, MAX_JFX = 0, MAX_JFY = 0, MAX_JFZ = 0, MAX_JFE = 0, JMFEED = 0, T_JMFEED = 0, PARK_X = 0, PARK_Y = 0, PARK_Z = 100, PARK_FEED = 30, MAX_FILE_SIZE = 0, T0_X_OFFSET = 0, T0_Y_OFFSET = 0, T0_Z_OFFSET = 0;

int Invrt_X = 0, Invrt_Y = 0, Invrt_Z = 0, Invrt_E = 0, COM_PORT = 0, UNITS = 0, ABL_ITERATIONS = 0;
int32_t BAUD_RATE = 0;
uint16_t CORE_FREQ = 0;
uint8_t X_ENABLE = 0, Y_ENABLE = 0, Z_ENABLE = 0, E_ENABLE = 0;

double P_NOZZ = 0, I_NOZZ = 0, D_NOZZ = 0, P_BED = 0, I_BED = 0, D_BED = 0;
uint16_t NOZZLE_TEMP = 0, BED_TEMP = 0, CYCLE_NOZZ = 0, CYCLE_BED = 0, P_NOZZ_MIRROR = 0, I_NOZZ_MIRROR = 0, D_NOZZ_MIRROR = 0, P_BED_MIRROR = 0, I_BED_MIRROR = 0, D_BED_MIRROR = 0;
uint8_t Wait_nozz = 0, Wait_bed = 0, THERMISTOR_TYPE_NOZZLE = 0, THERMISTOR_TYPE_BED = 0, HEATED_NOZZLE = 0, HEATED_BED = 0, PID_nozz = 0, PID_bed = 0, DIFFERENTIAL_NOZZ = 0, DIFFERENTIAL_BED = 0;

int HOME_FEED_X = 0, HOME_FEED_Y = 0, HOME_FEED_Z = 0;
uint16_t HOME_X_DURATION = 0, HOME_Y_DURATION = 0, HOME_Z_DURATION = 0;
uint8_t HOME_X_ENABLE = 0, HOME_Y_ENABLE = 0, HOME_Z_ENABLE = 0, HOME_X_STATE = 0, HOME_Y_STATE = 0, HOME_Z_STATE = 0, HOME_X_DIR = 0, HOME_Y_DIR = 0, HOME_Z_DIR = 0;

double XY_PLANE = 0, ZX_PLANE = 0, ZY_PLANE = 0;
int CURVE_DETECTION = 0;
double ANGLE_THRESHOLD = 0;

uint8_t GP = 0, GC = 0, LAST_TIME_BOXES = 0;
bool embedded_line = false;

double JM_PRC = 1, FD_PRC = 1, AC_PRC = 1, JR_PRC = 1;

double ABL_X = 0, ABL_Y = 0, BED_WIDTH = 200, BED_LENGTH = 200, ABL_Z_Fade = 0;
double BED_XSIZE = 0, BED_YSIZE = 0, ABL_Z_last = 0, GRID_RESOLUTION = 1;
bool ABL_Process = false, ABL_Data = false, HOME_ALL = false;
int ABL_COORD_SIZE = 0, ABL_resolution = 1, ABL_Include = 0;

int16_t X_ENABLE_PIN = 0, X_STEP_PIN = 0, X_DIR_PIN = 0, X_ENDSTOP_PIN = 0, Y_ENABLE_PIN = 0, Y_STEP_PIN = 0, Y_DIR_PIN = 0, Y_ENDSTOP_PIN = 0, \
    	Z_ENABLE_PIN = 0, Z_STEP_PIN = 0, Z_DIR_PIN = 0, Z_ENDSTOP_PIN = 0, Z1_ENABLE_PIN = 0, Z1_STEP_PIN = 0, Z1_DIR_PIN = 0, Z1_ENDSTOP_PIN = 0, \
		E_ENABLE_PIN = 0, E_STEP_PIN = 0, E_DIR_PIN = 0, E_ENDSTOP_PIN = 0, N_HEATER_PIN = 0, N_SENSOR_PIN = 0, N_FAN_PIN = 0, B_HEATER_PIN = 0, B_SENSOR_PIN = 0, \
    	FAN_PIN = 0, SERVO1_PIN = 0, SERVO2_PIN = 0, EXP1_1_PIN = 0, EXP1_3_PIN = 0, EXP1_5_PIN = 0, EXP1_7_PIN = 0, EXP1_9_PIN = 0, EXP1_2_PIN = 0, EXP1_4_PIN = 0, \
		EXP1_6_PIN = 0, EXP1_8_PIN = 0, EXP1_10_PIN = 0, EXP2_1_PIN = 0, EXP2_3_PIN = 0, EXP2_5_PIN = 0, EXP2_7_PIN = 0, EXP2_9_PIN = 0, EXP2_2_PIN = 0, \
		EXP2_4_PIN = 0, EXP2_6_PIN = 0, EXP2_8_PIN = 0, EXP2_10_PIN = 0;

int total_digi_lines = 0;
int state = 0;
int file_buffer_size = 0;
int PRINT_STATE = 0;
int file_num = 0;

double storage_step = 0.00000000000001;
double storage_counter = 0;
double ACCEL_ERATION = 0;
double theta_adj_last = 0;
double flag_num = 101101101.101010;
double axis_num = 10201.102012;

bool first = false;
bool flag_file_state = false;
bool first_time_executed = true;
bool f_adj = true;
bool clockwise;
bool ABSOLUTE_POSITIONING = 1;
bool E_ABSOLUTE_POSITIONING = 1;

double tmin = 0, u1_t1 = 0, u2_t2 = 0, x1_t1 = 0, x2_t2 = 0, x3_t3 = 0, x4_t4 = 0, x5_t5 = 0, x6_t6 = 0, x7_t7 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0, t6 = 0, t7 = 0, cu = 0, ca = 0, time = 0, last_time = 0;
double LOC_CASE = 1, new_CURVE = 0, gen_DISTANCE = 0, gen_FEED = 0, trajectory_POINT = 0;
double X_GLOB = 0, Y_GLOB = 0, X_GLOB_STEP = 0, Y_GLOB_STEP = 0, Z_GLOB = 0;

wchar_t savepath[19][100] = {
    L"//3DHex2//support files//coordinates.txt\0",
    L"//3DHex2//support files//fcoordinates.txt\0",
    L"//3DHex2//support files//gc2info.txt\0",
    L"//3DHex2//support files//savepath.txt\0",
    L"//3DHex2//support files//gen.txt\0",
    L"//3DHex2//support files//GCODE.txt\0",
    L"//3DHex2//settings//boxes settings.txt\0",
    L"//3DHex2//settings//cboxes settings.txt\0",
    L"//3DHex2//settings//dboxes settings.txt\0",
    L"//3DHex2//binary files//buffer_1.bin\0",
    L"//3DHex2//binary files//buffer_2.bin\0",
    L"//3DHex2//binary files//flag.bin\0",
    L"//3DHex2//binary files//flag_py.bin\0",
    L"//3DHex2//binary files//child.bin\0",
    L"//3DHex2//binary files//fly.bin\0",
    L"//3DHex2//settings//abl_x.txt\0",
    L"//3DHex2//settings//abl_y.txt\0",
    L"//3DHex2//settings//XYZ.txt\0",
    L"//3DHex2//settings//pins settings.txt\0"
};

wchar_t coordinates_path[150] = {0};
wchar_t fcoordinates_path[150] = {0};
wchar_t gc2info_path[150] = {0};
wchar_t savepath_path[150] = {0};
wchar_t gen_path[150] = {0};
wchar_t gcode_path[150] = {0};
wchar_t boxes_path[150] = {0};
wchar_t cboxes_path[150] = {0};
wchar_t dboxes_path[150] = {0};
wchar_t buffer1_path[150] = {0};
wchar_t buffer2_path[150] = {0};
wchar_t flag_path[150] = {0};
wchar_t startpy_path[150] = {0};
wchar_t child_path[150] = {0};
wchar_t fly_path[150] = {0};
wchar_t abl_x_path[150] = {0};
wchar_t abl_y_path[150] = {0};
wchar_t abl_xyz_path[150] = {0};
wchar_t pins_path[150] = {0};

FILE *SD_binary_file = NULL;
FILE *buffer1_file = NULL;
FILE *buffer2_file = NULL;
FILE *flag_file = NULL;
FILE *startpy_file = NULL;
FILE *child_file = NULL;
FILE *fly_file = NULL;
FILE *abl_x_file = NULL;
FILE *abl_y_file = NULL;

float *ABL_XCOORD = NULL;
float *ABL_YCOORD = NULL;
float *ABL_ZCOORD = NULL;

struct data MG_data;




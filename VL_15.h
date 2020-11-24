#ifdef _WIN32
#include <tchar.h>
#endif


#ifndef MAIN_VL15_H
#define MAIN_VL15_H


#include "src/private_vl_15.h"
#include "src/ts.h"


#define CURRENT_Q 430.0
#define CURRENT_Q_BF 0.0025
#define TIME_THROTTLE_SWITCH1 1.0
#define TIME_THROTTLE_SWITCH2 2.0

#define BRAKE_STR_RATE 1.8
#define BRAKE_MR_RATIO    0.005
#define BRAKE_PIPE_RATE_CHARGE 1.8
#define BRAKE_UR_RATE_CHARGE   0.25
#define BRAKE_PIPE_RATE 0.5
#define BRAKE_PIPE_APPL_RATE   0.25
#define BRAKE_PIPE_EMERGENCY -1.2
#define PIPE_DISCHARGE_SLOW  -0.005

#define FORCE_SHIFT 30000.0
#define BRAKE_FORCE_SHIFT 10000.0
#define CURRENT_SHUNT_Q 1.2

typedef struct st_Pneumo
{
  int Arm_254;
  int Arm_394;
  int Blok_367;

}Pneumo;

typedef struct st_Electric
{
int Panto;
float power;
}Electric;

typedef struct st_Tumblers
{
    int bv1;
    int bv2_on;
    int bv2_off;
    int panto;
    int panto1_3;
    int panto2_4;
}st_Tumblers;

extern "C" void Q_DECL_EXPORT Switched(const ElectricLocomotive *loco,ElectricEngine *eng,
        unsigned int SwitchID,unsigned int PrevState);

#endif

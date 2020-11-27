#ifndef VL15_LOGIC_H
#define VL15_LOGIC_H

#include <sys/timeb.h>
#include "radiostation.h"
#include "saut_datatype.h"
#include "src/private_vl_15.h"
#include "src/vl15_datatypes/cab/section1/elements.h"

#define NEUTRAL 2
#define SECTION_DEST_FORWARD 1
#define SECTION_DEST_BACKWARD -1
#define SIGNALS_CNT 4


PACKED_BEGIN

/*разные флаги локомотива*/
typedef struct st_Flags
{
    int EPK_Triggered;
}st_Flags;


typedef struct st_Pneumo
{
  int Arm_254;
  int Arm_394;
  int Blok_367;

}Pneumo;

typedef struct st_Electric
{
    unsigned char PantoRaised;
    float power;
    float LineVoltage;
}Electric;

typedef struct st_Tumblers
{
    int bv1;
    int bv2_on;
    int bv2_off;
    int panto;
    int panto1_3;
    int panto2_4;
    int projHalf;
    int projFull;
}st_Tumblers;

typedef struct st_timeForDebug
{
    struct timeb prevTime;
    struct timeb currTime;
}st_timeForDebug;

/**
 * @brief The st_Self struct Структура с собственными параметрами для работы между функциями библиотеки.
 */
struct st_Self
{
  st_Flags flags;
  int brake394_pos;
  int secdionCabDest;
  int dest; // 1 ,0,  -1
  unsigned int SignalColor[4];
  st_Tumblers tumblers;
  Electric elecrto;
  Pneumo pneumo;
  float Velocity; // тек. скорость
  float prevVelocity;
  bool SL2M_Ticked;
  int BV_STATE;
  int EPK;
  int RB;
  int MK;
  int MV_low;
  st_Radiostation radio;
  int Reverse;
  unsigned int ThrottlePosition;
  int shuntNum;
  st_SAUT sautData;
  st_timeForDebug debugTime;
};

PACKED_END

bool VL15_init(const ElectricLocomotive *loco, struct st_Self *self);
void VL15_set_destination(st_Self *self, int dest);
void VL15_ALSN(const Locomotive *loco, UINT NumSigAhead, SignalsInfo *sigAhead, UINT NumSigBack,
               SignalsInfo *sigBack, struct st_Self *self);
int VL15_Step(const ElectricLocomotive *loco, ElectricEngine *eng, struct st_Self *self);

#endif // VL15_LOGIC_H

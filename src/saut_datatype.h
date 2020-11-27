#ifndef SAUT_DATATYPE_H
#define SAUT_DATATYPE_H

#include "shared_code.h"
#include "ts.h"
#include "sys/timeb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum en_SignColors
{
  UNSET = -1,
  COLOR_WHITE = 0,
  COLOR_RED = 1,
  COLOR_RD_YEL = 2,
  COLOR_YELLOW = 3,
  COLOR_GREEN = 4,

}en_SignColors;

PACKED_BEGIN
typedef struct st_SAUT
{
 float CurrSpeed;
 float PrevSpeed;
 en_SignColors prevSignalColor;
 en_SignColors forwardSignalColor;
 struct SpeedLimitDescr SpeedLimit;
 int AspectSignal;
 wchar_t signalName[MAX_STRING_NAME];
}st_SAUT;
PACKED_END

#ifdef __cplusplus
}
#endif

#endif // SAUT_DATATYPE_H

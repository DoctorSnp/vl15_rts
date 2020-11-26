#ifndef SAUT_DATATYPE_H
#define SAUT_DATATYPE_H

#include "shared_code.h"
#include "ts.h"
#include "sys/timeb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum en_Colors
{
  UNSET = -1,
  COLOR_WHITE = 0,
  COLOR_RED = 1,
  COLOR_RD_YEL = 2,
  COLOR_YELLOW = 3,
  COLOR_GREEN = 4,

}en_Colors;

PACKED_BEGIN
typedef struct st_SAUT
{
 float CurrSpeed;
 float PrevSpeed;
 struct SpeedLimitDescr SpeedLimit;
 en_Colors forwColor;
 wchar_t signalName[MAX_STRING_NAME];
}st_SAUT;
PACKED_END

#ifdef __cplusplus
}
#endif

#endif // SAUT_DATATYPE_H

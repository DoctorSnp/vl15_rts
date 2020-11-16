#ifdef _WIN32
#include <tchar.h>
#endif

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

/*
 Stack Variables

 0 - unsigned long Flags
  1bite
   1bit - low voltage
   2bit - emergency brake incurred
   3bit - protection incurred
   4bit - compressor on
   5bit - alert started
   6bit -
   7bit - enable train pipe pressure maintenance
   8bit - init radiostation

  2bite
   1bit - convertor on
   2bit - MV on
   3bit - MV on high

 1 - throttle switch timer

 2 - set throttle position
   1 - off
   2 - S
   3 - SP
   4 - P1
   5 - P2
   6 - +1
   7 - -1
   8 - temp on
   9 - temp off
   10 - SHUNTS
   11 - start EDT
   12 - EDT+
   13 - EDT-
   14 - EDT S
   15 - EDT SP
   16 - EDT 0
   17 - stop EDT

 3 - [VACANT]

 4 - FlagsAsync
  1bite
   1bit - switch gv off
   2bit - switch gv on
   3,4bit - raise 1,2 panto
   5,6bit - lower 3,4 panto
   7bit - convertor on
   8bit - temp(position)
  2bite
   1bit - reset throttle set
   2bit - compressor on
   3bit - sanding on
   4bit - enable train pipe charge
   5bit - turn vents on
   6bit - turn vents on high
   7bit - TED 1-2 off
   8bit - TED 3-4 off
  3bite
   1bit - compressor manual on
   2bit - No S
   3bit - shutdown P2

 5 - Traction current limit

 6 - throttle lamp blink timer

 7 - shunting position

 8 - dynamic brake

 9 - previous force/brake force

 10 - resistors heat amount

 11 - EDT position

 12 - EDT current limit

 13 - previous brake force

 14 - current speed limit

 15 - EPK state
  0 - free
  1 - signal change (non-critical)
  2 - approaching red
  3 - overspeed
  4 - approaching red
  5 - approaching signal limit

 16 - EPK timer

 17 - previous signal aspect

 Sound Triggers

 4,5 - sander on/off
 14 - brake applied
 23 - BV switching
 54 - brake released
 56,57 - EPK sound on,off
 101 - Compressor start
 102 - Compressor release
 103 - BV
 104 - radio on
 105 - radio off
 106 - MV on
 107 - MV off
 108 - button press
 109 - MV high started
 110 - MV high stop
 111 - EPK release

 LocoOn
  1bite
   1bit - battery on
   2bit - KU on
   3bit - converter on
   4bit - EPK on
*/

extern "C" void Q_DECL_EXPORT Switched(const ElectricLocomotive *loco,ElectricEngine *eng,
        unsigned int SwitchID,unsigned int PrevState);

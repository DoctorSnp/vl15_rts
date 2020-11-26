#ifndef SAUT_H
#define SAUT_H

#include "ts.h"
#include "saut_datatype.h"

typedef enum en_Saut
{
    SAUT_OFF = 200,
    PLATFORM = 201,
    STATION = 202,
    Drive_Backward = 203,
    Start_Drive = 204,
    NEITRAL_STATE= 205,
    GREEN = 206,
    YELLOW = 207,
    KG = 208,
    RED = 209,
    WHITE = 210,
    DISABLE_TYAGA = 211,
    TRY_BRAKE  = 212,
    STO = 213,
    PEREEZD = 214,
    PICK = 215
}
SAUT_sounds;


typedef struct st_intermnalSAUT
{
    int timeForDisableForce;
    int timeForPick;
    int timeForEPKstart;
    int timeForEPKbrake;
    struct timeb prevTime;
    struct timeb currTime;
}st_intermnalSAUT;

class SAUT
{
public:
    SAUT();
    int init();
    int start(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData);
    int stop(const ElectricLocomotive *loco, ElectricEngine *eng);
    int step(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData);
private:
    void m_playColor(const ElectricLocomotive *loco);
    void m_updateSoundsTime();
    void m_Sound_Pick(const ElectricLocomotive *loco);
    void m_Sound_DisableTyaga(const ElectricLocomotive *loco);
    void m_Sound_ResetTime();

    int isEnabled = false;
    st_SAUT m_Data;
    st_intermnalSAUT m_InternalState;
};

#endif // SAUT_H

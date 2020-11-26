#ifndef SAUT_H
#define SAUT_H

#include "ts.h"
#include "saut_datatype.h"

typedef enum en_Saut
{
    Start_Drive = 204,
    STATION = 202,
    Drive_Backward = 203,
    GREEN = 206,
    YELLOW = 207,
    KG = 208,
    RED = 209,
    WHITE = 210,
    STO = 213,
    PICK = 215
}
SAUT_sounds;


class SAUT
{
public:
    SAUT();
    int init();
    int start(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData);
    int stop();
    int step(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData);
private:
    void m_playColor(const ElectricLocomotive *loco);
    int isEnabled = false;
    st_SAUT m_Data;
};

#endif // SAUT_H

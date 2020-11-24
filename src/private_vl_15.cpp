
#include <math.h>
#include <stdio.h>

#include "private_vl_15.h"

int _checkSwitchWithSound(const ElectricLocomotive *loco, unsigned int switchElem, int soundId, int singleSound)
{
    if (soundId >= 0)
    {
        if (loco->Cab()->Switch(soundId) > 0)
            loco->PostTriggerCab((unsigned short)soundId); // устанавливаем звук
        else
        {
            if (!singleSound) // если не PlayOneShot
                loco->PostTriggerCab((unsigned short)soundId + 1); // сбрасываем звук
        }
    }
    return loco->Cab()->Switch(switchElem);
}

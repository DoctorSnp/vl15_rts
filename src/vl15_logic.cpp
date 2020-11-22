#include "vl15_logic.h"

VL15_logic::VL15_logic()
{
    for (int i=0; i< SWITCHES_CNT; i++)
        m_switchedsStates[i] = 0;
}

bool VL15_logic::init() noexcept
{
    return true;
}

int VL15_logic::checkSwitch(const ElectricLocomotive *loco, unsigned short switchElem) noexcept
{
    m_switchedsStates[switchElem] = m_checkSwitch(loco, switchElem);;
    return m_switchedsStates[switchElem];
}

unsigned short VL15_logic::m_checkSwitch(const ElectricLocomotive *loco, unsigned short switchElem) noexcept
{
    unsigned short newState = 0;
    if (loco->Cab()->Switch(switchElem) > 0)
    {
     loco->PostTriggerCab(switchElem); // включаем звук
     newState = 1;
    }
    else
     loco->PostTriggerCab(switchElem+1); // сбрасываем звук

    return newState;
}



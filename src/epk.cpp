
#include "vl15_datatypes/cab/section1/elements.h"

#include "epk.h"

EPK::EPK()
{

}

int EPK::init()
{
    ftime(&prevTime);
    currTime = prevTime;
    m_state = en_EPKState::EPK_Normal;
    return 1;
}

void EPK::setEnabled(const ElectricLocomotive *loco, int isEnabled)
{
    if (isEnabled == 0)
    {
        loco->PostTriggerCab(SoundsID::EPK_ALARM_S);
        m_state = en_EPKState::EPK_Normal;
    }
}

int EPK::step(const ElectricLocomotive *loco, int state)
{
    if ((state == EPK_ACTIVATING) || (m_state == en_EPKState::EPK_ACTIVATING ))
    {
        if (m_state != en_EPKState::EPK_ACTIVATING)
        {
            m_state = en_EPKState::EPK_ACTIVATING;
            ftime(&prevTime);
            ftime(&currTime);
        }
        ftime(&currTime);
        if (prevTime.time + 5 <= currTime.time )
        {
            loco->PostTriggerCab(SoundsID::EPK_ALARM_S);
            if (prevTime.time + 10 <= currTime.time)
                return 1;
        }
    }
    return 0;
}

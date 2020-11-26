#include <cmath>
#include "utils/utils.h"

#include "src/vl15_datatypes/cab/section1/elements.h"

#include "saut.h"

constexpr int TIME_FOR_DISABLE_TYAGA = 20;
constexpr int TIME_FOR_EPK_START = 14;
constexpr int TIME_FOR_EPK_BRAKE = TIME_FOR_EPK_START + 14;

SAUT::SAUT()
{}

int SAUT::init()
{
    return 1;
}

int SAUT::start(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData)
{
    ftime(&m_InternalState.prevTime);
    ftime(&m_InternalState.currTime);

    m_InternalState.timeForDisableForce = 5;
    m_InternalState.timeForPick = 2;
    m_InternalState.timeForEPKstart = 0;
    m_InternalState.timeForEPKbrake = 0;

    isEnabled = 1;
    eng->ALSNOn = 0x1;
    m_Data = externalData;

    m_playColor(loco);
    return 1;
}

int SAUT::stop(const ElectricLocomotive *loco, ElectricEngine *eng)
{
    isEnabled = 0;
    eng->ALSNOn = 0;
    loco->PostTriggerCab(SAUT_sounds::SAUT_OFF);
    return 1;
}

int SAUT::step(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData)
{

    if (isEnabled == 0)
        return 0;
    if ( loco->Velocity == 0.0)
        return 0;

    ftime(&m_InternalState.currTime);
    m_updateSoundsTime();

    m_Data = externalData;
    if ( m_Data.SpeedLimit.Limit <= m_Data.CurrSpeed )
    {
        m_Sound_DisableTyaga(loco);
        m_Sound_Pick(loco);

    }
    else  if ((m_Data.SpeedLimit.Limit - 2.0) <= m_Data.CurrSpeed)
    {
         // за 2 км/ч начинаем пищать
         m_Sound_Pick(loco);
    }

    static en_Colors prevColor = externalData.forwColor;
    if ( (externalData.PrevSpeed == 0.0) && (std::fabs(externalData.CurrSpeed) > 0.0) )
    {
        if (externalData.CurrSpeed)
            loco->PostTriggerCab(SAUT_sounds::Start_Drive);
        else
            loco->PostTriggerCab(SAUT_sounds::Drive_Backward);
    }

    if (prevColor != externalData.forwColor)
    {
        m_playColor(loco);
        prevColor = externalData.forwColor;
    }

    return 1;
}

void SAUT::m_playColor(const ElectricLocomotive *loco)
{
    if (m_Data.forwColor == en_Colors::COLOR_GREEN)
        loco->PostTriggerCab(SAUT_sounds::GREEN);
    else if (m_Data.forwColor == en_Colors::COLOR_YELLOW)
        loco->PostTriggerCab(SAUT_sounds::YELLOW);
    else if (m_Data.forwColor == en_Colors::COLOR_RD_YEL)
        loco->PostTriggerCab(SAUT_sounds::KG);
    else if (m_Data.forwColor == en_Colors::COLOR_RED)
        loco->PostTriggerCab(SAUT_sounds::RED);
    else if (m_Data.forwColor == en_Colors::COLOR_WHITE )
        loco->PostTriggerCab(SAUT_sounds::WHITE);
    else
    {
        loco->PostTriggerCab(SAUT_sounds::STO);
        //loco->PostTriggerCab(SAUT_sounds::PICK);
    }
}

/* обновляет время для начала звучаний */
void SAUT::m_updateSoundsTime()
{
    ftime(&m_InternalState.currTime);
    int prevMillis = m_InternalState.prevTime.time * 1000 + m_InternalState.prevTime.millitm;
    int currMillis = m_InternalState.currTime.time * 1000 + m_InternalState.currTime.millitm;

    if (prevMillis + 500 <= currMillis)
    {
        if (m_InternalState.timeForDisableForce < TIME_FOR_DISABLE_TYAGA)
            m_InternalState.timeForDisableForce++;
       m_InternalState.timeForPick = 1;

       if (m_InternalState.timeForDisableForce < TIME_FOR_EPK_START)
            m_InternalState.timeForEPKstart++;
       if (m_InternalState.timeForDisableForce < TIME_FOR_EPK_BRAKE)
            m_InternalState.timeForEPKbrake++;

       m_InternalState.prevTime = m_InternalState.currTime;
    }
}

void SAUT::m_Sound_Pick(const ElectricLocomotive *loco)
{
    if (m_InternalState.timeForPick )
    {
        loco->PostTriggerCab(SAUT_sounds::PICK);
        m_InternalState.timeForPick = 0;
    }
}

void SAUT::m_Sound_DisableTyaga(const ElectricLocomotive *loco)
{
    if (m_InternalState.timeForDisableForce >= TIME_FOR_DISABLE_TYAGA )
    {
        loco->PostTriggerCab(SAUT_sounds::DISABLE_TYAGA);
        m_InternalState.timeForDisableForce = 0;
    }
}

void SAUT::m_Sound_ResetTime()
{
    m_InternalState.timeForPick = 0;
    m_InternalState.timeForEPKstart = 0;
    m_InternalState.timeForDisableForce = 0;
    m_InternalState.timeForEPKbrake = 0;
}

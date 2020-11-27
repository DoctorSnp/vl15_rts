#include <cmath>
#include "utils/utils.h"

#include "src/vl15_datatypes/cab/section1/elements.h"

#include "saut.h"

constexpr int TIME_FOR_DISABLE_TYAGA = 20;
constexpr int TIME_FOR_EPK_START = 14;
constexpr int TIME_FOR_EPK_BRAKE = TIME_FOR_EPK_START + 14;

constexpr int WHITE_LIMIT = 40; // ограничение на 40 км

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
        return SAUT_DISABLED;

    static en_SignColors prevColor = externalData.forwardSignalColor;
    float prevSpeed = m_Data.PrevSpeed;
    m_Data = externalData;
    m_Data.PrevSpeed = prevSpeed;


    /*корректируем красный/белый*/
    if (externalData.forwardSignalColor == COLOR_RED)
    {    if (prevColor != en_SignColors::COLOR_RD_YEL)
            m_Data.forwardSignalColor = en_SignColors::COLOR_WHITE;
    }

    if (prevColor != externalData.forwardSignalColor)
    {
        m_playColor(loco);
        prevColor = externalData.forwardSignalColor;
        if (externalData.forwardSignalColor != en_SignColors::COLOR_GREEN)
        {
            if (loco->Velocity != 0.0)
                return EPK_ALARM;
        }
    }

    if ( (m_Data.forwardSignalColor == en_SignColors::COLOR_WHITE) && (WHITE_LIMIT > -1))
        m_Data.SpeedLimit.Limit = WHITE_LIMIT;

    if ( loco->Velocity == 0.0)
        return isEnabled;

    ftime(&m_InternalState.currTime);
    m_updateSoundsTime();

    if ( m_Data.SpeedLimit.Limit < m_Data.CurrSpeed )
    {
        m_Sound_DisableTyaga(loco);
        m_Sound_Pick(loco);
        return EPK_ALARM;
    }
    else  if ((m_Data.SpeedLimit.Limit - 2.0) <= m_Data.CurrSpeed)
    {
         // за 2 км/ч начинаем пищать
         m_Sound_Pick(loco);
    }

    if ( (m_Data.PrevSpeed == 0.0) && (std::fabs(m_Data.CurrSpeed) > 0.0) )
    {
        if (externalData.CurrSpeed)
            loco->PostTriggerCab(SAUT_sounds::Start_Drive);
        else
            loco->PostTriggerCab(SAUT_sounds::Drive_Backward);
        m_Data.PrevSpeed = m_Data.CurrSpeed;
        return EPK_ALARM;
    }

    m_Data.PrevSpeed = m_Data.CurrSpeed;
    return SAUT_ENABLED;
}

void SAUT::m_playColor(const ElectricLocomotive *loco)
{
    if (m_Data.forwardSignalColor == en_SignColors::COLOR_GREEN)
        loco->PostTriggerCab(SAUT_sounds::GREEN);
    else if (m_Data.forwardSignalColor == en_SignColors::COLOR_YELLOW)
        loco->PostTriggerCab(SAUT_sounds::YELLOW);
    else if (m_Data.forwardSignalColor == en_SignColors::COLOR_RD_YEL)
        loco->PostTriggerCab(SAUT_sounds::KG);
    else if (m_Data.forwardSignalColor == en_SignColors::COLOR_RED)
        loco->PostTriggerCab(SAUT_sounds::RED);
    else
        loco->PostTriggerCab(SAUT_sounds::WHITE);
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

#include <cmath>
#include "utils/utils.h"

#include "src/vl15_datatypes/cab/section1/elements.h"

#include "saut.h"


SAUT::SAUT()
{}

int SAUT::init()
{
    return 1;
}

int SAUT::start(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData)
{
    isEnabled = 1;
    m_Data = externalData;
    eng->ALSNOn=1;

    m_playColor(loco);

    return 1;

    Printer_print(eng, GMM_POST, L"Limit: %f  NextLimit: %f Dist: %f\n",
                  m_Data.SpeedLimit.Limit,
                  m_Data.SpeedLimit.NextLimit,
                  m_Data.SpeedLimit.Distance);
}

int SAUT::stop()
{
    isEnabled = 0;
    return 1;
}

int SAUT::step(const ElectricLocomotive *loco, ElectricEngine *eng, st_SAUT externalData)
{
    if (isEnabled == 0)
        return 0;

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

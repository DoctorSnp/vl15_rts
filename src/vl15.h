#ifndef VL15_LOGIC_H
#define VL15_LOGIC_H

#include "src/private_vl_15.h"
#include "src/vl15_datatypes/cab/section1/elements.h"

constexpr int SWITCHES_CNT = 500;
constexpr int ENABLED = 1;
constexpr int DISABLED = 0;

class VL15_logic
{
public:
    VL15_logic();
    bool init() noexcept;

    /**
     * @brief checkSwitch проверяет, переключился ли элемент.
     * @param loco
     * @param switchElem
     * @param singleSound
     */
    //int checkSwitch(const ElectricLocomotive* loco, unsigned short switchElem) noexcept;

private:
    unsigned short m_checkSwitch(const ElectricLocomotive* loco, unsigned short switchElem) noexcept;

    int m_switchedsStates[SWITCHES_CNT];
};

#endif // VL15_LOGIC_H

#ifndef ELEMENTS_H
#define ELEMENTS_H

typedef enum en_Tumblers
{
    Tbm_Panto = 13,
    Tmb_Panto1_3 = 14,
    Tmb_Panto2_4 = 15,
    Tmb_vozvrBV1 = 16,
    Tmb_BV1 = 17,
    Tmb_BV2_on = 12,
    Tmb_BV2_off = 11,
    Tmb_Searchlight_Bright = 18,
    Tmb_Searchlight_Dimly = 19,

}Tumblers;


typedef enum en_Buttons
{
    Btn_Tifon = 39,
    Btn_Svistok = 10,

}Buttons;

typedef enum en_Arms
{
  Arm_Reverse = 2,
  Arm_Zadatchik = 4,
}Arms;

typedef enum en_Keys
{
    Key_EPK = 56,
    Key_EPK_ON = 106
}Keys;


typedef enum en_Sensors
{
    Snsr_voltmeterAKB = 0,
    Snsr_ampermeterAKB = 1,
    Snsr_Yakor = 2,
    Snsr_TED_1 = 15,
    Snsr_TED_2 = 14,
    Snsr_TED_3 = 13,
    Snsr_TED_4 = 12,
    Snsr_TED_5 = 11,
    Snsr_TED_6 = 10,

}Sensors;

#endif // ELEMENTS_H

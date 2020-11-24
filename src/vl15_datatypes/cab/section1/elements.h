#ifndef ELEMENTS_H
#define ELEMENTS_H


typedef enum sounds
{
    Default_Tumbler = 24,
    Kran_254 = 51,
    Kran_394 = 52,
    BV = 17,
    EPK_INIT = 106,
    TP_UP = 13,
    TP_DOWN = 14,
    MK = 101,
    MV_low = 108,
    Revers = 2,
    PesokButton = 4,
    FinalStop = 54,
}SoundsID;

typedef enum en_Tumblers
{
    Tmb_Panto = 13,
    Tmb_Panto1_3 = 14,
    Tmb_Panto2_4 = 15,
    Tmb_vozvrBV1 = 16,
    Tmb_BV1 = 17,
    Tmb_BV2_on = 12,
    Tmb_BV2_off = 11,
    Tmb_MK_ = 9,
    Tmb_MV_low = 10,
    Tmb_Searchlight_Bright = 18,
    Tmb_Searchlight_Dimly = 19,

}Tumblers;


typedef enum en_Buttons
{
    Btn_Svistok = 37,
    Btn_Tifon = 39,
    Btn_Pesok = 29,
}Buttons;


typedef enum en_Equipment
{
  SL_2M = 300,
}Equipment;

typedef enum en_Saut
{
    Start_Drive = 204,
    STATION = 202,
    Drive_Backward = 203,
    GREEN = 206,
    YELLOW = 207,
    KG = 208,
    RED = 209,
    WHITE = 210
}
SAUT;

typedef enum en_Arms
{
  Arm_394 = 0,
  Arm_254 = 1,
  Arm_Reverse = 2,
  Arm_Zadatchik = 4,
}Arms;

typedef enum en_Keys
{
    Key_EPK = 8,
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

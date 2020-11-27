#include <sys/timeb.h>
#include <stdio.h>
#include <wchar.h>
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>  // For _CrtSetReportMode
#include <errno.h>

#include "utils/utils.h"
#include "epk.h"
#include "saut.h"
#include "math.h"
#include "vl15.h"

#define TO_KM_PH (3.6)


/*** Закрытые функции ***/
static void SL2m_step(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self);
static void HodovayaSound(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self);
static int m_getDest(st_Self *self);

static float m_tractionВorce ( st_Self *self);
static void _osveshenie(const ElectricLocomotive * loco, st_Self *self);
static void _checkBrake(const ElectricLocomotive * loco, ElectricEngine *eng, st_Self *self);
static void _setBrakePosition(int newPos, const ElectricLocomotive * loco, ElectricEngine *eng, st_Self *self);
static en_SignColors _getForwardSignColor(SignalsInfo *signal);
static void _debugPrint(ElectricEngine *eng, st_Self *self);
/************************/

static SAUT saut;
static EPK epk;

bool VL15_init(const ElectricLocomotive *loco, st_Self *self)
{
    memset(self, 0, sizeof (struct st_Self));
    self->pneumo.Arm_254 = 6 - _checkSwitchWithSound(loco, Arms::Arm_254, -1, 1 );
    self->pneumo.Arm_394 = 7 - _checkSwitchWithSound(loco, Arms::Arm_394, -1, 1 );

    self->brake394_pos = self->pneumo.Arm_394;
    ftime(&self->debugTime.prevTime);
    self->debugTime.currTime = self->debugTime.prevTime;

    self->SL2M_Ticked = false;
    self->Reverse = NEUTRAL;

    VL15_set_destination(self, SECTION_DEST_BACKWARD);
    saut.init();
    Radiostation_Init(&self->radio);
    Radiostation_setEnabled(&self->radio, 1);

    self->sautData.SpeedLimit.Limit = 0.0;
    self->sautData.SpeedLimit.Distance = 0.0;
    return true;
}

void VL15_set_destination(st_Self *self, int SectionDest)
{
    self->secdionCabDest = SectionDest;
}

void VL15_ALSN(const Locomotive *loco, UINT NumSigAhead,
                SignalsInfo *sigAhead,UINT NumSigBack, SignalsInfo *sigBack, st_Self *self)
{
    self->sautData.PrevSpeed = self->prevVelocity;
    self->sautData.CurrSpeed = self->Velocity;
    self->Velocity = fabs(double(loco->Velocity)) * TO_KM_PH; // переводим из м/с в км/ч

    if ( (sigBack != NULL) && (NumSigBack> 0) )
        self->sautData.prevSignalColor = _getForwardSignColor(sigBack);

    if ( (sigAhead != NULL) && (NumSigAhead> 0) )
        self->sautData.forwardSignalColor = _getForwardSignColor(sigAhead);

    for (UINT i=0; i< NumSigAhead && i < SIGNALS_CNT; i++)
        self->SignalColor[i] = sigAhead->Aspect[i];

    wchar_t* name = sigAhead->SignalInfo->Name;
    swprintf(self->sautData.signalName, sizeof(self->sautData.signalName),  L"%s", name);
}

int VL15_Step(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self)
{
    /*Грузим данные из движка себе в МОЗГИ*/
    self->elecrto.LineVoltage =  loco->LineVoltage;
    Cabin *cab = loco->cab;

    cab->SetDisplayState(6, 1);
    cab->SetDisplayValue(6, 5.2);
    cab->SetDisplayValue(7, 1.7);

    /*Работаем в своём соку*/
    _checkBrake(loco, eng, self);
    _osveshenie(loco, self);
    Radiostation_Step(loco, eng, &self->radio);
    SL2m_step(loco, eng, self);
    HodovayaSound(loco, eng, self);

    static int prevEpk = self->EPK;
    if ( prevEpk != self->EPK)
    {
        if (prevEpk == 0)
            saut.start(loco, eng, self->sautData);
        else
            saut.stop(loco, eng);
        prevEpk = self->EPK;
    }
    else
    {
        if (self->RB)
            epk.okey(loco);

        int currEpkState = epk.step(loco,  saut.step(loco, eng, self->sautData));
        //static int emergencyStop = currEpkState;
        if ( currEpkState )
        {
               if (!self->flags.EPK_Triggered)
                _setBrakePosition(7, loco, eng, self);
               //eng->IndependentBrakeValue = 6.0;
               self->flags.EPK_Triggered = 1;
        }
    }


    /*А тепер пихаем из наших МОЗГОВ данные в движок*/
    eng->Panto = ((unsigned char)self->elecrto.PantoRaised);
    eng->ThrottlePosition = self->ThrottlePosition;
    eng->Reverse = self->Reverse;
    eng->IndependentBrakeValue= ( self->pneumo.Arm_254 - 1) * 1.0;
    self->prevVelocity = self->Velocity;


    //float Voltage = self->elecrto.LineVoltage;
    float SetForce = m_tractionВorce (self);

    if(self->Velocity <= 3.01)
        SetForce *= 4000.0;
    else
        SetForce *= 6800.0;

    eng->Force = SetForce;
     _debugPrint( eng, self);
    // так как у нас всегда всё нормалёк - возвращаем 1
    return 1;
}

/**
 * @brief SL2m_step Шаг работы Скоростемера
 * @param loco
 * @param eng
 * @param self
 */
static void SL2m_step(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self)
{
    if (self->Velocity >= 5.0)
    {
        if (self->SL2M_Ticked == false )
        {
            loco->PostTriggerCab(Equipment::SL_2M);
            self->SL2M_Ticked = true;
        }
    }
    else
    {
        if (self->SL2M_Ticked == true )
        {
            loco->PostTriggerCab(Equipment::SL_2M + 1);
            self->SL2M_Ticked = false;
        }
    }
}

/**
 * @brief HodovayaSound Озвучка ходовой части
 * @param loco
 * @param eng
 * @param self
 */
static void HodovayaSound(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self)
{
    static int isFinalStop = 0;
    float currSpeed = fabs(self->Velocity);
    float prevSpeed = fabs(self->prevVelocity);

    if ( (prevSpeed > currSpeed) && (currSpeed > 0.3)  ) // типа, быстро так тормозим
    {
        if ((self->Velocity < 3.0) && (isFinalStop == 0)  )
        {
            isFinalStop = 1;
            loco->PostTriggerCab(SoundsID::FinalStop );
            return;
        }
    }

    if (fabs(self->Velocity) <= 0.00)
    {
        if (isFinalStop )
            isFinalStop = 0;
    }
}

static int m_getDest( st_Self *self)
{
    if (self->Reverse > NEUTRAL)
        return SECTION_DEST_FORWARD * self->secdionCabDest;
    else if (self->Reverse < NEUTRAL )
        return SECTION_DEST_BACKWARD * self->secdionCabDest;
    else
        return 0;
}

static float m_tractionВorce ( st_Self *self)
{
    float startForce = 0.0;
    if (self->BV_STATE == 0)
        return 0.0;
    if (self->elecrto.PantoRaised == 0)
        return 0.0;
    startForce = (float) (( self->Reverse - NEUTRAL) * self->ThrottlePosition )  ;
    startForce *= float(m_getDest(self));
    return startForce;
}

static void _checkBrake(const ElectricLocomotive * loco, ElectricEngine *eng, st_Self *self)
{
    /*корректируем показатели */
    if (self->pneumo.Arm_254 < 1)
        self->pneumo.Arm_254 = 1;

    if (self->pneumo.Arm_394 < 1)
        self->pneumo.Arm_394 = 1;

    if ( self->brake394_pos == self->pneumo.Arm_394)
        return;

    if (self->flags.EPK_Triggered )
    {    if (self->pneumo.Arm_394 == 7)
        {
            _setBrakePosition(self->pneumo.Arm_394, loco, eng, self);
            self->flags.EPK_Triggered = 0;
        }
        else
            return;
    }
    else
        _setBrakePosition(self->pneumo.Arm_394, loco, eng, self);
}

static void _setBrakePosition(int newPos, const ElectricLocomotive * loco, ElectricEngine *eng, st_Self *self)
{
    if (self->brake394_pos == 7)
        loco->PostTriggerCab(SoundsID::Kran394_Extrennoe +1);

    if ((self->brake394_pos == 5) || (self->brake394_pos == 6))
        loco->PostTriggerCab(SoundsID::Kran394_Slugebnoe +1);
    if (self->brake394_pos == 2)
        loco->PostTriggerCab(SoundsID::Kran394_Poezdnoe +1);

    if (self->brake394_pos == 1)
        loco->PostTriggerCab(SoundsID::Kran394_Otpusk +1);

    if (newPos == 7)
        loco->PostTriggerCab(SoundsID::Kran394_Extrennoe);
    else if ( (newPos == 5) || (newPos == 5) )
        loco->PostTriggerCab(SoundsID::Kran394_Slugebnoe);

    else if ((newPos == 4) || (newPos == 3))
        loco->PostTriggerCab(SoundsID::Kran394_Slugebnoe + 1);

    else if (newPos == 2)
        loco->PostTriggerCab(SoundsID::Kran394_Poezdnoe);

    else if (newPos == 1)
        loco->PostTriggerCab(SoundsID::Kran394_Otpusk);
    else
        loco->PostTriggerCab(SoundsID::Kran394_Otpusk + 1);
    self->brake394_pos = newPos;
    eng->BrakeForce = (self->brake394_pos - 1) * 2.0;
    eng->ChargingRate = (self->brake394_pos -1) * 2.0;
}


static void _osveshenie(const ElectricLocomotive * loco, st_Self *self)
{
    loco->SwitchLight(en_Lights::Light_Proj_Half1, self->tumblers.projHalf, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Half2, self->tumblers.projHalf, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Half3, self->tumblers.projHalf, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Half4, self->tumblers.projHalf, 0.0, 0);

    loco->SwitchLight(en_Lights::Light_Proj_Full1, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full2, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full3, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full4, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full5, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full6, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full7, self->tumblers.projFull, 0.0, 0);
    loco->SwitchLight(en_Lights::Light_Proj_Full8, self->tumblers.projFull, 0.0, 0);
}

/**
 * @brief _getForwardSignColor Получить цвет сигнала впереди
 * @param eng
 * @return
 */
static en_SignColors _getForwardSignColor(SignalsInfo *signal)
{
    unsigned int Aspect = signal->Aspect[0];
    if ( (Aspect == SIGASP_CLEAR_1 ) || (Aspect == SIGASP_CLEAR_2) )
       { return en_SignColors::COLOR_GREEN; }
    else if ( (Aspect == SIGASP_APPROACH_1) || (Aspect == SIGASP_APPROACH_2) || (Aspect == SIGASP_APPROACH_3) )
       { return en_SignColors::COLOR_YELLOW; }
    else if (Aspect == SIGASP_STOP_AND_PROCEED)
       { return en_SignColors::COLOR_RD_YEL; }
    else if (Aspect == SIGASP_RESTRICTING)
       { return en_SignColors::COLOR_WHITE; }
    else
    {
        // по идее 0 и 8
        return en_SignColors::COLOR_RED;
    }
}

/**
 * @brief _debugPrint
 * @param loco
 * @param eng
 * @param self
 */
static void _debugPrint(ElectricEngine *eng, st_Self *self)
{
    ftime(&self->debugTime.currTime);
    if ((self->debugTime.prevTime.time + 1) > self->debugTime.currTime.time)
        return;

    Printer_print(eng, GMM_POST, L"kran254 %f Kran394: %f, Signal: %d EPK %d \
                  Speed: %f Limit: %f NextLimit: %f Force: %f SigName: %s\n",
                eng->IndependentBrakeValue, eng->BrakeForce, self->sautData.forwardSignalColor,
                self->flags.EPK_Triggered,
                self->Velocity, self->sautData.SpeedLimit.Limit,
                self->sautData.SpeedLimit.NextLimit, eng->Force, self->sautData.signalName);
    self->debugTime.prevTime = self->debugTime.currTime;
}


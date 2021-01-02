
#include <windows.h>
#include <math.h>
#include <stdio.h>

#include <cmath>

/*#define RTS_NODIRECTOBJLINKS*/
#include "src/utils/utils.h"
#include "src/private_vl_15.h"
#include "src/vl15_datatypes/cab/section1/elements.h"
#include "../VL_15.h"

#include "vl15.h"

#define MAX_LOCOS 12 // макс. кол-во локов - с учётом перспективы на МВПС

#pragma hdrstop
#pragma argsused

#define TR_CURRENT_C 272.0
#define TO_KM_PH (3.6)

static struct st_Self SELF;
static int isInitialised = 0;

//ElectricLocomotive *BackSections_ptr[MAX_LOCOS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
//UINT *BackSecFlags_ptr[MAX_LOCOS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

struct st_Selfservice
{
    int state;
};

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
    return 1;
}

/****************************
 * Открытые функции библиотеки
 *******************************/


/**
 * Инициализация локомотива.
 * Возвращает true в случае успеха и false в случае ошибки.
 */
extern "C" bool Q_DECL_EXPORT Init
 (ElectricEngine *eng,ElectricLocomotive *loco,unsigned long State,
        float time,float AirTemperature)
{
 isInitialised = 0;
 loco->HandbrakeValue=0.0;
 eng->HandbrakePercent=0.0;
 eng->DynamicBrakePercent=0;
 eng->MainResRate=0.0;
 eng->Sanding=0;
 eng->BrakeSystemsEngaged=0;
 eng->BrakeForce=0.0;

 eng->ChargingRate=0;
 eng->TrainPipeRate=0;
 eng->UR=0.0;

/*
 eng->var[0]=0;
 eng->var[1]=0.0;
 eng->var[2]=0.0;
 eng->var[4]=0.0;
 eng->var[5]=0.0;
 eng->var[6]=0.0;
 eng->var[7]=0.0;
 eng->var[8]=0.0;
 eng->var[9]=0.0;
 eng->var[10]=0.0;
 eng->var[11]=0.0;
 eng->var[12]=0.0;
 eng->var[13]=0.0;
 eng->var[14]=25.0;
 eng->var[15]=0.0;
 eng->var[16]=0.0;
 eng->var[17]=0.0;*/
 eng->AuxilaryRate=0.0;
 eng->ALSNOn=0;
 eng->EPTvalue = 0.0;
 eng->Reverse = NEUTRAL;
 eng->ThrottlePosition = 1;
 SELF.service.LocoState = 0;
 Cabin *cab=eng->cab;
 UINT *Flags=(UINT *)&eng->var[0];
 switch(State&0xFF)
 {
 case 0:
     SELF.service.LocoState = 0;

     if((State>>8)&1){
      loco->LocoFlags|=1;
     };
     cab->SetSwitch(3,2,true);
     cab->SetSwitch(0,0,0,true);
     cab->SetSwitch(0,1,0,true);
     cab->SetSwitch(0,2,0,true);
     loco->MainResPressure=7.0;
     loco->TrainPipePressure=5.0;
     loco->AuxiliaryPressure=5.2;
     eng->UR=4.0;
     eng->Power=0;
     eng->Force=0;
     loco->BrakeCylinderPressure=0.0;
     eng->IndependentBrakeValue=4.0;
     eng->TrainPipeRate=0.0;
     eng->ThrottlePosition=0;
     eng->Reverse=0;
     eng->ALSNOn=0;
     eng->MainSwitch=0;
     eng->Panto=0;
     *Flags|=32<<8;

     break;
 case 1:
     SELF.service.LocoState = 1;
     eng->UR=5.5;
     loco->IndependentBrakePressure=0.0;
     loco->TrainPipePressure=0.0;
     loco->AuxiliaryPressure=0.0;
     loco->BrakeCylinderPressure=0.0;
     loco->MainResPressure=0.0;
     eng->IndependentBrakeValue=0.0;
     break;
 }

 SELF.dest = -1;
 if ( VL15_init(&SELF, loco ))
     isInitialised = 1;

 return isInitialised;
}


extern "C" void Q_DECL_EXPORT ALSN ( Locomotive *loco, SignalsInfo *sigAhead, UINT NumSigAhead,
    SignalsInfo *sigBack,UINT NumSigBack, SignalsInfo *sigPassed, UINT NumPassed,
        float SpeedLimit, float NextLimit,
        float DistanceToNextLimit, bool Backwards )
{

    SELF.alsn.SpeedLimit.Distance = DistanceToNextLimit;
    SELF.alsn.SpeedLimit.Limit = (float)(SpeedLimit * TO_KM_PH);
    SELF.alsn.SpeedLimit.NextLimit = (float)(NextLimit * TO_KM_PH);
    SELF.alsn.CurrSpeed = (float)( fabs(double(loco->Velocity)) * TO_KM_PH);

    SELF.isBackward = (int)Backwards;

    /*загружаем информацию по сигналам */
    SELF.alsn.NumSigForw = NumSigAhead;
    SELF.alsn.NumSigBack = NumSigBack;
    SELF.alsn.NumSigPassed = NumPassed;

    if (sigAhead)
    {
        swprintf(SELF.alsn.signalName, sizeof(SELF.alsn.signalName),  L"%s", sigAhead->SignalInfo->Name);
        SELF.alsn.SpeedLimit.NextLimit = sigAhead[0].SpeedLimit;
        for (UINT i=0; i< NumSigAhead; i++)
        {
            SELF.alsn.ForwardSignalsList[i] = sigAhead[i];
        }
    }
    else
        swprintf(SELF.alsn.signalName, sizeof(SELF.alsn.signalName),  L"UNSET");
    if (sigBack)
        SELF.alsn.signListBack =  *sigBack;

    VL15_ALSN(&SELF, loco );
}


extern "C" void Q_DECL_EXPORT Run
 (ElectricEngine *eng,const ElectricLocomotive *loco, unsigned long State,
        float time,float AirTemperature)
{
     VL15_Step(&SELF, loco, eng);
}


extern "C" void Q_DECL_EXPORT  ChangeLoco
(Locomotive *loco,const Locomotive *Prev, unsigned long State)
{
 if(Prev)
 {
  if(!Prev->Eng()->Reverse)
   if(!Prev->Cab()->Switch(132) ||
        (Prev->Cab()->Switch(55)+Prev->Cab()->Switch(58)==2))
   loco->LocoFlags|=1;
 }else
  loco->LocoFlags|=1;
  //  VL15_init(&SELF, loco );
}

extern "C" void  Q_DECL_EXPORT  LostMaster
(Locomotive *loco,const Locomotive *Prev, unsigned long State)
{
 UINT &Flags=*(UINT *)&loco->locoA->var[0];
 Engine *eng=loco->Eng();
 UINT &AsyncFlags=*(UINT *)&eng->var[4];
 AsyncFlags&=~14016;
 AsyncFlags&=~(2<<16);
 //eng->var[11]=0.0;
 if(eng->ThrottlePosition>36)
  eng->ThrottlePosition-=9;
 //Flags&=~1272;
}

extern "C" bool Q_DECL_EXPORT CanWorkWith(const Locomotive *loco, const wchar_t *Type)
{
 //if(!lstrcmpiW(Type,L"vl15"))
  return true;
 //return false;
}


extern "C" bool Q_DECL_EXPORT  CanSwitch(const ElectricLocomotive *loco, const ElectricEngine *eng,
        unsigned int SwitchID, unsigned int SetState)
{

 //if (isInitialised == 0)
 //    return false;
 //Cabin *cab=loco->Cab();

 if(SwitchID==Arm_Reverse)
 {
    if ( (loco->Velocity > 0.01) || (loco->Eng()->Force > 0.01))
        return false;
    if (eng->ThrottlePosition > 1)
        return false;
    return true;
 }
 if (SwitchID==Arm_Zadatchik)
 {
     if (SELF.Reverse != NEUTRAL)
         return true;
     return false;
 }
 return true;
}


/**
 * Реакция на переключение элементов
 */
extern "C" void Q_DECL_EXPORT Switched(const ElectricLocomotive *loco,ElectricEngine *eng,
        unsigned int SwitchID,unsigned int PrevState)
{

    ElectricLocomotive *BackSec=NULL;
    if(loco->NumSlaves)
     BackSec=(ElectricLocomotive *)loco->Slaves[0];

    if(BackSec)
        BackSec->loco->IndependentBrakeValue=eng->IndependentBrakeValue;

    switch(SwitchID)
    {
    case Tumblers::Tmb_Panto:
        SELF.tumblers.panto = _checkSwitchWithSound(loco, Tumblers::Tmb_Panto, SoundsID::Default_Tumbler, 1);
        if (SELF.tumblers.panto == 0)
        {
            int isSound =0;
            if  (SELF.tumblers.panto1_3 == 1)
            {
                SELF.tumblers.panto1_3 = 0;
                SELF.elecrto.PantoRaised &=~(1UL << 0);
                SELF.elecrto.PantoRaised &=~(1UL << 2);
                isSound = 1;
            }
            if (SELF.tumblers.panto2_4 == 1)
            {
                SELF.tumblers.panto2_4 = 0;
                SELF.elecrto.PantoRaised &=~(1UL << 1);
                SELF.elecrto.PantoRaised &=~(1UL << 3);
                isSound = 1;
            }
            if (isSound)
                loco->PostTriggerCab(SoundsID::TP_DOWN);

        }
        else
        {
            int isSound =0;
            if  (SELF.tumblers.panto1_3 == 1)
            {
                SELF.elecrto.PantoRaised |=~(1UL << 0);
                SELF.elecrto.PantoRaised |=~(1UL << 2);
                isSound = 1;
            }
            if (SELF.tumblers.panto2_4 == 1)
            {
                SELF.elecrto.PantoRaised |=~(1UL << 1);
                SELF.elecrto.PantoRaised |=~(1UL << 3);
                isSound = 1;
            }
            if (isSound)
            {
                loco->PostTriggerCab(SoundsID::TP_UP);
            }
        }
        break;
    case Tumblers::Tmb_Panto1_3:
        SELF.tumblers.panto1_3 = _checkSwitchWithSound(loco, Tumblers::Tmb_Panto1_3, SoundsID::Default_Tumbler, 1);
        if (SELF.tumblers.panto)
        {
            if (SELF.tumblers.panto1_3)
            {
                SELF.elecrto.PantoRaised |= 1UL << 0;
                SELF.elecrto.PantoRaised |= 1UL << 2;
                loco->PostTriggerCab(SoundsID::TP_UP);
            }
            else
            {
                SELF.elecrto.PantoRaised &=~(1UL << 0);
                SELF.elecrto.PantoRaised &=~(1UL << 2);
                loco->PostTriggerCab(SoundsID::TP_DOWN);
             }
        }
        break;
    case Tumblers::Tmb_Panto2_4:
        SELF.tumblers.panto2_4 = _checkSwitchWithSound(loco, Tumblers::Tmb_Panto2_4, SoundsID::Default_Tumbler, 1);
        if (SELF.tumblers.panto)
        {
            if (SELF.tumblers.panto2_4)
            {
                SELF.elecrto.PantoRaised |= 1UL << 1;
                SELF.elecrto.PantoRaised |= 1UL << 3;
                loco->PostTriggerCab(SoundsID::TP_UP);
            }
            else
            {
                SELF.elecrto.PantoRaised &=~(1UL << 1);
                SELF.elecrto.PantoRaised &=~(1UL << 3);
                loco->PostTriggerCab(SoundsID::TP_DOWN);
            }
        }
        break;
    case Arms::Arm_254:
        SELF.pneumo.Arm_254 = 6 - _checkSwitchWithSound(loco, Arms::Arm_254, SoundsID::Kran_254, 1);
        break;
    case Arms::Arm_394:
        SELF.pneumo.Arm_394 = 7 -  _checkSwitchWithSound(loco, Arms::Arm_394, SoundsID::Kran_394, 1);
        break;

    case Buttons::Btn_Tifon:
        _checkSwitchWithSound(loco, Buttons::Btn_Tifon, Buttons::Btn_Tifon, 0, 2);
        break;
    case Buttons::Btn_Svistok:
        _checkSwitchWithSound(loco, Buttons::Btn_Svistok, Buttons::Btn_Svistok, 0, 2);
        break;
    case Buttons::Btn_Pesok:
        _checkSwitchWithSound(loco, Buttons::Btn_Pesok, SoundsID::PesokButton, 0);
        break;
    case Buttons::Btn_RescueBrake:
        SELF.RB = _checkSwitchWithSound(loco, Buttons::Btn_RescueBrake, SoundsID::Default_Button, 0);
        break;

    case Tumblers::Tmb_MK_:
        SELF.MK =  _checkSwitchWithSound(loco, Tumblers::Tmb_MK_, Default_Tumbler, 1);
        if (SELF.MK)
        {
            loco->PostTriggerCab(SoundsID::MK);
        }
        else
            loco->PostTriggerCab(SoundsID::MK+1);
        break;
    case Tumblers::Tmb_MV_low:
        SELF.MV_low =  _checkSwitchWithSound(loco, Tumblers::Tmb_MV_low, Default_Tumbler, 1);
        if (SELF.MV_low)
        {
            loco->PostTriggerCab(SoundsID::MV_low);
        }
        else
            loco->PostTriggerCab(SoundsID::MV_low + 1);
        break;
    case  Keys::Key_EPK:
        SELF.EPK = _checkSwitchWithSound(loco, Keys::Key_EPK, SoundsID::EPK_INIT, 1);

        break;
    case Tumblers::Tmb_BV1:
        SELF.tumblers.bv1 = _checkSwitchWithSound(loco, Tumblers::Tmb_BV1, Default_Tumbler, 1);
        if (SELF.tumblers.bv1 == 0)
        {
            loco->PostTriggerCab(SoundsID::BV+1);
            SELF.BV_STATE = 0;
        }
        break;
    case Tumblers::Tmb_vozvrBV1:
        if( _checkSwitchWithSound(loco, Tumblers::Tmb_vozvrBV1, Default_Tumbler, 1) == 1 )
        {
            if (SELF.tumblers.bv1)
            {
                loco->PostTriggerCab(SoundsID::BV);
                SELF.BV_STATE = 1;
            }
        }
      break;
    case Tumblers::Tmb_Searchlight_Dimly:
        SELF.tumblers.projHalf = _checkSwitchWithSound(loco, Tumblers::Tmb_Searchlight_Dimly, Default_Tumbler, 1);
        break;
    case Tumblers::Tmb_Searchlight_Bright:
        SELF.tumblers.projFull = _checkSwitchWithSound(loco, Tumblers::Tmb_Searchlight_Bright, Default_Tumbler, 1);
        break;

    case Arms::Arm_Reverse:
        SELF.Reverse = _checkSwitchWithSound(loco,  Arms::Arm_Reverse, SoundsID::Revers, 1);
        break;
    case Arms::Arm_Zadatchik:
        SELF.ThrottlePosition = _checkSwitchWithSound(loco,  Arms::Arm_Zadatchik, SoundsID::Controller, 1);
        break;
    default:
        break;
    }
}



extern "C" void Q_DECL_EXPORT SpeedLimit(const Locomotive *loco,
        SpeedLimitDescr Route,SpeedLimitDescr Signal,SpeedLimitDescr Event)
{

    //Printer_print(loco->Eng(), GMM_POST, L"Speed Limit!\n");
    //SELF.sautData.SpeedLimit =  Event;

}

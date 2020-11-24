
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


#pragma hdrstop
#pragma argsused

#define TR_CURRENT_C 272.0


wchar_t E_99[] = {L"E99"};
wchar_t WIPE_LEFT[] ={L"wiperleft"};
wchar_t WIPE_RIGHT[] ={L"wiperright"};

/**
  * Структура с собственными параметрами для работы между функциями библиотеки.
  **/
static struct st_Self
{
  int dest; // 1 ,0,  -1
  st_Tumblers tumblers;
  Electric elecrto;
  Pneumo pneumo;
  float Velocity;
  bool SL2M_Ticked;
  int BV_STATE;
  int EPK;
  int MK;
  int MV_low;
  int isFinalStop = 0;
}SELF;

static void _initSelf(struct st_Self *selfStr)
{
    memset(selfStr, 0, sizeof (struct st_Self));
    selfStr->SL2M_Ticked = false;
}


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
 loco->HandbrakeValue=0.0;
 eng->HandbrakePercent=0.0;
 eng->DynamicBrakePercent=0;
 eng->Sanding=0;
 eng->BrakeSystemsEngaged=0;
 eng->BrakeForce=0.0;
 eng->var[0]=0;
 eng->ChargingRate=0;
 eng->TrainPipeRate=0;
 eng->var[1]=0.0;
 eng->var[2]=0.0;
 eng->UR=0.0;
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
 eng->var[17]=0.0;
 eng->AuxilaryRate=0.0;


 eng->EPTvalue = 0.0;
 eng->Reverse =0;

 switch(State&0xFF){
  case 1:
   eng->UR=5.5;
   eng->HandbrakePercent=0.0;
   loco->IndependentBrakePressure=0.0;
   loco->TrainPipePressure=0.0;
   loco->AuxiliaryPressure=0.0;
   loco->BrakeCylinderPressure=0.0;
   loco->MainResPressure=0.0;
   eng->IndependentBrakeValue=0.0;
  break;
 };

 _initSelf(&SELF);
 SELF.dest = -1;
 return true;

};


extern "C" void Q_DECL_EXPORT Run
 (ElectricEngine *eng,const ElectricLocomotive *loco,unsigned long State,
        float time,float AirTemperature)
{
 eng->MainResRate=0.0;
 eng->Panto = SELF.elecrto.Panto;
 Cabin *cab=loco->Cab();
 static unsigned int ThrottlePosition = 1;
static int Reverse = 1;

 eng->Reverse = cab->Switch(Arm_Reverse);

   float Velocity = fabs(loco->Velocity);

   eng->ThrottlePosition = cab->Switch(Arm_Zadatchik);
   float LineVoltage =  loco->LineVoltage;
   if (ThrottlePosition < 1)
    ThrottlePosition = 1;

   float SetForce = 0.0;
   if(Velocity<=3.01)
       SetForce= 400000.0 * (ThrottlePosition-1);
   else
   {
       SetForce = (680000.0* ThrottlePosition-1);
       /*SetForce = (340000.0*eng->ThrottlePosition)/
                   (Velocity*Velocity*std::pow(0.978,eng->ThrottlePosition)) +
                   shunt*468750.0/Velocity;*/
   }


   if (ThrottlePosition != eng->ThrottlePosition )
   {
       ThrottlePosition = eng->ThrottlePosition;
       // от 1 до 6
       if (eng->Reverse > 2)
           SELF.dest = -1;
       else if (eng->Reverse < 2 )
           SELF.dest = 1;
       else
           SELF.dest = 0;
   }

   if (Reverse != eng->Reverse)
   {
       Reverse = eng->Reverse;
   }

   SetForce*= eng->Reverse*(LineVoltage/2000.0);
   eng->Force=eng->var[12];
   if(eng->Force<SetForce)
   {
       eng->Force+=(10000.0+(SetForce-eng->Force)*0.6)*time;
       if(eng->Force>SetForce)
           eng->Force=SetForce;
   }else if(eng->Force>SetForce)
   {
       //eng->Force-=3000.0*time;
       if(eng->Force<SetForce)
           eng->Force=SetForce;
   }
   eng->Force *= float(SELF.dest) * float(SELF.BV_STATE);

   if (SELF.pneumo.Arm_254 < 1)
       SELF.pneumo.Arm_254 = 1;

   SELF.pneumo.Arm_254 = 6 - _checkSwitchWithSound(loco, Arms::Arm_254, -1, 1);
   eng->IndependentBrakeValue= ( SELF.pneumo.Arm_254 - 1) * 1.0;

   if (Velocity >= 5.0)
   {
       if (SELF.SL2M_Ticked == false )
       {
           loco->PostTriggerCab(Equipment::SL_2M);
           SELF.SL2M_Ticked = true;
       }
   }
   else
   {
       if (SELF.SL2M_Ticked == true )
       {
           loco->PostTriggerCab(Equipment::SL_2M + 1);
           SELF.SL2M_Ticked = false;
       }
   }

   if (( SELF.Velocity == 0.0) && (fabs(loco->Velocity) > 0.0))
   {
       if ( (loco->Velocity) * float(SELF.dest) > 0.0 )
            loco->PostTriggerCab(SAUT::Start_Drive);
       else
            loco->PostTriggerCab(SAUT::Drive_Backward);
   }

   if (SELF.Velocity >  Velocity  ) // типа, быстро так тормозим
   {
       if ((SELF.Velocity < 3.0) && (SELF.isFinalStop == 0) )
       {
           SELF.isFinalStop = 1;
           //Printer_print(eng, GMM_POST, L"STarting finalstop!!!\n");
           loco->PostTriggerCab(SoundsID::FinalStop );
       }
   }

   SELF.Velocity = Velocity;

   if (SELF.Velocity == 0.0)
   {
       if (SELF.isFinalStop )
       {
           SELF.isFinalStop = 0;
           //Printer_print(eng, GMM_POST, L"STOP finalstop\n");
           loco->PostTriggerCab(SoundsID::FinalStop + 1);
       }
   }

   return; // с этим ретурном работат
   eng->Power=ThrottlePosition*28.8*(LineVoltage/26000.0)*fabs(eng->Force/TR_CURRENT_C)*4.0;

        if(loco->NumEngines){
         eng->EngineForce[0]= (eng->Force/4.0);// * SELF.dest;
         eng->EngineVoltage[0]=ThrottlePosition*26.5*(LineVoltage/000.0);
         eng->EngineCurrent[0]=fabs(eng->Force/TR_CURRENT_C);
         if(eng->EngineCurrent[0])
          eng->EngineCurrent[0]+=50.0;
         if(eng->cab->Switches[1].State>3)
          eng->EngineCurrent[0]*=1.0+(eng->cab->Switches[1].State-3)*0.05;
         //if(eng->Wheelslip)
          //eng->EngineCurrent[0]*=1.01+float(GetTickCount()%10)/200.0;
         if(eng->EngineCurrent[0]>1400.0||loco->BrakeCylinderPressure>3.0){
          //SwitchBV(loco,eng,0);
         // *Flags|=4;
          eng->Force=0;
          eng->Power=0;
          eng->EngineForce[0]=0;
          eng->EngineVoltage[0]=0;
          eng->EngineCurrent[0]=0;
         // if(BackSec)
         //  SwitchGV(BackSec,BackSec->loco,0);
         };
        }
        eng->EngineForce[0] *= SELF.dest;

        eng->EngineForce[1]=eng->EngineForce[0];
        eng->EngineCurrent[1]=eng->EngineCurrent[0];
        eng->EngineVoltage[1]=eng->EngineVoltage[0];
        eng->EngineForce[2]=eng->EngineForce[0];
        eng->EngineCurrent[2]=eng->EngineCurrent[0];
        eng->EngineVoltage[2]=eng->EngineVoltage[0];
        eng->EngineForce[3]=eng->EngineForce[0];
        eng->EngineCurrent[3]=eng->EngineCurrent[0];
        eng->EngineVoltage[3]=eng->EngineVoltage[0];

}


extern "C" void Q_DECL_EXPORT  ChangeLoco
(Locomotive *loco,const Locomotive *Prev,unsigned long State)
{
 /*if(Prev){
  if(!Prev->Eng()->Reverse)
   if(!Prev->Cab()->Switch(132) ||
        (Prev->Cab()->Switch(55)+Prev->Cab()->Switch(58)==2))
   loco->LocoFlags|=1;
 }else
  loco->LocoFlags|=1;*/
}

extern "C" void  Q_DECL_EXPORT  LostMaster
(Locomotive *loco,const Locomotive *Prev,unsigned long State)
{
 //UINT &Flags=*(UINT *)&loco->locoA->var[0];
 Engine *eng=loco->Eng();
 UINT &AsyncFlags=*(UINT *)&eng->var[4];
 AsyncFlags&=~14016;
 AsyncFlags&=~(2<<16);
 eng->var[11]=0.0;
 if(eng->ThrottlePosition>36)
  eng->ThrottlePosition-=9;
 //Flags&=~1272;
}

extern "C" bool Q_DECL_EXPORT CanWorkWith(const Locomotive *loco,const wchar_t *Type)
{
 if(!lstrcmpiW(Type,L"vl15"))
  return true;
 return false;
}


extern "C" bool Q_DECL_EXPORT  CanSwitch(const ElectricLocomotive *loco,const ElectricEngine *eng,
        unsigned int SwitchID,unsigned int SetState)
{
 Cabin *cab=loco->Cab();

 if(SwitchID==Arm_Reverse)
 {
    if ( (loco->Velocity > 0.01) || (loco->Eng()->Force > 0.01))
        return false;
    return true;
 }

     /*
 if(SwitchID==33 || SwitchID==38){
  if(!(loco->LocoFlags&1))
   return false;
  if(eng->ThrottlePosition)
   return false;
  if(eng->var[2]!=0.0)
   return false;
  if(eng->var[11]!=0.0)
   return false;
  if(!cab->Switch(132))
   return false;
  if(SwitchID==33){
   if(cab->SwitchSet(38))
    return false;
  }else if(SwitchID==38){
   if(cab->SwitchSet(33))
    return false;
  };
  loco->PostTriggerCab(15);
 }else if(SwitchID==0){
  loco->PostTriggerCab(17);
 }else if(SwitchID==1){
  if(SetState==1)
   Switched(loco,(ElectricEngine *)eng,1,0);
  loco->PostTriggerCab(18);
 }else if(SwitchID==11){
  if(eng->ThrottlePosition)
   return false;
  if(eng->var[2]!=0.0 && eng->var[2]!=11.0 && eng->var[2]!=16.0)
   return false;
  if(eng->var[11]!=0.0)
   return false;
  loco->PostTriggerCab(26);
 }else if(SwitchID==17 || SwitchID==18){
  if(SetState)
   loco->PostTriggerCab(48);
 }else if((SwitchID>=2 && SwitchID<=16 && SwitchID!=11)||SwitchID==37){
  if(SetState)
   loco->PostTriggerCab(108);
 }else if((SwitchID>=19 && SwitchID<33) || (SwitchID>33&&SwitchID<=36) ||
        (SwitchID>=39&&SwitchID<=114) )
 {
  loco->PostTriggerCab(26);
 }else if((SwitchID>=135 && SwitchID<=138)||(SwitchID>=121 && SwitchID<=132)){
  loco->PostTriggerCab(108);
 }else if(SwitchID==119 || SwitchID==117 || SwitchID==116 || SwitchID==120){
  loco->PostTriggerCab(18);
 };
*/

 return true;
}


/**
 * Реакция на переключение элементов
 */
extern "C" void Q_DECL_EXPORT Switched(const ElectricLocomotive *loco,ElectricEngine *eng,
        unsigned int SwitchID,unsigned int PrevState)
{

    Cabin *cab=loco->Cab();

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
                SELF.elecrto.Panto &=~(1UL << 0);
                isSound = 1;
            }
            if (SELF.tumblers.panto2_4 == 1)
            {
                SELF.tumblers.panto2_4 = 0;
                SELF.elecrto.Panto &=~(1UL << 1);
                isSound = 1;
            }
            if (isSound)
            {
                loco->PostTriggerCab(SoundsID::TP_DOWN);
            }
        }
        break;
    case Tumblers::Tmb_Panto1_3:
        SELF.tumblers.panto1_3 = _checkSwitchWithSound(loco, Tumblers::Tmb_Panto1_3, SoundsID::Default_Tumbler, 1);
        if (SELF.tumblers.panto)
        {
            if (SELF.tumblers.panto1_3)
            {
                SELF.elecrto.Panto |= 1UL << 0;
                loco->PostTriggerCab(SoundsID::TP_UP);
            }
            else
            {
                SELF.elecrto.Panto &=~(1UL << 0);
                {
                loco->PostTriggerCab(SoundsID::TP_DOWN);
                }
            }
        }
        break;
    case Tumblers::Tmb_Panto2_4:
        SELF.tumblers.panto2_4 = _checkSwitchWithSound(loco, Tumblers::Tmb_Panto2_4, SoundsID::Default_Tumbler, 1);
        if (SELF.tumblers.panto)
        {
            if (SELF.tumblers.panto2_4)
            {
                SELF.elecrto.Panto |= 1UL << 1;
                loco->PostTriggerCab(SoundsID::TP_UP);
            }
            else
            {
                SELF.elecrto.Panto &=~(1UL << 1);
                {
                    loco->PostTriggerCab(SoundsID::TP_DOWN);
                }
            }
        }
        break;
    case Arms::Arm_254:

        break;
    case Arms::Arm_394:
        SELF.pneumo.Arm_394 = 7 -  _checkSwitchWithSound(loco, Arms::Arm_394, SoundsID::Kran_394, 1);
        break;

    case Buttons::Btn_Tifon:
        _checkSwitchWithSound(loco, Buttons::Btn_Tifon, Buttons::Btn_Tifon, 0);
        break;
    case Buttons::Btn_Svistok:
        _checkSwitchWithSound(loco, Buttons::Btn_Svistok, Buttons::Btn_Svistok, 0);
        break;
    case Buttons::Btn_Pesok:
        _checkSwitchWithSound(loco, Buttons::Btn_Pesok, SoundsID::PesokButton, 0);
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
        if (SELF.EPK)
        {
            loco->PostTriggerCab(SAUT::WHITE);
        }
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
    default :
        break;
    }
}



extern "C" void Q_DECL_EXPORT SpeedLimit(const Locomotive *loco,
        SpeedLimitDescr Route,SpeedLimitDescr Signal,SpeedLimitDescr Event)
{
 if(!(loco->LocoFlags&1))
  return;

 return;
}

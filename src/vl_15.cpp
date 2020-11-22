
#include <windows.h>
#include <math.h>
#include <stdio.h>
//#include <bitset>
/*#define RTS_NODIRECTOBJLINKS*/
#include "src/private_vl_15.h"
#include "src/vl15_datatypes/cab/section1/elements.h"
#include "../VL_15.h"

#include "vl15_logic.h"


#pragma hdrstop
#pragma argsused

VL15_logic VL15;

wchar_t E_99[] = {L"E99"};
wchar_t WIPE_LEFT[] ={L"wiperleft"};
wchar_t WIPE_RIGHT[] ={L"wiperright"};
static struct
{
 int Panto;
 float power;

}SELF;

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
 //UINT *Flags=(UINT *)&eng->var[0];
 UINT *AsyncFlags=(UINT *)&eng->var[4];
 Cabin *cab=loco->Cab();
 loco->HandbrakeValue=0.0;
 eng->HandbrakePercent=0.0;
 eng->DynamicBrakePercent=0;
 eng->Sanding=0;
 eng->BrakeSystemsEngaged=1;
 eng->BrakeForce=0.0;
 eng->var[0]=0;
 eng->ChargingRate=0;
 eng->TrainPipeRate=0;
 eng->var[1]=0.0;
 eng->var[2]=0.0;
 eng->UR=8.0;
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
 loco->MainResPressure=5.2;

 eng->EPTvalue = 0.0;
 eng->Reverse =0;

 /*switch(State&0xFF){
  case 1:
   eng->UR=5.5;
   eng->HandbrakePercent=0.0;
   loco->IndependentBrakePressure=3.0;
   loco->TrainPipePressure=5.5;
   loco->AuxiliaryPressure=5.3;
   loco->BrakeCylinderPressure=0.0;
   loco->MainResPressure=7.8;
   eng->Panto=3;
   eng->MainSwitch=1;
   eng->IndependentBrakeValue=3.0;
   *AsyncFlags|=588;
   cab->SetSwitch(0,1,true);
   cab->SetSwitch(1,1,true);
   cab->SetSwitch(22,0,true);
   cab->SetSwitch(24,0,true);
   cab->SetSwitch(25,0,true);
   cab->SetSwitch(26,10,true);
   cab->SetSwitch(27,0,true);
   cab->SetSwitch(30,0,true);
   cab->SetSwitch(32,8,true);
   cab->SetSwitch(115,0,true);
   cab->SetSwitch(122,1,true);
   cab->SetSwitch(123,1,true);
   cab->SetSwitch(124,1,true);
   cab->SetSwitch(125,1,true);
   cab->SetSwitch(127,1,true);
   if(State&(1<<8)){
    cab->SetSwitch(132,1,true);
    cab->SetSwitch(116,0,true);
    cab->SetSwitch(117,0,true);
   };
  break;
  case 2:
  case 3:
   eng->UR=5.5;
   eng->HandbrakePercent=0.0;
   loco->IndependentBrakePressure=0.0;
   loco->TrainPipePressure=5.5;
   loco->AuxiliaryPressure=5.3;
   loco->BrakeCylinderPressure=0.0;
   loco->MainResPressure=9.2;
   eng->Panto=3;
   eng->MainSwitch=1;
   eng->IndependentBrakeValue=0.0;
   *AsyncFlags|=4684;
   cab->SetSwitch(0,1,true);
   cab->SetSwitch(1,1,true);
   cab->SetSwitch(22,0,true);
   cab->SetSwitch(24,0,true);
   cab->SetSwitch(25,0,true);
   cab->SetSwitch(26,10,true);
   cab->SetSwitch(27,0,true);
   cab->SetSwitch(28,0,true);
   cab->SetSwitch(30,0,true);
   cab->SetSwitch(32,8,true);
   cab->SetSwitch(115,0,true);

   cab->SetSwitch(122,1,true);
   cab->SetSwitch(123,1,true);
   cab->SetSwitch(124,1,true);
   cab->SetSwitch(125,1,true);
   cab->SetSwitch(127,1,true);
   if(State&(1<<8)){
    cab->SetSwitch(38,2,true);
    cab->SetSwitch(132,1,true);
    cab->SetSwitch(116,0,true);
    cab->SetSwitch(117,0,true);
   };
   cab->SetDisplayState(Snsr_Yakor,10);
   cab->SetDisplayState(Snsr_TED_2,1);
  break;
 };
*/
 return VL15.init();


};


extern "C" void Q_DECL_EXPORT Run
 (ElectricEngine *eng,const ElectricLocomotive *loco,unsigned long State,
        float time,float AirTemperature)
{
 /*wchar_t buf[32];
 UINT &Flags=*(UINT *)&eng->var[0];
 UINT &AsyncFlags=*(UINT *)&eng->var[4];
 UINT LocoOn=IsLocoOn(loco,Flags);
 Cabin *cab=loco->Cab();
 SMSObject *soundExt=loco->SoundEng(),
           *soundCab=loco->SoundCab();
 bool CurrentOn=false,UpdateSlaves=false,AllowPipeCharging=false;*/
 static int ReversePrev = 0;
 eng->MainResRate=0.0;
 eng->Panto =  SELF.Panto;
 Cabin *cab=loco->Cab();SwitchBV(loco, eng, 1 );

 cab->SetDisplayValue(Snsr_Yakor, 10.6);

 eng->Reverse = cab->Switch(Arm_Reverse);
 if (eng->Reverse >= 2)
 {

    eng->ThrottlePosition = cab->Switch(Arm_Zadatchik);
    //eng->EngineCurrent = 200.0;


   if (ReversePrev != eng->Reverse)
   {

    loco->PostTriggerCab(Arm_Reverse );
    wchar_t text[1024];
    //swprintf(text, L"Reverse: %d\n", eng->Reverse);
   // eng->ShowMessage(GMM_POST, text);

    swprintf(text, L"Throttle: %u,  Flags: %lu  TC (254): %f  TC: %f  TM %f  NM %f  Voltage: %f\n",
             eng->ThrottlePosition,
             loco->LocoFlags,
             loco->IndependentBrakePressure,
             loco->BrakeCylinderPressure,
             loco->TrainPipePressure,
             loco->ChargingPipePressure,
             loco->LineVoltage);

    eng->ShowMessage(GMM_POST, text);

    /*if ( CHECK_BIT(eng->EngineFlags, 2) )
        swprintf(text, L"254 Released:\n");
    else
        swprintf(text, L"254 NOT Released:\n");
    eng->ShowMessage(GMM_POST, text);*/


    ReversePrev =  eng->Reverse;
   }

 }
 //else
 //    loco->PostTriggerCab(Arm_Reverse + 1); // сбрасываем звук
}


extern "C" void Q_DECL_EXPORT  ChangeLoco
(Locomotive *loco,const Locomotive *Prev,unsigned long State)
{
 if(Prev){
  if(!Prev->Eng()->Reverse)
   if(!Prev->Cab()->Switch(132) ||
        (Prev->Cab()->Switch(55)+Prev->Cab()->Switch(58)==2))
   loco->LocoFlags|=1;
 }else
  loco->LocoFlags|=1;
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
};


/**
 * Реакция на переключение элементов
 */
extern "C" void Q_DECL_EXPORT Switched(const ElectricLocomotive *loco,ElectricEngine *eng,
        unsigned int SwitchID,unsigned int PrevState)
{

 Cabin *cab=loco->Cab();
 /*
  * UINT &Flags=*(UINT *)&eng->var[0];
  * UINT i,id;
 int rev;
 UINT BVOn=0,Panto=0;
 UINT LocoOn=IsLocoOn(loco,Flags);
 UINT ConvOn=0,SetPos=0;
 bool ThrottleReset=false,NoS=false;
 FreeAnimation *anim;*/


 switch(SwitchID){
  case Tumblers::Tbm_Panto:
  case Tumblers::Tmb_Panto1_3:
  case Tumblers::Tmb_Panto2_4:
  if  (cab->Switch(Tumblers::Tbm_Panto))
  {
      if  (cab->Switch(Tumblers::Tmb_Panto1_3))
        SELF.Panto |= 1UL << 0;
      else
        SELF.Panto &=~(1UL << 0);

      if  (cab->Switch(Tumblers::Tmb_Panto2_4))
        SELF.Panto |= 1UL << 1;
      else
        SELF.Panto &=~(1UL << 1);
  }
  else
      SELF.Panto = 0x0;
  break;
 }

 VL15.checkSwitch(loco, Buttons::Btn_Tifon);
 VL15.checkSwitch(loco, Buttons::Btn_Svistok);
 VL15.checkSwitch(loco, Arms::Arm_Reverse);

 if (SwitchID == Keys::Key_EPK)
    VL15.checkSwitch(loco, Keys::Key_EPK_ON);

 if (SwitchID == Tumblers::Tmb_vozvrBV1 )
 {
     if (VL15.checkSwitch(loco, Tumblers::Tmb_BV1))
     {
        SwitchBV(loco, eng, 1 );
        wchar_t text[1024];
        swprintf(text, L"BV Enabled\n");
        eng->ShowMessage(GMM_POST, text);
     }
 }

 else if  (SwitchID == Tumblers::Tmb_BV1 )
 {
     if  (VL15.checkSwitch(loco,Tumblers::Tmb_BV1 ) < 1 )
     {
           SwitchBV(loco, eng, 0 );
           wchar_t text[1024];
           swprintf(text, L"BV Disabled\n");
           eng->ShowMessage(GMM_POST, text);
     }
 }

}



extern "C" void Q_DECL_EXPORT SpeedLimit(const Locomotive *loco,
        SpeedLimitDescr Route,SpeedLimitDescr Signal,SpeedLimitDescr Event)
{
 if(!(loco->LocoFlags&1))
  return;

 Engine *eng=loco->Eng();
 //unsigned long &Flags=*(unsigned long *)&eng->var[0];

 float Limit=GetLimit(Route.Limit,Signal.Limit,Event.Limit);
 //float Vel=fabs(loco->Velocity);
 eng->var[14]=Limit*3.6;
 if(Limit<6.945)
 {
  Limit=6.945;
  eng->var[14]=25.0;
 }

}

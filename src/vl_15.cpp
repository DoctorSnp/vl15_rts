
#include <windows.h>
#include <math.h>
#include <stdio.h>

#include <cmath>

/*#define RTS_NODIRECTOBJLINKS*/
#include "src/utils/utils.h"
#include "src/private_vl_15.h"
#include "src/vl15_datatypes/cab/section1/elements.h"
#include "../VL_15.h"

#include "vl15_logic.h"


#pragma hdrstop
#pragma argsused

#define TR_CURRENT_C 272.0

VL15_logic VL15;

wchar_t E_99[] = {L"E99"};
wchar_t WIPE_LEFT[] ={L"wiperleft"};
wchar_t WIPE_RIGHT[] ={L"wiperright"};
static struct
{
 int Panto;
 float power;
 int dest; //1 , -1
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
 //UINT *AsyncFlags=(UINT *)&eng->var[4];
 //Cabin *cab=loco->Cab();
 loco->HandbrakeValue=0.0;
 eng->HandbrakePercent=0.0;
 eng->DynamicBrakePercent=0;
 eng->Sanding=0;
 eng->BrakeSystemsEngaged=0; //1
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
 //loco->MainResPressure=5.2;

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
  // eng->Panto=3;
  // eng->MainSwitch=1;
   eng->IndependentBrakeValue=0.0;
   //*AsyncFlags|=588;

  break;
 };

 SELF.dest = -1;
 return VL15.init();

};


extern "C" void Q_DECL_EXPORT Run
 (ElectricEngine *eng,const ElectricLocomotive *loco,unsigned long State,
        float time,float AirTemperature)
{
 eng->MainResRate=0.0;
 eng->Panto =  SELF.Panto;
 Cabin *cab=loco->Cab();
 //SwitchBV(loco, eng, 1 );

 //cab->SetDisplayValue(Snsr_Yakor, 10.6);
 static unsigned int ThrottlePosition = 1;
static int Reverse = 1;

 eng->Reverse = cab->Switch(Arm_Reverse);

 if (eng->Reverse >= 1)
 {
   float Velocity = loco->Velocity;

   float SetForce = 0.0;
   eng->ThrottlePosition = cab->Switch(Arm_Zadatchik);
   float LineVoltage =  loco->LineVoltage;
   if (ThrottlePosition < 1)
    ThrottlePosition = 1;
   if(Velocity<=3.01)
       SetForce= 400000.0 * (ThrottlePosition-1);
   else
   {
       SetForce = (340000.0* ThrottlePosition-1);
       /*SetForce = (340000.0*eng->ThrottlePosition)/
                   (Velocity*Velocity*std::pow(0.978,eng->ThrottlePosition)) +
                   shunt*468750.0/Velocity;*/
   }


   if (ThrottlePosition != eng->ThrottlePosition )
   {
       ThrottlePosition = eng->ThrottlePosition;
       wchar_t text[1024]; \
       swprintf(text, L"Force %f Throttle: %d Reverse: %d\n", SetForce, ThrottlePosition,  eng->Reverse);
       eng->ShowMessage(GMM_POST, text);

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
   eng->Force *= SELF.dest;
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
  //case Arms::Arm_Reverse:
    //eng->Reverse = cab->Switch(Arms::Arm_Reverse);
  //break;
 default:
     break;
 }

 VL15.checkSwitch(loco, Buttons::Btn_Tifon);
 VL15.checkSwitch(loco, Buttons::Btn_Svistok);
 //VL15.checkSwitch(loco, Arms::Arm_Reverse);

// if (SwitchID == Keys::Key_EPK)
//    VL15.checkSwitch(loco, Keys::Key_EPK_ON);

 if (SwitchID == Tumblers::Tmb_vozvrBV1 )
 {
     if (VL15.checkSwitch(loco, Tumblers::Tmb_BV1))
     {
        //SwitchBV(loco, eng, 1 );
        //wchar_t text[1024];
        //swprintf(text, L"BV Enabled\n");
        //eng->ShowMessage(GMM_POST, text);
     }
 }
 else if  (SwitchID == Tumblers::Tmb_BV1 )
 {
     if  (VL15.checkSwitch(loco,Tumblers::Tmb_BV1 ) < 1 )
     {
           //SwitchBV(loco, eng, 0 );
          // wchar_t text[1024];
          // swprintf(text, L"BV Disabled\n");
          // eng->ShowMessage(GMM_POST, text);
     }
 }

}



extern "C" void Q_DECL_EXPORT SpeedLimit(const Locomotive *loco,
        SpeedLimitDescr Route,SpeedLimitDescr Signal,SpeedLimitDescr Event)
{
 if(!(loco->LocoFlags&1))
  return;

 return;
}

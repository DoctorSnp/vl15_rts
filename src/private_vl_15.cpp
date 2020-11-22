
#include <math.h>
#include <stdio.h>

#include "src/vl15_datatypes/cab/section1/elements.h"
#include "private_vl_15.h"


extern "C" void Q_DECL_EXPORT LostMaster
        (Locomotive *loco,const Locomotive *Prev,unsigned long State);

extern "C" void Switched(const ElectricLocomotive *loco,ElectricEngine *eng,
        unsigned int SwitchID,unsigned int PrevState);


void m_checkSwitch(const ElectricLocomotive* loco, unsigned short switchElem) noexcept
{
    if (loco->Cab()->Switch(switchElem) > 0)
     loco->PostTriggerCab(switchElem); // включаем звук
    else
     loco->PostTriggerCab(switchElem+1); // сбрасываем звук
}

UINT IsLocoOn(const ElectricLocomotive *loco, ULONG Flags)
{
 UINT res=1;
 //KU
 if(loco->Cab()->Switch(132))
  res|=2;
 //convertor
 if(Flags&256)
  res|=4;
 if(loco->Cab()->Switch(131)&&loco->Cab()->Switch(118))
  res|=8;
 return res;
};

void SwitchBV(const Locomotive *loco,ElectricEngine *eng,bool BVOn)

{
 if((eng->MainSwitch!=0)!=BVOn){
  eng->MainSwitch=BVOn?1:0;
  loco->PostTriggerBoth(Tmb_BV1);
 };
};

void SwitchLights(const Locomotive *loco,Engine *eng,UINT State)
{
 Cabin *cab=loco->Cab();
 if(loco->LibParam==0){
  switch(State){
   case 0:
    loco->SwitchLight(0,false,0.0,0);
    loco->SwitchLight(1,false,0.0,0);
    loco->SwitchLight(6,false,0.0,0);
    loco->SwitchLight(7,false,0.0,0);
   break;
   case 1:
    loco->SwitchLight(0,true,0.0,0);
    loco->SwitchLight(1,false,0.0,0);
    loco->SwitchLight(6,true,0.0,0);
    loco->SwitchLight(7,false,0.0,0);
   break;
   case 2:
    loco->SwitchLight(0,false,0.0,0);
    loco->SwitchLight(1,true,0.0,0);
    loco->SwitchLight(6,false,0.0,0);
    loco->SwitchLight(7,true,0.0,0);
   break;
   case 3:
    loco->SwitchLight(2,false,0.0,0);
    loco->SwitchLight(14,false,0.0,0);
   break;
   case 4:
    loco->SwitchLight(2,false,0.0,0);
    loco->SwitchLight(14,true,0.0,0);
   break;
   case 5:
    loco->SwitchLight(2,true,0.0,0);
    loco->SwitchLight(14,false,0.0,0);
   break;
   case 6:
    loco->SwitchLight(3,false,0.0,0);
    loco->SwitchLight(15,false,0.0,0);
   break;
   case 7:
    loco->SwitchLight(3,false,0.0,0);
    loco->SwitchLight(15,true,0.0,0);
   break;
   case 8:
    loco->SwitchLight(3,true,0.0,0);
    loco->SwitchLight(15,false,0.0,0);
   break;
  };
 }else if(loco->LibParam==1){
  switch(State){
   case 0:
    loco->SwitchLight(2,false,0.0,0);
    loco->SwitchLight(3,false,0.0,0);
    loco->SwitchLight(4,false,0.0,0);
    loco->SwitchLight(5,false,0.0,0);
   break;
   case 1:
    loco->SwitchLight(2,true,0.0,0);
    loco->SwitchLight(3,false,0.0,0);
    loco->SwitchLight(4,true,0.0,0);
    loco->SwitchLight(5,false,0.0,0);
   break;
   case 2:
    loco->SwitchLight(2,false,0.0,0);
    loco->SwitchLight(3,true,0.0,0);
    loco->SwitchLight(4,false,0.0,0);
    loco->SwitchLight(5,true,0.0,0);
   break;
   case 3:
    loco->SwitchLight(6,false,0.0,0);
    loco->SwitchLight(8,false,0.0,0);
   break;
   case 4:
    loco->SwitchLight(6,false,0.0,0);
    loco->SwitchLight(8,true,0.0,0);
   break;
   case 5:
    loco->SwitchLight(6,true,0.0,0);
    loco->SwitchLight(8,false,0.0,0);
   break;
   case 6:
    loco->SwitchLight(7,false,0.0,0);
    loco->SwitchLight(9,false,0.0,0);
   break;
   case 7:
    loco->SwitchLight(7,false,0.0,0);
    loco->SwitchLight(9,true,0.0,0);
   break;
   case 8:
    loco->SwitchLight(7,true,0.0,0);
    loco->SwitchLight(9,false,0.0,0);
   break;
  };
 };
 switch(State){
  case 9:
   cab->SetLightState(0,0,0,-1.0);
   cab->SetLightState(1,0,0,-1.0);
  break;
  case 10:
   cab->SetLightState(0,1,0x90bbaa40,0.8);
   cab->SetLightState(1,1,0x90bbaa40,0.8);
  break;
  case 11:
   cab->SetLightState(0,1,0xaaddddcc,0.5);
   cab->SetLightState(1,1,0xaaddddcc,0.5);
  break;
  case 12:
   cab->ChangeTexture(0,0);
   cab->ChangeTexture(1,0);
   cab->ChangeTexture(2,0);
   cab->ChangeTexture(3,0);
  break;
  case 13:
   cab->ChangeTexture(0,1);
   cab->ChangeTexture(1,1);
   cab->ChangeTexture(2,1);
   cab->ChangeTexture(3,1);
  break;
 };
};



void AsyncSwitch(const ElectricLocomotive *loco,UINT UnitID,UINT Command,bool On)

{
 const ElectricLocomotive *unit=NULL;
 ElectricEngine *eng=(ElectricEngine *)loco->Eng();
 switch(UnitID){
  case 3:
   if(loco->NumSlaves==1)
    unit=(ElectricLocomotive *)loco->SlaveLoco(0);
   else if(loco->NumSlaves==2)
    unit=(ElectricLocomotive *)loco->SlaveLoco(1);
   else if(loco->NumSlaves>=3)
    unit=(ElectricLocomotive *)loco->SlaveLoco(2);
  break;
  case 2:
   if(loco->NumSlaves==2)
    unit=(ElectricLocomotive *)loco->SlaveLoco(0);
   else if(loco->NumSlaves>=3)
    unit=(ElectricLocomotive *)loco->SlaveLoco(1);
  break;
  case 1:
   if(loco->NumSlaves>=3)
    unit=(ElectricLocomotive *)loco->SlaveLoco(0);
  break;
  case 0:
   unit=loco;
  break;
 };
 if(unit){
  UINT &AsyncFlags=*(UINT *)&unit->Eng()->var[4];
  switch(Command){
   case 0:
    //Control SU
    if(On){
     LostMaster((ElectricLocomotive *)unit,NULL,0);
    }else{
     Switched(loco,eng,10,0);
     Switched(loco,eng,11,0);
     Switched(loco,eng,12,0);
     Switched(loco,eng,24,0);
     Switched(loco,eng,25,0);
     Switched(loco,eng,27,0);
     Switched(loco,eng,28,0);
     Switched(loco,eng,30,0);
     Switched(loco,eng,31,0);
     Switched(loco,eng,34,0);
     Switched(loco,eng,36,0);
    };
   break;
   case 1:
    //Sanding
    if(On)
     AsyncFlags|=1024;
    else
     AsyncFlags&=~1024;
   break;
   case 2:
    //Panto
    if(On)
     AsyncFlags|=48;
    else{
     Switched(loco,eng,24,0);
     Switched(loco,eng,25,0);
    };
   break;
   case 3:
    //BV
    if(On)
     AsyncFlags|=2;
   break;
   case 4:
    //turn EDT off
    if(On)
     unit->Eng()->var[2]=17.0;
   break;
   case 5:
    //BV off
    if(On)
     AsyncFlags|=1;
   break;
   case 6:
    //Heaters 1

   break;
   case 7:
    //TED1-2 off
    if(On)
     AsyncFlags|=64<<8;
    else
     AsyncFlags&=~(64<<8);
   break;
   case 8:
    //HCH lights

   break;
   case 9:
    //TED2-3 off
    if(On)
     AsyncFlags|=128<<8;
    else
     AsyncFlags&=~(128<<8);
   break;
   case 10:
    //valve heaters

   break;
   case 11:
    //compressor off
    if(!On){
     Switched(loco,eng,30,0);
    }else
     AsyncFlags&=~512;
   break;
   case 12:
    //auxiliary machines off

   break;
   case 13:
    //vents off
    if(On)
     AsyncFlags&=~(48<<8);
    else
     Switched(loco,eng,28,0);
   break;
   case 14:
    //control TS
    if(On){
     //unit->locoA->var[2]=1.0;
    }else{
     Switched(loco,eng,36,0);
    };
   break;
  };
 };
};


bool ThreeSectionOK(const ElectricLocomotive *loco,ElectricEngine *eng)
{
 Cabin *cab=loco->Cab();
 if(cab->Switch(34))
  return false;
 if(cab->Switch(55)||cab->Switch(111))
  return false;
 if(loco->NumSlaves>2){
  if(!cab->Switch(58)&&!cab->Switch(114))
   return false;
  if(cab->Switch(56)||cab->Switch(112))
   return false;
  if(cab->Switch(57)||cab->Switch(113))
   return false;
 }else if(loco->NumSlaves!=2){
  return false;
 }else{
  if(cab->Switch(57)||cab->Switch(113))
   return false;
  if(cab->Switch(58)||cab->Switch(114))
   return false;
 };

 return true;
};

UINT ApproachRed(Engine *eng,float SigDist,float Vel)
{
 UINT res=0;
     if(SigDist<500.0){
      if(Vel>5.55)
       res=1;
      if(SigDist<300.0){
       if(Vel>5.55)
        res=2;
       if(SigDist<250.0){
        if(Vel>4.0 && res<1)
         res=1;
        if(SigDist<150.0){
         if(Vel>4.0)
          res=2;
         if(SigDist<100.0){
          if(Vel>2.7 && res<1)
           res=1;
          if(SigDist<100.0){
           if(Vel>2.7)
            res=2;
           else if(Vel>1.4 && res<1)
            res=1;
           if(SigDist<20.0)
            if(Vel>1.4)
             res=2;
          };
         };
        };
       };
      };
     };
 return res;
};


bool SwitchPosition(ElectricEngine *eng,bool Fast,bool NoS,UINT SetPos,float Current)
{
 if(SetPos>eng->ThrottlePosition){
  if(Current<eng->var[5]){
   if(Fast){
    eng->ThrottlePosition+=3;
    if(eng->ThrottlePosition>SetPos)
     eng->ThrottlePosition=SetPos;
   }else
    eng->ThrottlePosition++;
   if(NoS){
    if(eng->ThrottlePosition<=16)
     eng->ThrottlePosition=17;
   };
   return true;
  };
 }else if(SetPos<eng->ThrottlePosition){
  if(Fast){
   if(eng->ThrottlePosition-SetPos>3)
    eng->ThrottlePosition-=3;
   else
    eng->ThrottlePosition--;
  }else
   eng->ThrottlePosition--;
  if(NoS){
   if(eng->ThrottlePosition<=16)
    eng->ThrottlePosition=0;
  };
  return true;
 };
 return false;
};

void WriteThrottlePosition(wchar_t *buf,Engine *eng)

{
 buf[0]=0;
 int Pos=eng->ThrottlePosition;
 if(Pos>0){
  if(Pos>=1 && Pos<=16){
   swprintf(buf,L"С %02i",Pos);
  }else if(Pos>=17 && Pos<=27){
   Pos-=16;
   swprintf(buf,L"СП %02i",Pos);
  }else if(Pos>=28 && Pos<=36){
   Pos-=27;
   swprintf(buf,L"П1 %02i",Pos);
  }else if(Pos>=37 && Pos<=45){
   Pos-=36;
   swprintf(buf,L"П2 %02i",Pos);
  };
  Pos=eng->ThrottlePosition;
  if(Pos==16 || Pos==27 ||Pos==36 || Pos==45 ){
   switch(int(eng->var[7])){
    case 1: lstrcatW(buf,L"\nОП1"); break;
    case 2: lstrcatW(buf,L"\nОП2"); break;
    case 3: lstrcatW(buf,L"\nОП3"); break;
    case 4: lstrcatW(buf,L"\nОП4"); break;
    default: lstrcatW(buf,L"\nХП"); break;
   };
  };
 }else if(eng->var[11]>0.0){
  Pos=eng->var[11];
  if(Pos>=1 && Pos<=15){
   swprintf(buf,L"Т2 %02i",Pos);
  }else if(Pos>=16 && Pos<=30){
   Pos-=15;
   swprintf(buf,L"Т1 %02i",Pos);
  };
 };
};

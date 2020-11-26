#include <sys/timeb.h>
#include "utils/utils.h"
#include "saut.h"
#include "math.h"
#include "vl15.h"


/*Закрытые функции */
static void SL2m_step(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self);
static void HodovayaSound(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self);

static int _getDest(st_Self *self);

static en_Colors _getForwardSignColor(ElectricEngine *eng);

static float _getForce ( st_Self *self);
static void _debugPrint(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self);


static SAUT saut;

bool VL15_init(st_Self *self)
{
    ftime(&self->debugTime.prevTime);
    self->debugTime.currTime = self->debugTime.prevTime;

    memset(self, 0, sizeof (struct st_Self));
    self->SL2M_Ticked = false;
    self->Reverse = NEUTRAL;

    VL15_set_destination(self, SECTION_DEST_BACKWARD);
    saut.init();
    Radiostation_Init(&self->radio);
    Radiostation_setEnabled(&self->radio, 1);

    self->sautData.SpeedLimit.Limit = -1.0;
    self->sautData.SpeedLimit.Distance = -1.0;
    return true;
}

void VL15_set_destination(st_Self *self, int SectionDest)
{
    self->secdionCabDest = SectionDest;
}

int VL15_Step(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self)
{
    /*Грузим данные из движка себе в МОЗГИ*/
    self->Velocity = fabs(loco->Velocity) * 2.5;
    static int prevEpk = self->EPK;

    self->sautData.PrevSpeed = self->prevVelocity;
    self->sautData.CurrSpeed = self->Velocity;

    self->sautData.forwColor = _getForwardSignColor(eng);
    self->elecrto.LineVoltage =  loco->LineVoltage;

    /*корректируем показатели */
    if (self->pneumo.Arm_254 < 1)
        self->pneumo.Arm_254 = 1;

    if (self->pneumo.Arm_394 < 1)
        self->pneumo.Arm_394 = 1;


    /*Работаем в своём соку*/
    Radiostation_Step(loco, eng, &self->radio);
    SL2m_step(loco, eng, self);
    HodovayaSound(loco, eng, self);

    if ( prevEpk != self->EPK)
    {
        if (prevEpk == 0)
            saut.start(loco, eng, self->sautData);
        else
            saut.stop();
        prevEpk = self->EPK;
    }
    else
        saut.step(loco, eng, self->sautData);

    /*А тепер пихаем из наших МОЗГОВ данные в движок*/
    eng->Panto = ((unsigned char)self->elecrto.PantoRaised);// (PantoRaised);
    eng->ThrottlePosition = self->ThrottlePosition;
    eng->Reverse = self->Reverse;
    eng->IndependentBrakeValue= ( self->pneumo.Arm_254 - 1) * 1.0;
    self->prevVelocity = self->Velocity;


    //float Voltage = self->elecrto.LineVoltage;
    float SetForce = _getForce (self);

    if(self->Velocity <= 3.01)
        SetForce *= 40000.0;
    else
    {
        SetForce *= 68000.0;
        /*SetForce = (680000.0*eng->ThrottlePosition)/
                      (SELF.Velocity*SELF.Velocity*std::pow(0.978,eng->ThrottlePosition)) +
                      SELF.shuntNum*468750.0/SELF.Velocity;*/
    }

    eng->Force = SetForce;
     /*
      if(eng->Force<SetForce)
      {
          eng->Force+=(10000.0+(SetForce-eng->Force)*0.6);
          if(eng->Force>SetForce)
              eng->Force=SetForce;
      }else if(eng->Force>SetForce)
      {
          //eng->Force-=3000.0*time;
          if(eng->Force<SetForce)
              eng->Force=SetForce;
      }*/

     _debugPrint(loco, eng, self);
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
    if (self->prevVelocity >  fabs(self->Velocity)  ) // типа, быстро так тормозим
    {
        if ((self->Velocity < 3.0) && (isFinalStop == 0) )
        {
            isFinalStop = 1;
            loco->PostTriggerCab(SoundsID::FinalStop );
            return;
        }
    }

    if (fabs(self->Velocity) <= 0.00)
    {
        if (isFinalStop )
        {
            isFinalStop = 0;
        }
    }
}

static int _getDest( st_Self *self)
{
    if (self->Reverse > NEUTRAL)
        return SECTION_DEST_FORWARD * self->secdionCabDest;
    else if (self->Reverse < NEUTRAL )
        return SECTION_DEST_BACKWARD * self->secdionCabDest;
    else
        return 0;
}

static float _getForce ( st_Self *self)
{
    float startForce = 0.0;
    if (self->BV_STATE == 0)
        return 0.0;
    if (self->elecrto.PantoRaised == 0)
        return 0.0;
    startForce = (float) (( self->Reverse - NEUTRAL) * self->ThrottlePosition )  ;
    startForce *= float(_getDest(self));
    return startForce;
}

static void _debugPrint(const ElectricLocomotive *loco, ElectricEngine *eng, st_Self *self)
{
    ftime(&self->debugTime.currTime);
    if ((self->debugTime.prevTime.time + 1) > self->debugTime.currTime.time)
        return;

    unsigned int Aspect = eng->cab->Signal.Aspect[0];
    float DistanceToSig=eng->cab->Signal.Distance;
    Printer_print(eng, GMM_POST, L"SomeVar %f Aspect: %u, Dist to sig: %f"
                  "Speed: %f Force: %f\n",
                 eng->var[6], Aspect, DistanceToSig, self->Velocity,
            eng->Force);
    self->debugTime.prevTime = self->debugTime.currTime;

}

/**
 * @brief _getForwardSignColor Получить цвет сигнала впереди
 * @param eng
 * @return
 */
static en_Colors _getForwardSignColor(ElectricEngine *eng)
{
    unsigned int Aspect = eng->cab->Signal.Aspect[0];
    if (Aspect == 7)
        return en_Colors::COLOR_GREEN;
    else if (Aspect == 6)
        return en_Colors::COLOR_YELLOW;
    else if (Aspect == 5)
        return en_Colors::COLOR_RD_YEL;
    else if (Aspect == 4)
        return en_Colors::COLOR_RED;
    else if (Aspect == 2)
        return en_Colors::COLOR_WHITE;
    else
        return en_Colors::UNSET;
}

#ifndef PRIVATE_VL_15_H
#define PRIVATE_VL_15_H

#include <windows.h>
#include "ts.h"


#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
#else
#  define Q_DECL_EXPORT     __attribute__((visibility("default")))
#  define Q_DECL_IMPORT     __attribute__((visibility("default")))
#endif

#if defined(VL15_LIBRARY)
#  define VL15_EXPORT Q_DECL_EXPORT
#else
#  define VL15_EXPORT Q_DECL_IMPORT
#endif

/* Q_DECL_EXPORT не было в оригинале */

/*extern "C" void Q_DECL_EXPORT LostMaster
        (Locomotive *loco,const Locomotive *Prev,unsigned long State);*/


#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/**
 * @brief m_checkSwitch Запускает озвучку или сбрасывает её в зависимости от положения элемента.
 * @param loco Указатель на локомотив.
 * @param switchElem ID элемента, который нужно проверить.
 */
void m_checkSwitch(const ElectricLocomotive* loco, unsigned short switchElem) noexcept;

UINT IsLocoOn(const ElectricLocomotive *loco, ULONG Flags);
void SwitchBV(const Locomotive *loco,ElectricEngine *eng,bool BVOn);
void SwitchLights(const Locomotive *loco,Engine *eng,UINT State);
void AsyncSwitch(const ElectricLocomotive *loco,UINT UnitID,UINT Command,bool On);
bool ThreeSectionOK(const ElectricLocomotive *loco,ElectricEngine *eng);
bool ThreeSectionOK(const ElectricLocomotive *loco,ElectricEngine *eng);
UINT ApproachRed(Engine *eng,float SigDist,float Vel);
bool SwitchPosition(ElectricEngine *eng,bool Fast,bool NoS,UINT SetPos,float Current);
void WriteThrottlePosition(wchar_t *buf,Engine *eng);

#endif // PRIVATE_VL_15_H

﻿#ifndef PRIVATE_VL_15_H
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

void _playSound(const ElectricLocomotive *loco, int soundId, int where = 0 );

int _checkSwitch(const ElectricLocomotive *loco, unsigned int switchElem);

/**
 * @brief _checkSwitchWithSound Возвращает состояние элемента и проигрывает его звук
 * @param loco
 * @param switchElem
 * @param soundId
 * @return
 */
int _checkSwitchWithSound(const ElectricLocomotive *loco, unsigned int switchElem, int soundId, int singleSound, int where =0);

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#endif

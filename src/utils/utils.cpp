
//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <errno.h>


#include "src/ts.h"
#include "utils.h"

#if 0
void  Printer_print( ElectricEngine *eng, int dbgLevel, const wchar_t *format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    wchar_t text[1024];
    vswprintf_s(text, sizeof (text), format, args);
    va_end(args);

    eng->ShowMessage(dbgLevel, text);
}
#endif

//#include "stdafx.h"
#include "pch.h"
#include <Windows.h>
#include "strsafe.h"
#include "log.h"

void myLog(LOG_LEVEL level, const char *pFormat, ...)
{
#ifndef _DEBUG
    //return;
#endif

    if (level > DEBUG_LEVEL)
        return;

    va_list args;
    int nLen = 0;
    char *pszBuf = NULL;

    va_start(args, pFormat);

    nLen = _vscprintf(pFormat, args) + 1;

    if (char *pszBuf = new char[nLen])
    {
        vsprintf_s(pszBuf, nLen, pFormat, args);

        ::OutputDebugStringA(pszBuf);

        delete[] pszBuf;
    }

    va_end(args);
}

void myLogW(LOG_LEVEL level, const wchar_t *pFormat, ...)
{
#ifndef _DEBUG
    //return;
#endif

    if (level > DEBUG_LEVEL)
        return;

    va_list args;
    int nLen = 0;
    wchar_t *pszBuf = NULL;

    va_start(args, pFormat);

    nLen = _vscwprintf(pFormat, args) + 1;

    if (wchar_t *pszBuf = new wchar_t[nLen])
    {
        vswprintf_s(pszBuf, nLen, pFormat, args);

        ::OutputDebugString(pszBuf);

        delete[] pszBuf;
    }

    va_end(args);
}

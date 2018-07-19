#pragma once
#include "pch.h"
#include <locale>
#include <codecvt>

typedef enum _LOG_LEVEL
{
    LOG_ERROR = 0,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE,

    LOG_MAXVALUE

} LOG_LEVEL;

//#define DEBUG_LEVEL LOG_MAXVALUE
#define DEBUG_LEVEL LOG_INFO

#define TAG "[SlitherLink] "
#define LTAG L"[SlitherLink] "

#if __cplusplus < 201703L
#define __funcw__ (std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(__func__).c_str())
#else
// wstring_convert deprecated in c++17
inline std::wstring strToWstr(const std::string& str)
{
    if (str.empty())
        return std::wstring(L"");
    std::wstring ret(L"", MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0));
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], (int)ret.length());
    return ret;
}

#define __funcw__ (strToWstr(std::string(__func__)).c_str())
#endif

void myLog(LOG_LEVEL level, const char *pFormat, ...);
void myLogW(LOG_LEVEL level, const wchar_t *pFormat, ...);

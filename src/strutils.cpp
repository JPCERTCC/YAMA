#include "strutils.h"

namespace yama {

const char* WideCharToUtf8(wchar_t* lpwcStr){
    int cSize = WideCharToMultiByte(CP_UTF8, 0, lpwcStr, -1, (char *)NULL, 0, NULL, NULL);
    char * lpUtf8Str = new char[cSize]();

    WideCharToMultiByte(CP_UTF8, 0, lpwcStr, -1,lpUtf8Str , cSize, NULL, NULL);
    return reinterpret_cast<const char*>(lpUtf8Str);
}

const wchar_t *StdStringToWideChar(std::string str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);
    wchar_t *wc = new wchar_t[len + 1]();
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), wc, len);
    return wc;
}


} // namespace yama 
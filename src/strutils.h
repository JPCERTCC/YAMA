#ifndef SRC_STRUTILS_H
#define SRC_STRUTILS_H

#include <Windows.h>
#include <string>

namespace yama {

const char* WideCharToUtf8(wchar_t*);
const wchar_t *StdStringToWideChar(std::string str);

}  // namespace yama

#endif  // SCR_STRUTILS_H
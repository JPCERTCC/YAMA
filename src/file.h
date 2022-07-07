#ifndef _FILE_H
#define _FILE_H

#include "common.h"

namespace yama {

wchar_t* ConvertDevicePath(_In_ wchar_t* wcInPath);
void WriteDumpFile(_In_ void* lpInBuffer, _In_ uint32_t dwSize, _In_ uint64_t StartVa);
bool PathExistsW(_In_ LPCWCHAR lpcwPath);

}  // namespace yama

#endif  // _FILE_H
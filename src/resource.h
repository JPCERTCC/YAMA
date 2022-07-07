#ifndef SRC_RESOURCE_H
#define SRC_RESOURCE_H

#include "common.h"
#include "rc4.hpp"

namespace yama {

DWORD GetFileResource(_In_ HMODULE hModule, _In_ LPCWSTR lpcwResourceName, _Out_ LPVOID* lpBuffer);
DWORD ExportFileResource(_In_ HMODULE hModule, _In_ LPCWSTR lpcwResourceName,
                         _In_ LPWSTR lpwFilePath);
const char* LoadYaraRuleFromResource();

}  // namespace yama
#endif  // SRC_RESOURCE_H
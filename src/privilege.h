#ifndef SRC_PRIVILEGE_H
#define SRC_PRIVILEGE_H

#include "common.h"

// should include after common.h
#include <winternl.h>

namespace yama {
bool SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, bool bEnablePrivilege);
bool IsElevated();
BOOL SetSeDebugPrivilege();

}  // namespace yama
#endif  // SRC_PRIVILEGE_H

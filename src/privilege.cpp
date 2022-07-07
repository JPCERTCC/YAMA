#include "privilege.h"

namespace yama {
bool SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, bool bEnablePrivilege) {
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!LookupPrivilegeValueW(nullptr, lpszPrivilege, &luid)) {
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;

    // Enable the privilege or disable all privileges.
    if (!AdjustTokenPrivileges(hToken, false, &tp, sizeof(TOKEN_PRIVILEGES),
                               (PTOKEN_PRIVILEGES) nullptr, (PDWORD) nullptr)) {
        return false;
    }
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        return false;
    }
    return true;
}

bool IsElevated() {
    bool res = false;
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            res = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return res;
}

BOOL SetSeDebugPrivilege() {
    HANDLE currentTokenHandle = nullptr;
    BOOL bStatus =
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &currentTokenHandle);
    if (!bStatus) {
        // return GetLastError();
        return bStatus;
    }
    bStatus = SetPrivilege(currentTokenHandle, SE_DEBUG_NAME, true);
    return bStatus;
}

}  // namespace yama

#include "pid.h"

namespace yama {
std::vector<DWORD> ListPid() {
    std::vector<DWORD> vPid;

    PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    BOOL result;

    for (result = Process32FirstW(snapshot, &pe); result == TRUE;
         result = Process32NextW(snapshot, &pe)) {
        vPid.push_back(pe.th32ProcessID);
    }
    return vPid;
}
}  // namespace yama

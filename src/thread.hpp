#ifndef _THREAD_HPP
#define _THREAD_HPP

#include "common.h"
#include "typedef.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

namespace yama {
typedef NTSTATUS(NTAPI* NtQueryInformationThread_t)(HANDLE, nt::THREADINFOCLASS, void*, uint32_t,
                                                    uint32_t*);

class Thread {
   public:
    DWORD dwThreadId = 0;
    DWORD dwPid = 0;
    nt64::TEB* pTebAddress = nullptr;
    uint64_t pEntryPoint = 0;
    uint64_t pStackBase = 0;
    Thread(DWORD dwThreadId, HANDLE hProcess);
    // ~Thread();

   private:
    HANDLE hThread = INVALID_HANDLE_VALUE;
};

void thread_test();

}  // namespace yama
#endif  // _THREAD_HPP
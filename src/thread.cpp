#include "thread.hpp"

namespace yama {
static NtQueryInformationThread_t NtQueryInformationThread = nullptr;

Thread::Thread(DWORD dwThreadId, HANDLE hProcess) {
    BOOL bRes = FALSE;
    NTSTATUS NtState;

    /*
    Get NtQueryInformationThread function pointer.
    */
    if (NtQueryInformationThread == nullptr) {
        NtQueryInformationThread = reinterpret_cast<NtQueryInformationThread_t>(
            GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationThread"));

        if (NtQueryInformationThread == nullptr) {
            LOGERROR("Failed to get NtQueryInformationThread function pointer.");
        }
    }

    this->dwThreadId = dwThreadId;
    this->hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT, false, dwThreadId);
    if (this->hThread == INVALID_HANDLE_VALUE) {
        LOGWARN("Failed to open thread handle. Thread ID: {0:d}", dwThreadId);
        return;
    }

    /*
    Duplicate thread handle.
    */
    HANDLE hDupThread = INVALID_HANDLE_VALUE;
    bRes = DuplicateHandle(GetCurrentProcess(), this->hThread, GetCurrentProcess(), &hDupThread,
                           THREAD_QUERY_INFORMATION, FALSE, NULL);
    if (!bRes) {
        LOGWARN("Failed to duplicate thread handle. Thread ID: {0:d}", dwThreadId);
        return;
    }

    /*
    Set EntryPoint Address
    */
    NtState =
        NtQueryInformationThread(hDupThread, nt::THREADINFOCLASS::ThreadQuerySetWin32StartAddress,
                                 &this->pEntryPoint, sizeof(this->pEntryPoint), NULL);
    // printf("EntryPoint: %#.16I64x\n", (uint64_t)this->pEntryPoint);
    if (!NT_SUCCESS(NtState)) {
        LOGWARN("Failed to aqcuire entry point address.");
    }

    /*
    Set StackBase Address
    */
    nt64::THREAD_BASIC_INFORMATION* pThreadBasicInformation = new nt64::THREAD_BASIC_INFORMATION();
    NtState = NtQueryInformationThread(hThread, nt::THREADINFOCLASS::ThreadBasicInformation,
                                       pThreadBasicInformation,
                                       sizeof(nt64::THREAD_BASIC_INFORMATION), NULL);
    if (!NT_SUCCESS(NtState)) {
        LOGWARN("Failed to query ThreadBasicInformation. Thread ID: {0:d}\n", dwThreadId);
        return;
    }
    // TBU: support wow64 process
    this->pTebAddress = new nt64::TEB();
    bRes = ReadProcessMemory(hProcess, pThreadBasicInformation->TebBaseAddress, this->pTebAddress,
                             sizeof(nt64::TEB), NULL);
    if (bRes) {
        this->pStackBase = this->pTebAddress->NtTib.StackBase;
    }

    /*
    clean up
    */
    CloseHandle(hDupThread);
    return;
}

}  // namespace yama
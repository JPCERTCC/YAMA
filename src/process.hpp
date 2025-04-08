#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include "common.h"

// should include after "typedef.h"
#include <winternl.h>

#include "memory.hpp"
#include "thread.hpp"
#include "typedef.h"

namespace yama {
class LdrEntry {
   public:
    LdrEntry(HANDLE hProcess, void* lpDataTableEntry);
    uint64_t lpBaseAddress = 0;
    wchar_t* wcDllFullPath = nullptr;
    const char* cLoadReason = nullptr;
    uint8_t SigningLevel = 0;
    DWORD dwImageSize = 0;

   private:
    const char* GetLoadReasonName(uint16_t LoadReason);
};

class Peb {
   public:
    Peb(HANDLE hProcess);
    wchar_t* GetCmdLine() { return this->wcCmdLine; };
    void SetProcessParam();
    std::map<uint64_t, LdrEntry*> mapLoadedModules;
    std::vector<uint64_t> GetHeapAddressList();
    nt64::PEB* GetPEB() const { return pPEB; }

   private:
    HANDLE hProcess = INVALID_HANDLE_VALUE;
    nt64::PEB* pPEB = nullptr;
    wchar_t* wcCmdLine = nullptr;
};

class Process {
   public:
    BOOL isWow64 = FALSE;
    DWORD pid;
    Peb* pPeb = nullptr;
    wchar_t* wcCommandLine = nullptr;
    wchar_t* wcImagePath = nullptr;
    wchar_t* wcProcessName = nullptr;
    std::map<uint64_t /* StackBase */, Thread*> StackBases;
    std::map<uint64_t /* StartVA*/, MemoryBaseRegion*> MemoryBaseEntries;
    std::map<uint64_t /* StartVA */, MemoryRegion*> MemoryEntries;
    std::vector<uint64_t /* HeapVA */> HeapAddressList;
    Process(DWORD pid);
    void PrintProcessInfo();

   private:
    HANDLE hProcess = NULL;
    void SetStackBaseAddress();
    void SetMemoryRegions();
};
}  // namespace yama
#endif  // _PROCESS_HPP
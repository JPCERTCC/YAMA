
#ifndef _MEMORY_HPP
#define _MEMORY_HPP

#include "common.h"

// should include after common.h
#include <winternl.h>

#include "common.h"
#include "typedef.h"

namespace yama {
typedef NTSTATUS(__stdcall* NtQueryVirtualMemory_t)(HANDLE, void*, nt::MEMORY_INFORMATION_CLASS,
                                                    void*, SIZE_T, SIZE_T*);

class MemoryRegion {
   public:
    uint64_t StartVa = 0;
    uint64_t EndVa = 0;
    uint64_t BaseVa = 0;
    const char* MemType = nullptr;
    const char* MemState = nullptr;
    char* MemProtect = nullptr;
    char* MemAllocationProtect = nullptr;
    wchar_t* wcMappedFile = nullptr;
    uint32_t RegionSize = 0;
    uint32_t PrivateSize = 0;
    MemoryRegion(HANDLE hProcess, MEMORY_BASIC_INFORMATION* pMemBasicInfo);
    bool DumpRegion(_In_ void* lpBuffer, _In_ uint32_t dwSizeOfBuffer,
                    _Out_ uint32_t* lpdwWrittenSize);
    void PrintRegionInfo(uint8_t indent = 4);

    // virtual ~MemoryRegion();
   private:
    HANDLE hProcess = NULL;
    const MEMORY_BASIC_INFORMATION* pMemBasicInfo;
    DWORD QueryPrivateSize();
    const char* GetStateSymbolName(DWORD dwState);
    const char* GetTypeSymbolName(DWORD dwType);
    char* GetProtectSymbolName(DWORD dwProtected);
    bool SetMemoryBasicInformation();
};

class MemoryBaseRegion {
   public:
    uint32_t dwState = 0;
    uint32_t dwRegionCount = 0;
    wchar_t* wcMappedFile = nullptr;
    wchar_t* wcLdrDllFullPath = nullptr;
    const char* cLdrDllLoadReson = "none";
    SIZE_T ImageSize = 0;
    ULONG ImageNotExecutable = 0;
    ULONG ImageSigningLevel = 0;
    uint64_t StartVa = 0;
    bool isHeap = false;
    bool isStack = false;
    uint32_t dwThreadId = 0;
    // uint32_t dwHeapId = 0; // TBD
    std::map<uint64_t, MemoryRegion*> SubRegions;
    MemoryBaseRegion(HANDLE hProcess, MEMORY_BASIC_INFORMATION* pMemBasicInfo);
    bool AppendSubRegion(MemoryRegion* region);
    void PrintBaseRegionInfo(uint8_t indent = 0);

   private:
    HANDLE hProcess = NULL;
    void SetMappedFile();
    bool SetMemoryImageInformation();
    nt64::MEMORY_IMAGE_INFORMATION* pMemImageInfo;
};
} // namespace yama

#endif  // _MEMORY_HPP

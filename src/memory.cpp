#include "memory.hpp"

#include "file.h"

namespace yama {
static NtQueryVirtualMemory_t NtQueryVirtualMemory = nullptr;

MemoryRegion::MemoryRegion(HANDLE hProcess, MEMORY_BASIC_INFORMATION* pMemBasicInfo) {
    this->hProcess = hProcess;
    this->pMemBasicInfo = pMemBasicInfo;
    this->SetMemoryBasicInformation();
}

const char* MemoryRegion::GetStateSymbolName(DWORD dwState) {
    switch (dwState) {
        case MEM_COMMIT:
            return "MEM_COMMIT";
        case MEM_FREE:
            return "MEM_FREE";
        case MEM_RESERVE:
            return "MEM_RESERVE";
        default:
            return "?";
    }
}

const char* MemoryRegion::GetTypeSymbolName(DWORD dwType) {
    switch (dwType) {
        case MEM_IMAGE:
            return "MEM_IMAGE";
        case MEM_MAPPED:
            return "MEM_MAPPED";
        case MEM_PRIVATE:
            return "MEM_PRIVATE";
        default:
            return "?";
    }
}

char* MemoryRegion::GetProtectSymbolName(DWORD dwProtected) {
    char* strProtected = (char*)calloc(8, sizeof(char));
    if (!dwProtected) {
        return strProtected;
    }
    if (dwProtected & PAGE_READONLY) {
        strncat_s(strProtected, 8, "R", 1);
    } else if (dwProtected & PAGE_NOACCESS) {
        strncat_s(strProtected, 8, "NA", 2);
    } else if (dwProtected & PAGE_READWRITE) {
        strncat_s(strProtected, 8, "RW", 2);
    } else if (dwProtected & PAGE_EXECUTE_READ) {
        strncat_s(strProtected, 8, "RX", 2);
    } else if (dwProtected & PAGE_EXECUTE_READWRITE) {
        strncat_s(strProtected, 8, "RWX", 3);
    } else if (dwProtected & PAGE_EXECUTE_WRITECOPY) {
        strncat_s(strProtected, 8, "RWXC", 4);
    } else if (dwProtected & PAGE_EXECUTE) {
        strncat_s(strProtected, 8, "X", 1);
    } else if (dwProtected & PAGE_WRITECOPY) {
        strncat_s(strProtected, 8, "WC", 1);
    }

    if (dwProtected & PAGE_WRITECOMBINE) {
        strncat_s(strProtected, 8, "+WC", 3);
    } else if (dwProtected & PAGE_GUARD) {
        strncat_s(strProtected, 8, "+G", 2);
    } else if (dwProtected & PAGE_NOCACHE) {
        strncat_s(strProtected, 8, "+NC", 3);
    }
    return strProtected;
}

DWORD MemoryRegion::QueryPrivateSize() {
    if (strcmp(this->MemState, "MEM_COMMIT")) {
        return 0;
    }
    if (!strcmp(this->MemType, "MEM_PRIVATE")) {
        return this->RegionSize;
    }
    DWORD dwPrivateSize = 0;
    for (DWORD dwPageOffset = 0; dwPageOffset < this->RegionSize; dwPageOffset += 0x1000) {
        PSAPI_WORKING_SET_EX_INFORMATION* pWorkingSet = new PSAPI_WORKING_SET_EX_INFORMATION();
        pWorkingSet->VirtualAddress = reinterpret_cast<void*>(this->StartVa + dwPageOffset);
        if (K32QueryWorkingSetEx(this->hProcess, pWorkingSet,
                                 sizeof(PSAPI_WORKING_SET_EX_INFORMATION))) {
            if (!pWorkingSet->VirtualAttributes.Shared) {
                dwPrivateSize += 0x1000;
            }
        }
    }
    return dwPrivateSize;
}

bool MemoryRegion::SetMemoryBasicInformation() {
    this->MemProtect = GetProtectSymbolName(this->pMemBasicInfo->Protect);
    this->MemAllocationProtect = GetProtectSymbolName(this->pMemBasicInfo->AllocationProtect);
    this->MemType = GetTypeSymbolName(this->pMemBasicInfo->Type);
    this->MemState = GetStateSymbolName(this->pMemBasicInfo->State);
    this->RegionSize = this->pMemBasicInfo->RegionSize;
    this->BaseVa = reinterpret_cast<uint64_t>(this->pMemBasicInfo->AllocationBase);
    this->StartVa = reinterpret_cast<uint64_t>(this->pMemBasicInfo->BaseAddress);
    this->EndVa = this->StartVa + this->RegionSize;
    this->PrivateSize = QueryPrivateSize();
    return true;
}

bool MemoryRegion::DumpRegion(_In_ void* lpBuffer, _In_ uint32_t dwSizeOfBuffer,
                              _Out_ uint32_t* lpdwWrittenSize) {
    bool bRes;
    bRes = ReadProcessMemory(this->hProcess, reinterpret_cast<void*>(this->StartVa), lpBuffer,
                             dwSizeOfBuffer, reinterpret_cast<SIZE_T*>(lpdwWrittenSize));
    return bRes;
}

void MemoryRegion::PrintRegionInfo(uint8_t indent) {
    printf("%*s%#.16I64x  %-6s%-6s%-14s%-14s%dKB\t%dKB\n", indent, " ", this->StartVa,
           this->MemProtect, this->MemAllocationProtect, this->MemType, this->MemState,
           this->RegionSize / 1024, this->PrivateSize / 1024);
}

MemoryBaseRegion::MemoryBaseRegion(HANDLE hProcess, MEMORY_BASIC_INFORMATION* pMemBasicInfo) {
    this->StartVa = reinterpret_cast<uint64_t>(pMemBasicInfo->AllocationBase);
    this->hProcess = hProcess;
    this->SetMemoryImageInformation();
    this->SetMappedFile();
}

bool MemoryBaseRegion::SetMemoryImageInformation() {
    NTSTATUS NtStatus;
    this->pMemImageInfo = new nt64::MEMORY_IMAGE_INFORMATION();
    if (!NtQueryVirtualMemory) {
        NtQueryVirtualMemory = reinterpret_cast<NtQueryVirtualMemory_t>(
            GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryVirtualMemory"));
    }
    NtStatus = NtQueryVirtualMemory(this->hProcess, reinterpret_cast<void*>(this->StartVa),
                                    nt::MEMORY_INFORMATION_CLASS::MemoryImageInformation,
                                    (PVOID)this->pMemImageInfo,
                                    sizeof(nt64::MEMORY_IMAGE_INFORMATION), NULL);
    if (NT_SUCCESS(NtStatus)) {
        this->ImageNotExecutable = pMemImageInfo->ImageNotExecutable;
        this->ImageSize = pMemImageInfo->SizeOfImage;
        this->ImageSigningLevel = pMemImageInfo->ImageSigningLevel;
    }
    return true;
}

void MemoryBaseRegion::SetMappedFile() {
    // Query Mapped FileName
    uint32_t dwSize = 0;
    this->wcMappedFile = (wchar_t*)calloc(MAX_PATH, sizeof(wchar_t));
    dwSize = K32GetMappedFileNameW(this->hProcess, reinterpret_cast<void*>(this->StartVa),
                                   this->wcMappedFile,
                                   MAX_PATH);  // filename is returen with device path.
    if (dwSize > 0) {
        this->wcMappedFile = ConvertDevicePath(this->wcMappedFile);
    } else {
        wsprintf(this->wcMappedFile, L"none");
    }
    //  wprintf(L"[Mapped File] %s\t%#.16I64x\n", this->wcMappedFile,
    //  (uint64_t)this->pMemBasicInfo->BaseAddress);
}

bool MemoryBaseRegion::AppendSubRegion(MemoryRegion* region) {
    if (region->BaseVa != this->StartVa) {
        return false;
    }
    this->SubRegions[region->StartVa] = region;
    this->dwRegionCount++;
    return true;
}

void MemoryBaseRegion::PrintBaseRegionInfo(uint8_t indent) {
    printf("%*sBaseAddress : %#.16I64x\n", indent, " ", this->StartVa);
    wprintf(L"%*sMappedFile  : %s\n", indent, L" ", this->wcMappedFile);
    wprintf(L"%*sLdrDllPath  : %s\n", indent, L" ", this->wcLdrDllFullPath);
    printf("%*sLdrLoadReson: %s\n", indent, " ", this->cLdrDllLoadReson);
    printf("%*sisHeap      : %s\n", indent, " ", this->isHeap ? "true" : "false");
    printf("%*sisStack     : %s\n", indent, " ", this->isStack ? "true" : "false");
    printf("%*sCodeSigning : %s\n", indent, " ", this->ImageSigningLevel ? "Singned" : "-");
}
}  // namespace yama

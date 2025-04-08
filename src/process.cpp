#include "process.hpp"

#include "file.h"

namespace yama {
const char* LdrEntry::GetLoadReasonName(uint16_t LoadReason) {
    switch (LoadReason) {
        case nt::LDR_DLL_LOAD_REASON::LoadReasonAsDataLoad:
            return "AsDataLoad";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonAsImageLoad:
            return "AsImageLoad";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonDelayloadDependency:
            return "DelayLoadDependency";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonDynamicForwarderDependency:
            return "DynamicForwarderDependency";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonDynamicLoad:
            return "DynamicLoad";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonStaticDependency:
            return "StaticDependency";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonStaticForwarderDependency:
            return "StaticForwarderDependency";
        case nt::LDR_DLL_LOAD_REASON::LoadReasonUnknown:
            return "Unknown";
        default:
            return "?";
    }
}

LdrEntry::LdrEntry(HANDLE hProcess, void* lpDataTableEntry) {
    nt64::LDR_DATA_TABLE_ENTRY* lpModuleData = new nt64::LDR_DATA_TABLE_ENTRY();
    ReadProcessMemory(hProcess, lpDataTableEntry, lpModuleData, sizeof(nt64::LDR_DATA_TABLE_ENTRY),
                      NULL);
    this->wcDllFullPath = (wchar_t*)calloc(lpModuleData->FullDllName.u.Length + 1, sizeof(wchar_t));
    ReadProcessMemory(hProcess, reinterpret_cast<void*>(lpModuleData->FullDllName.Buffer),
                      LdrEntry::wcDllFullPath, lpModuleData->FullDllName.u.Length, NULL);
    // wprintf(L"[module path] %s\n", this->wcDllFullPath);
    this->lpBaseAddress = lpModuleData->DllBase;
    this->cLoadReason = LdrEntry::GetLoadReasonName(lpModuleData->LoadReason);
    this->SigningLevel = lpModuleData->SigningLevel;
    this->dwImageSize = lpModuleData->SizeOfImage;
}

Peb::Peb(HANDLE hProcess) {
    this->hProcess = hProcess;
    PROCESS_BASIC_INFORMATION* pPBI = new PROCESS_BASIC_INFORMATION();
    this->pPEB = new nt64::PEB();

    // read PBI
    NtQueryInformationProcess(hProcess, ProcessBasicInformation, pPBI,
                              sizeof(PROCESS_BASIC_INFORMATION), NULL);
    // read PEB
    if (!ReadProcessMemory(hProcess, pPBI->PebBaseAddress, this->pPEB, sizeof(nt64::PEB), NULL)) {
        DWORD err = GetLastError();
        this->pPEB = nullptr;
        LOGTRACE("Failed to get address of PEB. error_code: {}", err);
        return;
    }

    this->SetProcessParam();

    // get loaded modules for peb
    PEB_LDR_DATA* pLdrData = new PEB_LDR_DATA();
    ReadProcessMemory(hProcess, reinterpret_cast<void*>(this->pPEB->Ldr), pLdrData,
                      sizeof(PEB_LDR_DATA), NULL);
    PLIST_ENTRY pModuleList = (PLIST_ENTRY)pLdrData->InMemoryOrderModuleList.Flink;
    PLIST_ENTRY pModuleListLast = new LIST_ENTRY();
    ReadProcessMemory(hProcess, pModuleList, pModuleListLast, sizeof(LIST_ENTRY), NULL);
    pModuleListLast = pModuleListLast->Blink;
    while (pModuleList != pModuleListLast) {
        LdrEntry* data = new LdrEntry(hProcess, pModuleList);
        this->mapLoadedModules[data->lpBaseAddress] = data;
        PLIST_ENTRY pModuleListNext = new LIST_ENTRY();
        ReadProcessMemory(hProcess, pModuleList, pModuleListNext, sizeof(LIST_ENTRY), NULL);
        pModuleList = pModuleListNext->Flink;
    }
}

void Peb::SetProcessParam() {
    nt64::RTL_USER_PROCESS_PARAMETERS* ProcessParam = new nt64::RTL_USER_PROCESS_PARAMETERS();
    if (ReadProcessMemory(this->hProcess, reinterpret_cast<void*>(this->pPEB->ProcessParameters),
                          ProcessParam, sizeof(nt64::RTL_USER_PROCESS_PARAMETERS), NULL)) {
        // set command line
        this->wcCmdLine = (wchar_t*)calloc(ProcessParam->CommandLine.u.Length + 1, sizeof(wchar_t));
        ReadProcessMemory(this->hProcess, reinterpret_cast<void*>(ProcessParam->CommandLine.Buffer),
                          this->wcCmdLine, ProcessParam->CommandLine.u.Length, NULL);
    }
}

std::vector<uint64_t> Peb::GetHeapAddressList() {
    std::vector<uint64_t> HeapAddressList;
    // get heap address from peb
    DWORD dwNumberOfHeaps = this->pPEB->NumberOfHeaps;

    // peb->ProcessHeaps points the array of pointers which points top of heap.
    // dereffered structure is _HEAP (NT Heap) or _SEGMENT_HEAP (Segment Heap).
    // we could identify the type of heap by the signature at offset +0x010
    // more detail, reffer https://github.com/ufrisk/MemProcFS/blob/master/vmm/vmmheap.c
    DWORD dwHeapsSize = dwNumberOfHeaps * sizeof(void*);
    std::unique_ptr<uint64_t[]> Heaps = std::make_unique<uint64_t[]>(dwNumberOfHeaps);
    if (ReadProcessMemory(hProcess, reinterpret_cast<void*>(this->pPEB->ProcessHeaps), Heaps.get(),
                          dwHeapsSize, nullptr)) {
        for (uint32_t i = 0; i < dwNumberOfHeaps; i++) {
            HeapAddressList.push_back(Heaps[i]);
        }
    }
    return HeapAddressList;
}

Process::Process(DWORD pid) {
    this->pid = pid;
    this->hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (this->hProcess == NULL) {
        LOGTRACE("Failed to open process handle. pid: {:d}, error: 0x{:x}", pid, GetLastError());
        return;
    }
    // check if target process is wow64
    IsWow64Process(hProcess, &this->isWow64);

    // TBU: support wow64 process

    // PEB
    this->pPeb = new Peb(hProcess);
    if (this->pPeb->GetPEB() == nullptr) {
        LOGERROR("Failed to read PEB for process {}. Skip scanning.", this->pid);
        return;
    }

    // Set Process Name
    this->wcProcessName = (wchar_t*)calloc(MAX_PATH, sizeof(wchar_t));
    GetModuleBaseNameW(this->hProcess, NULL, this->wcProcessName, MAX_PATH);

    // Set Image Path
    this->wcImagePath = (wchar_t*)calloc(MAX_PATH, sizeof(wchar_t));
    K32GetProcessImageFileNameW(this->hProcess, this->wcImagePath, MAX_PATH);
    this->wcImagePath = ConvertDevicePath(this->wcImagePath);

    // Set CommandLine
    this->wcCommandLine = this->pPeb->GetCmdLine();

    // Set StackBase Address from Thread
    this->SetStackBaseAddress();

    // Set Heap Memory
    this->HeapAddressList = this->pPeb->GetHeapAddressList();

    // Set Memory Regions
    this->SetMemoryRegions();
}

void Process::SetStackBaseAddress() {
    HANDLE hThreadSnapshot = INVALID_HANDLE_VALUE;
    THREADENTRY32* pThreadEntry = new THREADENTRY32();

    hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnapshot == INVALID_HANDLE_VALUE) {
        LOGWARN("Faild to take thread snapshot.\n");
        return;
    }

    pThreadEntry->dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnapshot, pThreadEntry)) {
        do {
            if (pThreadEntry->th32OwnerProcessID == this->pid) {
                // printf("ThreadID: %lu\t PID: %lu\n", pThreadEntry->th32ThreadID,
                // pThreadEntry->th32OwnerProcessID);
                Thread* entry = new Thread(pThreadEntry->th32ThreadID, this->hProcess);
                StackBases[reinterpret_cast<uint64_t>(entry->pStackBase)] = entry;
            }
        } while (Thread32Next(hThreadSnapshot, pThreadEntry));
    }
}

void Process::SetMemoryRegions() {
    MEMORY_BASIC_INFORMATION* pMemInfo = new MEMORY_BASIC_INFORMATION();
    LPCVOID lpOffest = nullptr;
    // std::cout <<
    // "BaseAddress\tAllocationBase\tProtected\tAllocationProtect\tType\tState\tRegionSize\tPrivateSize"
    // << std::endl;
    while (VirtualQueryEx(hProcess, lpOffest, pMemInfo, sizeof(MEMORY_BASIC_INFORMATION))) {
        MemoryRegion* region = new MemoryRegion(hProcess, pMemInfo);
        this->MemoryEntries[region->StartVa] = region;
        if (region->BaseVa != 0) {
            if (this->MemoryBaseEntries.find(region->BaseVa) == this->MemoryBaseEntries.end()) {
                // add new base region entry
                MemoryBaseRegion* baseRegion = new MemoryBaseRegion(hProcess, pMemInfo);
                this->MemoryBaseEntries[region->BaseVa] = baseRegion;

                // check if the base region is used as stack
                if (std::find(this->HeapAddressList.begin(), this->HeapAddressList.end(),
                              region->BaseVa) != this->HeapAddressList.end()) {
                    baseRegion->isHeap = true;
                }
                // check if the base region is used as heap
                if (this->StackBases.find(region->BaseVa) != this->StackBases.end()) {
                    baseRegion->isStack = true;
                    baseRegion->dwThreadId = this->StackBases[region->BaseVa]->dwThreadId;
                }

                // check LDR_DATA_ENRTY
                if (this->pPeb->mapLoadedModules.find(region->BaseVa) !=
                    this->pPeb->mapLoadedModules.end()) {
                    baseRegion->wcLdrDllFullPath =
                        this->pPeb->mapLoadedModules[region->BaseVa]->wcDllFullPath;
                    baseRegion->cLdrDllLoadReson =
                        this->pPeb->mapLoadedModules[region->BaseVa]->cLoadReason;
                } else {
                    baseRegion->wcLdrDllFullPath = (wchar_t*)calloc(5, sizeof(wchar_t));
                    wsprintf(baseRegion->wcLdrDllFullPath, L"none");
                }
            }
            this->MemoryBaseEntries[region->BaseVa]->AppendSubRegion(region);
        }

        lpOffest = (LPVOID)((DWORD_PTR)pMemInfo->BaseAddress + pMemInfo->RegionSize);
    }
}

void Process::PrintProcessInfo() {
    // printf("ProcessID : %#.16I64x\n", this->pid);
    printf("ProcessID     : %lu\n", this->pid);
    wprintf(L"ProcessName   : %s\n", this->wcProcessName);
    printf("IsWow64Process: %s\n", this->isWow64 ? "true" : "false");
    wprintf(L"ImagePath     : %s\n", this->wcImagePath);
    wprintf(L"CommandLine   : %s\n", this->wcCommandLine);
    printf("HeapCount     : %llu\n", this->HeapAddressList.size());
    printf("StackCount    : %llu\n", this->StackBases.size());
}
}  // namespace yama

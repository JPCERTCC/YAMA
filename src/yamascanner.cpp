#include "yamascanner.hpp"

namespace yama {
YamaScanner::YamaScanner(std::vector<DWORD>* PidList) {
    this->PidList = PidList;
    this->suspiciousProcessList = new std::vector<SuspiciousProcess*>();
}

void YamaScanner::ScanPidList() {
    YrResult* yrResult = nullptr;
    for (DWORD dwPid : *this->PidList) {
        LOGTRACE("now scanning pid: {}", dwPid);
        Process* proc = new Process(dwPid);
        if (proc->pPeb == nullptr || proc->pPeb->GetPEB() == nullptr) {
            continue;
        }
        yrResult = this->ScanProcessMemory(proc);
        if (yrResult->result) {
            LOGINFO("YARA MATCH: pid={}, process_name={}", proc->pid, WideCharToUtf8(proc->wcProcessName));
            for (std::string strRuleName : *(yrResult->matchRuleSet)){
                LOGINFO("DETECTED RULE: {}",strRuleName.c_str());
            }
            SuspiciousProcess *suspiciousProcess = new SuspiciousProcess(proc);
            suspiciousProcess->yaraMatchedRules = yrResult->matchRuleSet;
            this->suspiciousProcessList->push_back(suspiciousProcess);
        }
    }
}

void YamaScanner::InitYaraManager(const char* lpcYaraRuleString) {
    this->yrManager = new YaraManager();
    this->yrManager->YrAddRuleFromString(lpcYaraRuleString);
}

YrResult* YamaScanner::ScanProcessMemory(Process* proc) {
    // Init yara result object
    YrResult* yrResult = new YrResult();
    yrResult->result = false;
    yrResult->matchRuleSet = new std::unordered_set<std::string>();
    std::vector<MemoryRegion*> *yaraMatchedRegions = new std::vector<MemoryRegion*>();

    std::map<uint64_t /*BaseVirtualAddress*/, MemoryBaseRegion*>::iterator iterAddress_MemeryBaseRegion = proc->MemoryBaseEntries.begin();
    while (iterAddress_MemeryBaseRegion != proc->MemoryBaseEntries.end()) {
        // uint64_t BaseVa = iterAddress_MemeryBaseRegion->first;
        MemoryBaseRegion* BaseRegion = iterAddress_MemeryBaseRegion->second;
        // printf("=============\n");
        // BaseRegion->PrintBaseRegionInfo();
        std::map<uint64_t, MemoryRegion*>::iterator iterAddress_SubRegion = BaseRegion->SubRegions.begin();
        while (iterAddress_SubRegion != BaseRegion->SubRegions.end()) {
            MemoryRegion* Region = iterAddress_SubRegion->second;
            if (strcmp(Region->MemState, "MEM_COMMIT") == 0 && strcmp(Region->MemType, "MEM_PRIVATE") == 0) {
                if (strstr(Region->MemProtect, "X") != nullptr) {  // only scan executable region.
                    // printf("[scan]");
                    const unsigned char* lpBuffer = reinterpret_cast<const unsigned char*>(calloc(Region->RegionSize, 1));

                    Region->DumpRegion((void*)lpBuffer, Region->RegionSize, nullptr);

                    this->yrManager->YrScanBuffer(lpBuffer, Region->RegionSize, reinterpret_cast<void*>(yrResult));
                    if (yrResult->result) {
                        yaraMatchedRegions->push_back(Region);
                        // return yrResult;
                    }
                }
            }
            // WriteDumpFile(lpBuffer, Region->RegionSize, Region->StartVa);
            // Region->PrintRegionInfo();

            iterAddress_SubRegion++;
        }
        iterAddress_MemeryBaseRegion++;
    }
    return yrResult;
}

}  // namespace yama
#ifndef SRC_YAMASCANNER_HPP
#define SRC_YAMASCANNER_HPP

#include "common.h"
#include "process.hpp"
#include "rc4.hpp"
#include "resource.h"
#include "suspicious_process.hpp"
#include "yaramanager.hpp"

namespace yama {

// Class to manage the scanner and the scan results.
class YamaScanner {
   public:
    YamaScanner(std::vector<DWORD /* PID */>* PidList);

    std::vector<SuspiciousProcess*>* suspiciousProcessList = nullptr;

    void ScanPidList();
    // std::vector<SuspiciousProcess*>* GetScanResult();
    void InitYaraManager(const char* lpcYaraRuleString);

   private:
    std::vector<DWORD /* PID */>* PidList = nullptr;
    YaraManager* yrManager = nullptr;

    YrResult* ScanProcessMemory(Process* proc);
};

}  // namespace yama
#endif  // SRC_YAMASCANNER_HPP
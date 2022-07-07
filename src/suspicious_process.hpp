#ifndef SRC_SUSPICIOUS_PROCESS
#define SRC_SUSPICIOUS_PROCESS

#include "common.h"
#include "memory.hpp"
#include "process.hpp"

namespace yama {
/// Class to handle Process object which yara detects.
class SuspiciousProcess {
   public:
    SuspiciousProcess(Process *process);
    // const wchar_t *GetCmdLine();
    // void DumpSuspiciousRegions();
    std::unordered_set<std::string /*matched_rule_name*/> *yaraMatchedRules = nullptr;
    Process *process = nullptr;

   private:
    std::vector<MemoryRegion *> *yaraMatchedRegions = nullptr;
};

}  // namespace yama

#endif  // SRC_SUSPICIOUS_PROCESS

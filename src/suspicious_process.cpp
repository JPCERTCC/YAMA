#include "suspicious_process.hpp"

namespace yama {

yama::SuspiciousProcess::SuspiciousProcess(Process* process) {
    // initialize 
    this->process = process;
    this->yaraMatchedRegions = new std::vector<MemoryRegion*>();
}

}  // namespace yama
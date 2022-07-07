#ifndef SRC_REPORTER_HPP
#define SRC_REPORTER_HPP

#include "common.h"
#include "suspicious_process.hpp"
#include "scanner_context.hpp"

#include <nlohmann/json.hpp>

namespace yama {

class Reporter {
   public:
    Reporter(ScannerContext *context, std::vector<SuspiciousProcess *> *suspiciousProcessList);
    std::string *GenerateTextReport();
    std::string *GenerateJsonReport();
    Language lang = Language::EN;

   private:
    ScannerContext *context = nullptr;
    std::vector<SuspiciousProcess *> *suspiciousProcessList = nullptr;
};

}  // namespace yama

#endif  // SRC_REPORTER_HPP
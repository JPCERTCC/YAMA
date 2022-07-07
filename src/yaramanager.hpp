#ifndef SRC_YARAMANAGER_HPP
#define SRC_YARAMANAGER_HPP

#include <yara.h>
#include <unordered_set>
#include "logger.h"

namespace yama {
static const char* sample_rule =
    "rule mz_header {"
    "  condition:"
    "    uint16(0) == 0x5A4D "
    "}";

struct YrResult {
    bool result;
    std::unordered_set<std::string /*rule_name*/> *matchRuleSet = nullptr;
};

class YaraManager {
   private:
    YR_COMPILER* YrCompiler = nullptr;
    YR_SCANNER* YrScanner = nullptr;
    YR_RULES* YrRules = nullptr;

   public:
    YaraManager();
    static int YrScanCallback(YR_SCAN_CONTEXT* context, int message, void* message_data,
                              void* user_data);
    // void YrCompilerErrorCallback(int error_level, const char* file_name, int line_number, const
    // YR_RULE* rule, const char* message, void* user_data);
    bool YrCreateScanner();
    bool YrAddRuleFromString(const char* strRule);
    void YrScanBuffer(const unsigned char* lpBuffer, int dwBufferSize, void* lpUserData);

    ~YaraManager();
};

}  // namespace yama
#endif  // SRC_YARAMANAGER_HPP
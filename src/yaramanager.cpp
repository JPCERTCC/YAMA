#include "yaramanager.hpp"

namespace yama {

YaraManager::YaraManager() {
    int res = yr_initialize();
    if (res != ERROR_SUCCESS) {
        LOGERROR("Failed to initialize libyara. Error:0x{:x}", res);
        return;
    }
    LOGTRACE("Initialized YaraManager");
    // create yara compiler
    res = yr_compiler_create(&this->YrCompiler);
    if (res != ERROR_SUCCESS) {
        LOGERROR("Failed to create yara compiler. Error:0x{:x}", res);
    }
    return;
}

bool YaraManager::YrCreateScanner() {
    int dwRes = yr_scanner_create(this->YrRules, &this->YrScanner);
    if (dwRes != ERROR_SUCCESS) {
        LOGERROR("Failed to create scanner. Error:0x{:x}", dwRes);
    }
    return true;
}

bool YaraManager::YrAddRuleFromString(const char* strRule) {
    int res = yr_compiler_add_string(this->YrCompiler, strRule, nullptr);
    if (res != ERROR_SUCCESS) {
        LOGERROR("Failed to add rule. Error:0x{:x}", res);
        return false;
    }
    LOGTRACE("Add new rule to compiler.");

    res = yr_compiler_get_rules(this->YrCompiler, &this->YrRules);
    if (res != ERROR_SUCCESS) {
        LOGERROR("Failed to get rules from compiler. Error:0x{:x}", res);
        return false;
    }
    LOGTRACE("Get {:d} rules from compiler.", this->YrRules->num_rules);
    return true;
}

int YaraManager::YrScanCallback(YR_SCAN_CONTEXT* context, int message, void* message_data,
                                void* user_data) {
    LOGTRACE("YrScanCallback. {:d}", message);
    if (message == CALLBACK_MSG_RULE_MATCHING) {
        YR_RULE* yrRule = (YR_RULE*)message_data;
        YR_STRING* yrString = nullptr;
        char* lpcRuleName = new char[128]();
        strncpy_s(lpcRuleName, 128, yrRule->identifier, strlen(yrRule->identifier));
        LOGTRACE("[MATCH] rulename: {}", lpcRuleName);
        reinterpret_cast<YrResult*>(user_data)->result = true;
        std::string* strRuleName = new std::string(lpcRuleName); 
        reinterpret_cast<YrResult*>(user_data)->matchRuleSet->insert(*strRuleName);
        
        // release buffer
        delete[] lpcRuleName;
        lpcRuleName = nullptr;

        yr_rule_strings_foreach(yrRule, yrString) {}
    }

    return CALLBACK_CONTINUE;
}

void YaraManager::YrScanBuffer(const unsigned char* lpBuffer, int dwBufferSize, void* lpUserData) {
    // printf("YrScanBuffer. Va:%#.16I64x Size:%d\n",
    // reinterpret_cast<uint64_t>(lpBuffer), dwBufferSize);
    yr_rules_scan_mem(this->YrRules, lpBuffer, dwBufferSize, SCAN_FLAGS_REPORT_RULES_MATCHING,
                      this->YrScanCallback, lpUserData, 0);
    // printf("YrScanBuffer finished.\n");
    return;
}

YaraManager::~YaraManager() {
    if (this->YrRules != nullptr) {
        yr_rules_destroy(this->YrRules);
    }
    if (this->YrCompiler != nullptr) {
        yr_compiler_destroy(this->YrCompiler);
    }
    // if (this->YrScanner != nullptr) {
    //     yr_scanner_destroy(this->YrScanner);
    // }
    int res = yr_finalize();
    if (res != ERROR_SUCCESS) {
        LOGERROR("Failed to finalize libyara. Error:0x{:x}", res);
    }
    LOGTRACE("Finalized YaraManager");
    return;
}

}  // namespace yama
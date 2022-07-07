#include "reporter.hpp"
namespace yama {

Reporter::Reporter(ScannerContext* context, std::vector<SuspiciousProcess*>* suspiciousProcessList) {
    this->context = context;
    this->suspiciousProcessList = suspiciousProcessList;
}

std::string* Reporter::GenerateTextReport() { 
    std::string *strTextData = new std::string();
    *strTextData += "[YAMA Result]\n";
    *strTextData += "Scan time: ";
    *strTextData += std::string(this->context->lpcScanTime) + "\n";
    *strTextData += "Host Name: ";
    *strTextData += WideCharToUtf8(this->context->lpwHostName);
    *strTextData += "\n\n";
    if (suspiciousProcessList->size() > 0) {
        *strTextData += "[Result] \nDetected suspicious process(es).\n\n";
        for (SuspiciousProcess *suspiciousProcess : *suspiciousProcessList){
            *strTextData += "[Suspicious Process]";
            *strTextData += "\n  Process Name  : ";
            *strTextData += WideCharToUtf8(suspiciousProcess->process->wcProcessName);
            *strTextData += "\n  Process ID    : ";
            *strTextData += std::to_string(suspiciousProcess->process->pid);
            *strTextData += "\n  Image Path    : ";
            *strTextData += WideCharToUtf8(suspiciousProcess->process->wcImagePath);
            *strTextData += "\n  Matched Rules : ";
            for (std::string strMatchRule : *suspiciousProcess->yaraMatchedRules){
                *strTextData += strMatchRule;
                *strTextData += " ";
            }
            *strTextData += "\n\n";
        }
    } else {
        *strTextData += "[Result] \nNo detection.\n";
    }
    return strTextData;
}

std::string* Reporter::GenerateJsonReport() {
    nlohmann::json jsonData;
    // construct json data
    jsonData["scan_time"] = std::string(this->context->lpcScanTime);
    jsonData["hostname"] = WideCharToUtf8(this->context->lpwHostName);
    if (suspiciousProcessList->size() > 0) {
        jsonData["is_infected"] = true;
    } else {
        jsonData["is_infected"] = false;
    }
    if (jsonData["is_infected"]){
        for (SuspiciousProcess *suspiciousProcess : *suspiciousProcessList){
            nlohmann::json jsonSuspiciousProcess;
            jsonSuspiciousProcess["process_name"] = WideCharToUtf8(suspiciousProcess->process->wcProcessName);
            jsonSuspiciousProcess["process_id"] = suspiciousProcess->process->pid;
            jsonSuspiciousProcess["image_path"] = WideCharToUtf8(suspiciousProcess->process->wcImagePath);
            for (std::string strMatchRule : *suspiciousProcess->yaraMatchedRules){
                jsonSuspiciousProcess["matched_rules"].push_back(strMatchRule);
            }
            jsonData["suspicious_process"].push_back(jsonSuspiciousProcess);
        }
    }

    // convert json into std::string
    std::string *strJsonData = new std::string(jsonData.dump(2));

    return strJsonData;
}

}  // namespace yamas
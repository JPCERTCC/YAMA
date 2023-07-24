#define NOMINMAX
#include <argparse/argparse.hpp>

#include "../rsrc/resources.h"
#include "common.h"
#include "memory.hpp"
#include "pid.h"
#include "process.hpp"
#include "rc4.hpp"
#include "resource.h"
#include "reporter.hpp"
#include "scanner_context.hpp"
#include "thread.hpp"
#include "yaramanager.hpp"
#include "yamascanner.hpp"

const char* version = "1.0";

const char* banner = " __  _____   __  ______\n" 
                     " \\ \\/ / _ | /  |/  / _ |\n"
                     "  \\  / __ |/ /|_/ / __ |\n"
                     "  /_/_/ |_/_/  /_/_/ |_|\n"
                     "Yet Another Memory Analyzer for malware detection.\n";

int main(int argc, char* argv[]) {
    int verbosity = 0;

    // construct argument parser
    argparse::ArgumentParser program("yama.exe", version,  argparse::default_arguments::help);
    // program.add_description(banner);
    program.add_argument("-V", "--version").action([&](const auto &) {
          std::cout << version;
          std::exit(0);
        }).default_value(false)
        .help("prints version information and exits")
        .implicit_value(true)
        .nargs(0);
    program.add_argument("-p", "--pid").help("PID to scan (required if not using --all)").scan<'u', unsigned int>();
    program.add_argument("-a", "--all").default_value(false).help("Scan all processes (required if not using --pid)").nargs(0);
    program.add_argument("-o", "--output").default_value(std::string("./")).help("Specify output directory").nargs(1);
    program.add_argument("-j", "--json").default_value(false).help("Export json report to the output directory").nargs(0);
    program.add_argument("-e", "--evtx")
        .default_value(false)
        .help("Install YAMA EventLog manifest.")
        .nargs(0);
    program.add_argument("-u", "--uninstall")
        .default_value(false)
        .help("Uninstall YAMA EventLog manifest. (only do uninstall operaiton)")
        .nargs(0);
    program.add_argument("-s", "--suppress").default_value(false).help("Suppress warning logs").nargs(0);
    program.add_argument("-q", "--quiet").default_value(false).help("Suppress all console outputs").nargs(0);
    program.add_argument("-v", "--verbose")
        .action([&](const auto&) { ++verbosity; })
        .append()
        .default_value(false)
        .implicit_value(true)
        .help("Increase verbosity.(-v: info, -vv: debug, -vvv: trace)")
        .nargs(0);

    // display usage
    if (argc < 2) {
        std::cout << banner << std::endl;
        std::cout << program.help().str() << std::endl;
        return 0;
    }

    // parse arguments
    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    // init logger settings
    spdlog::set_pattern("%^%-9l%$: %v");  // info     : sample log
    // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^%-9l%$: %v"); // [2023-03-15 00:49:59.926] info
  
    // Set log verbosity level
    if (verbosity == 0) {
        spdlog::set_level(spdlog::level::warn);
    } else if (verbosity == 1) {
        spdlog::set_level(spdlog::level::info);
    } else if (verbosity == 2) {
        spdlog::set_level(spdlog::level::debug);
    } else {
        spdlog::set_level(spdlog::level::trace);
    }

    // Suppress warnning log
    if (program.is_used("-s")) {
        spdlog::set_level(spdlog::level::err);
    }

    // Suppress all output
    if (program.is_used("-q")) {
        spdlog::set_level(spdlog::level::off);
    }

    // Set output path
    std::string strOutputPath = program.get<std::string>("-o");

    // Convert output path to absolute path
    wchar_t* lpwcAbsPath = new wchar_t[MAX_PATH]();
    DWORD dwResult = GetFullPathNameW(yama::StdStringToWideChar(strOutputPath),MAX_PATH,lpwcAbsPath,NULL);
    if (dwResult == 0){
        LOGERROR("Failed to expand relative path. Set valid path.")
        return 1;
    }
    if (!yama::PathExistsW(lpwcAbsPath)){
        LOGWARN("Output path does not exists:  {}", yama::WideCharToUtf8(lpwcAbsPath));
        LOGWARN("Set output path to current directory.");
        GetCurrentDirectoryW(MAX_PATH, lpwcAbsPath);
    }
    
    strOutputPath = std::string(yama::WideCharToUtf8(lpwcAbsPath));
    LOGTRACE("Output path {}",strOutputPath);

    // Set output format
    bool isJson = false;  // output format is normal text
    if (program.is_used("-j")){
        isJson = true;    // output format is json style
    }

    // Check Invalid option combination
    if (program.is_used("-e") && program.is_used("-u")){
        LOGWARN("Invalid Option combination. --evtx and --uninstall can not use together.");
        return 0;
    }

    // Set scanner context
    yama::ScannerContext* context = new yama::ScannerContext();

    // Install Eventlog manifest
    if (context->isAdministrator && program.is_used("-e") && !context->canRecordEventlog) {
        if (context->InstallEventlogManifest()) {
            LOGINFO("Install Windows Event Log manifest.");
            context->canRecordEventlog = true;
        } else {
            LOGWARN("Failed to install  Windows Event Log manifest.");
        }
    }

    // Uninstall Eventlog manifest
    if (program.is_used("-u")) {
        if (!context->isAdministrator) {
            LOGWARN("Uninstall eventlog manifest requires administrative rights.");
        } else {
            if (context->UninstallEventlogManifest()) {
                LOGINFO("Uninstalled eventlog manifest.");
                return 0;
            } else {
                LOGWARN("Failed to uninstall Eventlog manifest. Close EventLog Viwer before uninstalling Eventlog manifest.");
                return 1;
            }
        }
    }

    // check required options
    if (!program.is_used("-a") && !program.is_used("-p")) {
        std::cout << program.help().str() << std::endl;
        return 0;
    }

    // register eventlog
    if (context->canRecordEventlog) { 
        LOGTRACE("Enabled Eventlog logging.");
        EventRegisterYama(); 
    }

    std::vector<DWORD> vPids;

    // check argument options
    if (program.is_used("--all")) {
        vPids = yama::ListPid();
    }

    if (program.is_used("--pid")) {
        DWORD pid = program.get<unsigned int>("pid");
        vPids.push_back(pid);
    }

    LOGINFO("Yama will scan {} process(es).",vPids.size());

    // Initialize YamaScanner
    yama::YamaScanner *scanner = new yama::YamaScanner(&vPids);
    scanner->InitYaraManager(yama::LoadYaraRuleFromResource());
    
    // Do memory scan
    if (context->canRecordEventlog) { EventWriteProcessStarted(); }
    scanner->ScanPidList();

    // Show detected processes count.
    LOGINFO("Suspicious Processes Count: {}", scanner->suspiciousProcessList->size());


    // write eventlog
    if (context->canRecordEventlog) {
        if (scanner->suspiciousProcessList->size() == 0){
        EventWriteNoDetection();
        } else {
        EventWriteDetectsMalware();
        }
    }

    // Generate report
    yama::Reporter *reporter = new yama::Reporter(context, scanner->suspiciousProcessList);
    std::string* strReport = nullptr;

    if (isJson) {
        strReport = reporter->GenerateJsonReport();
    } else {
        strReport = reporter->GenerateTextReport();
    }

    // Write result into console
    if (!program.is_used("-q") && strReport != nullptr) {
        std::cout << *strReport << std::endl;  
    }

    // Write result into output directory
    std::string strReportPath;
    strReportPath += strOutputPath;
    strReportPath += "\\";
    strReportPath += yama::WideCharToUtf8(context->lpwHostName);
    strReportPath += "_";
    strReportPath += std::string(context->lpcFilenameTime);
    strReportPath += "_yama.";
    if (isJson){
        strReportPath += "json";
    } else {
        strReportPath += "txt";
    }
    LOGINFO("Report file path: {}", strReportPath)

    std::ofstream ioOutputFile(strReportPath.c_str());

    if (ioOutputFile.is_open()){
        ioOutputFile << *strReport << std::endl;
        ioOutputFile.close();
        LOGINFO("File written successfully: {}", strReportPath);
    } else {
        LOGWARN("Failed to open file: {}", strReportPath);
    }

    // write eventlog and unregister eventlog
    if (context->canRecordEventlog) { 
        EventWriteProcessStopped(); 
        EventUnregisterYama();
    }

    return 0;
}
#ifndef SRC_SCANNER_CONTEXT_HPP
#define SRC_SCANNER_CONTEXT_HPP

#include <ctime>
#include <cstring>

#include "../rsrc/resources.h"
#include "common.h"
#include "etw_provider.h"
#include "file.h"
#include "privilege.h"
#include "resource.h"

namespace yama {
class ScannerContext {
   public:
    bool isAdministrator = false;
    bool haveDebugPrivilege = false;
    bool haveEvenlogManifestDll = false;
    bool isEtwProviderValid = false;
    bool canRecordEventlog = false;
    wchar_t* lpwHostName = nullptr;
    char* lpcScanTime = nullptr;
    char* lpcFilenameTime = nullptr;
    wchar_t* lpwOutputDirectory = nullptr;
    wchar_t* lpwOutputPath = nullptr;
    bool bTextOutput = true;
    bool bJsonOutput = false;
    bool bCmdlineOutput = true;
    bool bShowBannar = true;
    DWORD dwSelfPid;

    ScannerContext();
    bool EnableEventlogRecording();
    void SetOutputDirectory(LPCWSTR lpcwDirctoryPath);
    void SetOutputPath();
    bool InstallEventlogManifest();
    bool UninstallEventlogManifest();

   private:
    const wchar_t* lpcwManifestDllPath = L"%SystemDrive%\\Windows\\System32\\WinMemScan.Events.dll";
    const wchar_t* lpcwManifestImportCmd = L"cmd.exe /c \"wevtutil im %ws\"";
    const wchar_t* lpcwManifestUnloadCmd = L"cmd.exe /c \"wevtutil um %ws\"";
    bool CheckEventlogManifest();
    bool CheckEtwProviderGuid();
};
}  // namespace yama

#endif  // SRC_SCANNER_CONTEXT_HPP
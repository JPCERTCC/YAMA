#include "scanner_context.hpp"

namespace yama {

char* getCurrentTimeString(const size_t bufferSize, const char* lpcTimeFormat) {
    std::time_t currentTime = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &currentTime);

    // 時刻を指定された形式に変換
    char* timeBuffer = new char[bufferSize];
    std::strftime(timeBuffer, bufferSize, lpcTimeFormat, &localTime);

    return timeBuffer;
}

ScannerContext::ScannerContext() {
    // Get hostname
    this->lpwHostName = new wchar_t[256]();
    DWORD dwComputerNameSize = 256 / sizeof(wchar_t);
    GetComputerNameW(this->lpwHostName, &dwComputerNameSize);
    //wprintf(L"hostname: %s\n",this->lpwHostName);
    LOGTRACE("hostname: {}",WideCharToUtf8(this->lpwHostName));
    // Set scantime
    this->lpcScanTime = getCurrentTimeString(20, "%Y-%m-%d %H:%M:%S");
    this->lpcFilenameTime = getCurrentTimeString(16 ,"%Y%m%d%H%M%S");

    LOGTRACE("scantime: {}",this->lpcScanTime);
    LOGTRACE("filename: {}",this->lpcFilenameTime);

    // Check if scanner have administrative rights.
    this->isAdministrator = IsElevated();

    if (!this->isAdministrator) {
        LOGWARN("Yama is not running as Administrator. To scan all processes, relaunch YAMA with admin user.")
    }
    if (this->isAdministrator) {
        // Assign SeDebugPrivilege if the scanner run as Administrator.
        BOOL bRes = SetSeDebugPrivilege();
        if (bRes) {
            LOGTRACE("Set SeDebugPrivilege.");
            this->haveDebugPrivilege = true;
        }
    }

    // Check eventlog manifest.
    this->haveEvenlogManifestDll = PathExistsW(this->lpcwManifestDllPath);
    // Check ETW providers.
    this->isEtwProviderValid = IsGuidInEtwProviders(TheEventLog);
    // Set eventlog recording status.
    this->EnableEventlogRecording();
}

bool ScannerContext::EnableEventlogRecording() {
    if (this->haveEvenlogManifestDll && this->isEtwProviderValid) {
        this->canRecordEventlog = true;
    } else {
        this->canRecordEventlog = false;
    }
    return this->canRecordEventlog;
}

void ScannerContext::SetOutputDirectory(LPCWSTR lpcwDirctoryPath) {}

void ScannerContext::SetOutputPath() {}

bool ScannerContext::CheckEventlogManifest() {
    // TBD: implement if we need more strict checks.
    return false;
}

bool ScannerContext::CheckEtwProviderGuid() {
    // TBD: implement if we need more strict checks.
    return false;
}

bool ScannerContext::InstallEventlogManifest() {
    if (!this->isAdministrator) {
        // Install eventlog manifest requires administrative rights.
        return false;
    }

    // Install manifest dll into system directory.
    if (!this->haveEvenlogManifestDll) {
        LPWSTR lpwDllPath = (LPWSTR)calloc(MAX_PATH, sizeof(wchar_t));
        DWORD dwSize = 0;
        ExpandEnvironmentStringsW(this->lpcwManifestDllPath, lpwDllPath, MAX_PATH);
        dwSize = ExportFileResource(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_FILE1),
                                    lpwDllPath);  // Yama.event.dll
        if (dwSize > 0) {
            // success
            this->haveEvenlogManifestDll = true;
        } else {
            // failed
            this->haveEvenlogManifestDll = false;
            return false;
        }
    }

    // Import manifest to ETW providers.
    if (!this->isEtwProviderValid) {
        // extract manifest to temp file
        LPWSTR lpwTempDirPath = nullptr;
        DWORD dwTempDirPathLen = 0;
        LPWSTR lpwTempPath = (LPWSTR)calloc(MAX_PATH, sizeof(wchar_t));
        dwTempDirPathLen = GetTempPathW(0, lpwTempDirPath);
        lpwTempDirPath = (LPWSTR)calloc(dwTempDirPathLen + 1, sizeof(wchar_t));
        GetTempPathW(dwTempDirPathLen, lpwTempDirPath);
        GetTempFileNameW(lpwTempDirPath, L"", 0, lpwTempPath);

        DWORD dwSize = 0;
        dwSize = ExportFileResource(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_FILE2),
                                    lpwTempPath);  // Yama.event.man
        if (dwSize > 0) {
            // success
            DWORD dwCommandLength = 30 + wcslen(lpwTempPath);
            LPWSTR lpwCommandLine = (LPWSTR)calloc(dwCommandLength + 1, sizeof(wchar_t));
            wsprintf(lpwCommandLine, this->lpcwManifestImportCmd, lpwTempPath);
            _wsystem(lpwCommandLine);  // wevtutil im Yama.event.man
            // remove temp file
            DeleteFileW(lpwTempPath);
        } else {
            // Failed to export Yama.event.man from resource section.
            return false;
        }
        // Check Etw Providers once again.
        this->isEtwProviderValid = IsGuidInEtwProviders(TheEventLog);
    }
    if (this->isEtwProviderValid) {
        return true;
    }
    return false;
}

bool ScannerContext::UninstallEventlogManifest() {
    if (!this->isAdministrator) {
        // Uninstall eventlog manifest requires administrative rights.
        return false;
    }

    // Uninstall manifest dll into system directory.
    if (this->haveEvenlogManifestDll) {
        LPWSTR lpwDllPath = (LPWSTR)calloc(MAX_PATH, sizeof(wchar_t));
        // DWORD dwSize = 0;
        ExpandEnvironmentStringsW(this->lpcwManifestDllPath, lpwDllPath, MAX_PATH);
        if (!DeleteFileW(lpwDllPath)) {
            // Failed to remove manifest dll
            return false;
        }
        this->haveEvenlogManifestDll = false;
    }

    // Unload manifest from ETW providers.
    if (this->isEtwProviderValid) {
        // extract manifest to temp file
        LPWSTR lpwTempDirPath = nullptr;
        DWORD dwTempDirPathLen = 0;
        LPWSTR lpwTempPath = (LPWSTR)calloc(MAX_PATH, sizeof(wchar_t));
        dwTempDirPathLen = GetTempPathW(0, lpwTempDirPath);
        lpwTempDirPath = (LPWSTR)calloc(dwTempDirPathLen + 1, sizeof(wchar_t));
        GetTempPathW(dwTempDirPathLen, lpwTempDirPath);
        GetTempFileNameW(lpwTempDirPath, L"", 0, lpwTempPath);

        DWORD dwSize = 0;
        dwSize = ExportFileResource(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_FILE2),
                                    lpwTempPath);  // Yama.event.man
        if (dwSize > 0) {
            // success
            DWORD dwCommandLength = 30 + wcslen(lpwTempPath);
            LPWSTR lpwCommandLine = (LPWSTR)calloc(dwCommandLength + 1, sizeof(wchar_t));
            wsprintf(lpwCommandLine, this->lpcwManifestUnloadCmd, lpwTempPath);
            _wsystem(lpwCommandLine);  // wevtutil um Yama.event.man
            // remove temp file
            DeleteFileW(lpwTempPath);
        } else {
            // Failed to export Yama.event.man from resource section.
            return false;
        }
        // Check Etw Providers once again.
        this->isEtwProviderValid = IsGuidInEtwProviders(TheEventLog);
    }
    if (!this->isEtwProviderValid) {
        return true;
    }
    return false;
}


}  // namespace yama
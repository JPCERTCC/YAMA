#include "file.h"

namespace yama {

wchar_t* ConvertDevicePath(_In_ wchar_t* wcInPath) {
    wchar_t* wcOutPath = (wchar_t*)calloc(MAX_PATH, sizeof(wchar_t));
    for (uint16_t letter = 0x41 /* A */; letter <= 0x54 /* Z */; letter++) {
        wchar_t wcDrive[3] = {(wchar_t)letter, L':', 0};
        wchar_t wcDrivePath[MAX_PATH] = {0};
        DWORD dwSize = QueryDosDeviceW(wcDrive, wcDrivePath, MAX_PATH);
        if (dwSize == 0) {
            // case failed to query dos device path
            continue;
        }
        wchar_t* pos = wcsstr(wcInPath, wcDrivePath);
        if (pos == NULL) {
            // drive path is not contained in the input path.
            continue;
        }
        // move wchar_t pointer to snip dos device path.
        pos = reinterpret_cast<wchar_t*>(reinterpret_cast<uint64_t>(pos) +
                                         (dwSize - 1) * sizeof(wchar_t));
        wsprintf(wcOutPath, L"%s\\%s", wcDrive, pos);
        return wcOutPath;
    }
    return wcInPath;
}

void WriteDumpFile(_In_ void* lpInBuffer, _In_ uint32_t dwSize, _In_ uint64_t StartVa) {
    wchar_t wcFileName[MAX_PATH] = {0};
    HANDLE hFile = INVALID_HANDLE_VALUE;
    wsprintf(wcFileName, L"%#.16I64x.mem", StartVa);
    hFile = CreateFileW((LPWSTR)wcFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPCVOID)lpInBuffer, dwSize, &dwBytesWritten, NULL);
    wprintf(L"%lu bytes written.\n", dwBytesWritten);
    CloseHandle(hFile);
    return;
}

bool PathExistsW(_In_ LPCWSTR lpcwPath) {
    wchar_t fullPath[MAX_PATH] = {0};
    ExpandEnvironmentStringsW(lpcwPath, fullPath, MAX_PATH);
    DWORD dwFileAttrbutes = GetFileAttributesW(fullPath);
    if (dwFileAttrbutes == INVALID_FILE_ATTRIBUTES) {
        return false;
    } else {
        return true;
    }
}
}  // namespace yama

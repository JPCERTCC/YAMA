#include "resource.h"

namespace yama {

DWORD GetFileResource(_In_ HMODULE hModule, _In_ LPCWSTR lpcwResourceName, _Out_ LPVOID* lpBuffer) {
    HRSRC hResInfo = NULL;
    HGLOBAL hGlobal = NULL;
    DWORD dwSize = 0;

    hResInfo = FindResourceW(hModule, lpcwResourceName, L"FILE");
    if (!hResInfo) {
        // Resource not found.
        return dwSize;
    }
    dwSize = SizeofResource(hModule, hResInfo);
    hGlobal = LoadResource(hModule, hResInfo);

    // The pointer returned by LockResource is valid until the module containing the resource is
    // unloaded. It is not necessary to unlock resources because the system automatically deletes
    // them when the process that created them terminates.
    *lpBuffer = LockResource(hGlobal);

    return dwSize;
}

DWORD ExportFileResource(_In_ HMODULE hModule, _In_ LPCWSTR lpcwResourceName,
                         _In_ LPWSTR lpwFilePath) {
    LPVOID lpBuffer;
    DWORD dwSize = 0;
    DWORD dwBytesWritten = 0;
    BOOL bResult = FALSE;

    dwSize = GetFileResource(hModule, lpcwResourceName, &lpBuffer);
    if (dwSize == 0) {
        // Resource not found.
        return 0;
    }

    // Write into file
    HANDLE hFile = CreateFileW(lpwFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // Failed to open file handle for writing.
        return 0;
    }

    bResult = WriteFile(hFile, (LPCVOID)lpBuffer, dwSize, &dwBytesWritten, NULL);
    CloseHandle(hFile);
    if (!bResult) {
        // Failed to write file.
        return 0;
    }
    // Succeed to write file.
    return dwBytesWritten;
}

const char* LoadYaraRuleFromResource() {
    LPVOID lpBuffer = nullptr;
    DWORD dwSize = 0;
    dwSize = GetFileResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_FILE3), &lpBuffer);

    // decrypted resource buffer
    char* lpDecryptedBuffer = (char*)calloc(1, dwSize + 1);
    unsigned char key[24] = {'Y','e','t','A','n','o','t','h','e','r','M','e','m','o','r','y','A','n','a','l','y','z','e','r'};

    RC4 rc4;
    rc4.setKey(key, 24);
    rc4.crypt((char*)lpBuffer, lpDecryptedBuffer, dwSize);
    return reinterpret_cast<const char*>(lpDecryptedBuffer);
}

}  // namespace yama

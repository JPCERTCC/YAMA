#include "etw_provider.h"

namespace yama {

bool IsGuidInEtwProviders(const GUID& guid) {
    ULONG bufferSize = 0;
    GUID providerGuid = {0};
    ULONG status = ERROR_SUCCESS;
    DWORD idx = 0;
    WCHAR providerName[MAX_PATH] = {0};
    WCHAR guidString[40] = {0};
    bool inProviders = false;

    // Get the size of the buffer needed to retrieve the list of providers
    status = TdhEnumerateProviders(NULL, &bufferSize);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        LOGERROR("Failed to get the required buffer size. Error: 0x{:x}", status);
        return false;
    }

    // Allocate the buffer to retrieve the list of providers
    PVOID buffer = calloc(1, bufferSize);
    if (!buffer) {
        LOGERROR("Failed to allocate memory for the buffer. Error: 0x{:x}", GetLastError());
        return false;
    }

    // Retrieve the list of providers
    status = TdhEnumerateProviders((PPROVIDER_ENUMERATION_INFO)buffer, &bufferSize);
    if (status != ERROR_SUCCESS) {
        LOGERROR("Failed to enumerate providers. Error: 0x{:x}", status);
        free(buffer);
        return false;
    }

    // Iterate through the list of providers and check if the specified GUID exists
    PPROVIDER_ENUMERATION_INFO providerInfo = (PPROVIDER_ENUMERATION_INFO)buffer;
    for (idx = 0; idx < providerInfo->NumberOfProviders; idx++) {
        memset(providerName, 0, MAX_PATH);
        memcpy(&providerGuid, &(providerInfo->TraceProviderInfoArray[idx].ProviderGuid),
               sizeof(GUID));
        memcpy(providerName,
               (char*)providerInfo + providerInfo->TraceProviderInfoArray[idx].ProviderNameOffset,
               MAX_PATH * sizeof(WCHAR));
        if (IsEqualGUID(providerGuid, guid)) {
            StringFromGUID2(providerGuid, guidString, 40);
            LOGTRACE("ETW provider name: {}", WideCharToUtf8(providerName));
            LOGTRACE("ETW provider GUID: {}", WideCharToUtf8(guidString));
            inProviders = true;
            break;
        }
    }
    // Free the buffer
    free(buffer);
    return inProviders;
}
}  // namespace yama

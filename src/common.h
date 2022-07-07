#ifndef SRC_COMMON_H
#define SRC_COMMON_H

// Windows basic libs
#include <Windows.h>

// C standard libs
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

// Win32API libs
#include <evntprov.h>
#include <guiddef.h>
#include <psapi.h>
#include <tdh.h>
#include <tlhelp32.h>

// C++ standard libs
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <vector>

// String utils
#include "strutils.h"

// spdlog wrapper
#include "logger.h"

// Auto-generated header to record Eventlog
#include "../rsrc/eventlog/Yama.Events.h"

// Resource definitions
#include "../rsrc/resources.h"

// Linker annotations
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "tdh.lib")

// Enum for language settings

enum Language {
    EN,
    JP,
    FR,
};

#endif // SRC_COMMON_H
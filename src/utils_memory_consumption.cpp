//
// Created by omora on 2018-04-06.
//

#include "utils_memory_consumption.h"

#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)

/// windows stuff

#include "windows.h"
#include "psapi.h"

namespace memory {

    typedef unsigned __int64 ULONGLONG;
    typedef ULONGLONG DWORDLONG, *PDWORDLONG;

    float bytesToMegabytes(DWORDLONG bytes) {
            return float(bytes / pow(1024, 2));
    }

    float getTotalPhysical() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return bytesToMegabytes(memInfo.ullTotalPhys);
    }

    float getAvailiblePhysical() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return bytesToMegabytes(memInfo.ullAvailPhys);
    }

    float getTotalVirtual() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return bytesToMegabytes(memInfo.ullTotalPageFile);
    }

    float getAvailibleVirtual() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return bytesToMegabytes(memInfo.ullAvailPageFile);
    }

    float getConsumedVirtual() {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
        return bytesToMegabytes(pmc.PrivateUsage);
    }

    float getConsumedPhysical() {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
        return bytesToMegabytes(pmc.WorkingSetSize);
    }
}

//////////////////

#ifdef _WIN64
    //define something for Windows (64-bit only)
#else
//define something for Windows (32-bit only)
#endif
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif




//
// Created by omora on 2018-04-06.
//

#include "utils_memory_consumption.h"
#include <math.h>

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
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

namespace memory {

    struct sysinfo memInfo;

    float bytesToMegabytes(long long bytes) {
        return float(bytes / pow(1024, 2));
    }

    int parseLine(char* line){
        // This assumes that a digit will be found and the line ends in " Kb".
        int i = strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[i-3] = '\0';
        i = atoi(p);
        return i;
    }

    float getTotalVirtual() {
        sysinfo (&memInfo);
        long long totalVirtualMem = memInfo.totalram;
        //Add other values in next statement to avoid int overflow on right hand side...
        totalVirtualMem += memInfo.totalswap;
        totalVirtualMem *= memInfo.mem_unit;
        return bytesToMegabytes(totalVirtualMem);
    }

    float getTotalPhysical() {
        sysinfo (&memInfo);
        long long totalPhysMem = memInfo.totalram;
        //Multiply in next statement to avoid int overflow on right hand side...
        totalPhysMem *= memInfo.mem_unit;
        return bytesToMegabytes(totalPhysMem);
    }

    float getAvailibleVirtual() {
        sysinfo (&memInfo);
        long long availible = memInfo.freeram;
        //Add other values in next statement to avoid int overflow on right hand side...
        availible += memInfo.freeswap;
        availible *= memInfo.mem_unit;
        return bytesToMegabytes(availible);
    }

    float getAvailiblePhysical() {
        sysinfo (&memInfo);
        long long availible = memInfo.freeram;
        //Add other values in next statement to avoid int overflow on right hand side...
        availible *= memInfo.mem_unit;
        return bytesToMegabytes(availible);
    }

    float getConsumedVirtual(){ //Note: this value is in KB -> MB
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmSize:", 7) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return float(result / 1024);
    }

    float getConsumedPhysical(){ //Note: this value is in KB -> MB
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmRSS:", 6) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return float(result / 1024);
    }
}

#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif




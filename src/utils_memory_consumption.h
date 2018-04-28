//
// Created by omora on 2018-04-06.
//

#ifndef BIRCH_TREE_UTILS_MEMORY_CONSUMPTION_H
#define BIRCH_TREE_UTILS_MEMORY_CONSUMPTION_H

#ifdef _MSC_VER
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////
/// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366770(v=vs.85).aspx
/// http://nadeausoftware.com/articles/2012/07/c_c_tip_how_get_process_resident_set_size_physical_memory_use
//////////////////////////////////////////////////////////////////////////

namespace MEMORY {
    float getTotalPhysical();
    float getAvailablePhysical();
    float getConsumedPhysical();
//    float getTotalVirtual();
//    float getAvailableVirtual();
//    float getConsumedVirtual();
}



#endif //BIRCH_TREE_UTILS_MEMORY_CONSUMPTION_H

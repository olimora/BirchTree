cmake_minimum_required(VERSION 3.9)
project(BIRCH_tree)

set(CMAKE_CXX_STANDARD 14)

add_executable(BIRCH_tree
        src/include_basic.h
        src/include_global.h
        src/class_CF.cpp
        src/class_CF.h
        src/class_Entry.h
        src/class_Node.h
        src/class_Node.cpp
        src/class_CFTree.h
        src/class_CFTree.cpp
        src/utils_vector.cpp
        src/utils_vector.h
        src/algo_build_tree.cpp
        src/algo_build_tree.h
        src/algo_calculations.cpp
        src/algo_calculations.h
        src/algo_distance.h src/algo_distance.cpp src/algo_utils.h src/algo_utils.cpp src/utils_check_tree.h src/utils_check_tree.cpp src/utils_nodes_counts.h src/utils_nodes_counts.cpp src/utils_memory_consumption.h src/utils_memory_consumption.cpp src/algo_rebuild_tree.h src/algo_rebuild_tree.cpp src/algo_rebuild_utils.h src/algo_rebuild_utils.cpp src/utils_return_matrix.h src/utils_return_matrix.cpp)

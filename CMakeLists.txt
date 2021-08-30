cmake_minimum_required (VERSION 3.8)
set (CMAKE_C_STANDARD 99)

project (MicroVE)

option (MICROVE_BUILD_EXAMPLES "MicroVE Build Examples" ON)
option (MICROVE_BUILD_STATIC_LIB "MicroVE Build Static Library" ON)
option (MICROVE_BUILD_SHARED_LIB "MicroVE Build Shared Library" ON)

if (MICROVE_BUILD_EXAMPLES)
    add_subdirectory (examples)
endif()

add_subdirectory (src)
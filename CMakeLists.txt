cmake_minimum_required (VERSION 3.8)
set (CMAKE_C_STANDARD 99)

project (MicroVE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option (MICROVE_BUILD_EXAMPLES "MicroVE Build Examples" ON)
option (MICROVE_BUILD_STATIC_LIB "MicroVE Build Static Library" ON)
option (MICROVE_BUILD_SHARED_LIB "MicroVE Build Shared Library" ON)

if (MICROVE_BUILD_EXAMPLES)
    add_subdirectory (examples)
endif()

add_subdirectory (src)
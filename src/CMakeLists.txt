cmake_minimum_required (VERSION 3.8)

file (GLOB_RECURSE sources CONFIGURE_DEPENDS *.c *.h)

if (MICROVE_BUILD_STATIC_LIB)
    project (MicroVE_Static)

    add_library (MicroVE_Static STATIC ${sources})
endif()

if (MICROVE_BUILD_SHARED_LIB)
    project (MicroVE_Shared)

    add_library (MicroVE_Shared SHARED ${sources})
endif()
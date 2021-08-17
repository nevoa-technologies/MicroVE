cmake_minimum_required (VERSION 3.8)

project (MicroVE)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.c *.h)

add_library (MicroVE ${IMGUI_PUBLIC_HEADERS} ${sources})

if(MSVC)
  target_compile_options(MicroVE PRIVATE /W4 /WX)
else()
  target_compile_options(MicroVE PRIVATE -Wall -Wextra -Werror)
endif()
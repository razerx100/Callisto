cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp)

add_library(CallistoLib STATIC
    ${SRC}
)

target_include_directories(CallistoLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)

if(MSVC)
    target_compile_options(CallistoLib PRIVATE /fp:fast /MP /Ot /W4 /Gy)
endif()
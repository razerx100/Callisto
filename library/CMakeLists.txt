cmake_minimum_required(VERSION 3.21)

project(CallistoLib
    LANGUAGES CXX
)

file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp)

add_library(${PROJECT_NAME}
    ${SRC}
)

target_include_directories(${PROJECT_NAME} PUBLIC includes/)

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>")

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot /W4 /Gy)
endif()
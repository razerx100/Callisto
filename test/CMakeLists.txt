cmake_minimum_required(VERSION 3.21)

file(GLOB_RECURSE SRC src/*.cc)

add_executable(
    CallistoTest ${SRC}
)

include(FetchContent)
include(CTest)

# Date : 07-Jan-2024
set(GOOGLE_TEST_COMMIT_ID f8d7d77c06936315286eb55f8de22cd23c188571 CACHE STRING "Supply the latest commit ID from the GitHub repository.")

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/${GOOGLE_TEST_COMMIT_ID}.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

if(MSVC)
    target_compile_options(CallistoTest PRIVATE /fp:fast /MP /Ot /W4 /Gy /std:c++20 /Zc:__cplusplus)
endif()

target_link_libraries(CallistoTest PRIVATE CallistoLib GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(CallistoTest)

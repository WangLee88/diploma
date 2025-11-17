cmake_minimum_required(VERSION 3.8)

project(Diploma_test)

include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${INCLUDE_DIR})

set(gtest_disable_pthreads on)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()


add_executable(diploma_test tryaltest.cpp indextests.cpp servertests.cpp
    ${CMAKE_SOURCE_DIR}/src/invertedindex.cpp
    ${CMAKE_SOURCE_DIR}/src/searchserver.cpp)

target_link_libraries(diploma_test PRIVATE gtest_main)

include(GoogleTest)


gtest_discover_tests(diploma_test)



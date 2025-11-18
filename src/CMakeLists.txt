cmake_minimum_required(VERSION 3.20)


set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(nlohmann_json)

file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)

add_executable(diploma main.cpp converterjson.cpp
    invertedindex.cpp
    searchserver.cpp)


target_include_directories(diploma PUBLIC  ${INCLUDE_DIR})

target_link_libraries(diploma PRIVATE nlohmann_json::nlohmann_json)




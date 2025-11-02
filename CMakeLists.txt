cmake_minimum_required(VERSION 3.19)
project(Diploma LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()



set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(nlohmann_json)

qt_add_executable(Diploma
    main.cpp converterjson.cpp
    converterjson.h
    tests/CMakeLists
    # config.json
    # requests.json
    # answers.json
)

add_subdirectory(tests)


target_link_libraries(Diploma PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(Diploma
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS Diploma
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Diploma
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})



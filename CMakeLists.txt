cmake_minimum_required(VERSION 3.28)
project(Ballistic VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BALLISTIC_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

include(CompilerOptions)
include(Platform)

add_subdirectory(thirdparty)
add_subdirectory(engine)

add_executable(BallisticApp WIN32
    engine/application/main.cpp
    assets/ballistic.rc
)
set_target_properties(BallisticApp PROPERTIES OUTPUT_NAME "Radiance_v${BALLISTIC_VERSION}_win64")
target_link_libraries(BallisticApp PRIVATE ballistic_engine)
target_compile_definitions(BallisticApp PRIVATE BALLISTIC_VERSION_STR = "${BALLISTIC_VERSION}")

add_executable(BallisticConsole
    engine/platform/stub/console_stub.cpp
    assets/ballistic_console.rc
)
set_target_properties(BallisticConsole PROPERTIES OUTPUT_NAME "Radiance_v${BALLISTIC_VERSION}_win64_console")
target_compile_definitions(BallisticConsole PRIVATE BALLISTIC_VERSION_STR="${BALLISTIC_VERSION}")
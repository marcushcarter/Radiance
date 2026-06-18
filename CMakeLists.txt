cmake_minimum_required(VERSION 3.28)
project(Radiance VERSION 0.1.0 LANGUAGES CXX)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(RADIANCE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

include(CompilerOptions)
include(Platform)
include(EmbedAssets)

add_subdirectory(thirdparty)
add_subdirectory(engine)

set(RADIANCE_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${RADIANCE_GENERATED_DIR})

radiance_generate_asset_rc(
    ${RADIANCE_GENERATED_DIR}/embedded_assets.rc
    ${CMAKE_SOURCE_DIR}/assets/shaders
    ${CMAKE_SOURCE_DIR}/assets/textures
    ${CMAKE_SOURCE_DIR}/assets/skybox
)

add_executable(RadianceApp WIN32
    engine/application/main.cpp
    assets/icons/radiance.rc
    ${RADIANCE_GENERATED_DIR}/embedded_assets.rc
)

set_target_properties(RadianceApp PROPERTIES OUTPUT_NAME "Radiance_v${RADIANCE_VERSION}_win64")
target_link_libraries(RadianceApp PRIVATE radiance)
target_compile_definitions(RadianceApp PRIVATE RADIANCE_VERSION_STR = "${RADIANCE_VERSION}")

add_executable(RadianceConsole
    engine/platform/stub/console_stub.cpp
    assets/icons/radiance_console.rc
)
set_target_properties(RadianceConsole PROPERTIES OUTPUT_NAME "Radiance_v${RADIANCE_VERSION}_win64_console")
target_compile_definitions(RadianceConsole PRIVATE RADIANCE_VERSION_STR="${RADIANCE_VERSION}")
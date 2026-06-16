file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")

list(FILTER ENGINE_SOURCES EXCLUDE REGEX "application/main\\.cpp")
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "platform/stub/console_stub\\.cpp")

add_library(radiance STATIC ${ENGINE_SOURCES})

target_include_directories(radiance
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(radiance
    PUBLIC
        glfw
        glad
        glm
        imgui
        stb
    PRIVATE
        dwmapi
)

target_compile_definitions(radiance PUBLIC
    RADIANCE_VERSION_STR="${RADIANCE_VERSION}"
)

target_compile_options(radiance PRIVATE /wd4005)

target_precompile_headers(radiance PRIVATE pch.h)
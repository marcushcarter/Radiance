file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")

list(FILTER ENGINE_SOURCES EXCLUDE REGEX "application/main\\.cpp")
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "platform/stub/console_stub\\.cpp")

add_library(ballistic_engine STATIC ${ENGINE_SOURCES})

target_include_directories(ballistic_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ballistic_engine
    PUBLIC
        glfw
        glad
        glm
        imgui
        stb
)

target_compile_definitions(ballistic_engine PUBLIC
    BALLISTIC_VERSION_STR="${BALLISTIC_VERSION}"
)
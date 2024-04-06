project(usermode LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D UNICODE /D _UNICODE")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


if (MSVC)

    set(CMAKE_GENERATOR_TOOLSET=v143)

elseif (LINUX)

    message(FATAL_ERROR "Unsupported linux build system")

else ()

    message(FATAL_ERROR "Unsupported unix build system")

endif ()

file(GLOB_RECURSE IMGUI  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*.cpp)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES} ${IMGUI})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/lib)

target_link_libraries(${PROJECT_NAME} glfw3_mt.lib ntdll.lib OpenGL32.lib)
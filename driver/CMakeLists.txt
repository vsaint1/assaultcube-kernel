project(driver)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Both
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")

# Release
set(CMAKE_C_FLAGS_RELEASE "/Qspectre /O2 /GS- /GF -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "/Qspectre /O2 /GS- /GF -DNDEBUG")


# Debug
set(CMAKE_C_FLAGS_DEBUG "/GS /Qspectre /Od -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "/GS /Qspectre /GF /Od -D_DEBUG")

add_compile_definitions(_WIN32_WINNT=0x0A00000C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(WDK REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)

wdk_add_driver(${PROJECT_NAME} src/entry.cpp ${SOURCES})
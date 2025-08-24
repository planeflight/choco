cmake_minimum_required(VERSION 3.5)

project(choco)

# autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET (CMAKE_CXX_FLAGS "-Wall -Wextra -Wcast-qual -Wconversion-null -Wformat-security \
-Wmissing-declarations -Woverlength-strings -Wpointer-arith -Wundef \
-Wunused-local-typedefs -Wunused-result -Wvarargs -Wvla \
-Wwrite-strings -DNOMINMAX -fno-omit-frame-pointer -fPIC -g")

set(CMAKE_CXX_STANDARD 20)

include_directories("src/")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/token.cpp
    src/parser.cpp
    src/lexer.cpp
    src/ast.cpp
    src/runtime/interpreter.cpp
    src/runtime/memory.cpp
    src/runtime/runtime.cpp
    src/runtime/scope.cpp
    src/runtime/graphics.cpp
    src/util/file.cpp
    src/util/error.cpp
)

include(FetchContent)

# fmt

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(choco PRIVATE fmt::fmt)

# Raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)
target_link_libraries(choco PRIVATE raylib)


# GTEST

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/244cec869d12e53378fa0efb610cd4c32a454ec8.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    tests
    tests/test.cpp
)
target_link_libraries(
    tests 
    PRIVATE GTest::gtest_main
    PRIVATE fmt::fmt
)


include(GoogleTest)
gtest_discover_tests(tests)

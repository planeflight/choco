cmake_minimum_required(VERSION 3.5)

project(choco)

file(GLOB_RECURSE SRC
    "src/*.cpp"
    "src/*.hpp"
)

SET (CMAKE_CXX_FLAGS "-Wall -Wextra -Wcast-qual -Wconversion-null -Wformat-security \
-Wmissing-declarations -Woverlength-strings -Wpointer-arith -Wundef \
-Wunused-local-typedefs -Wunused-result -Wvarargs -Wvla \
-Wwrite-strings -DNOMINMAX -fno-omit-frame-pointer \
-std=c++20 -fPIC -g")

include_directories(".")

add_executable(${PROJECT_NAME} ${SRC})

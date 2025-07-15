project(NativeFuzzme)
cmake_minimum_required(VERSION 3.8)

link_directories(${CMAKE_SOURCE_DIR}/lib)

# Executable
# ==========

add_executable(fuzz "fuzz.c")

# 增加log
find_library(log-lib log REQUIRED)

set_property(TARGET fuzz APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-rpath=$ORIGIN")
# 增加log-lib库
target_link_libraries(fuzz blogfuzz ${log-lib})

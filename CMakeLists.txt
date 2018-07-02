
# minimum cmake version
cmake_minimum_required(VERSION 3.7.2)

# project name
project(yog-lang)

# set include directory
set(YOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${YOG_INCLUDE_DIR})

# set source directory
set(YOG_SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# create binary directory
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# set yog source code files
set(YOG_SRC ${YOG_SRC_DIR}/yog.c
            ${YOG_SRC_DIR}/config.c
            ${YOG_SRC_DIR}/errors.c
            ${YOG_SRC_DIR}/symtable.c
            ${YOG_SRC_DIR}/scanner.c)

# set c compiler debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c99 -Wall -Wextra -g")

# set c compiler release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c99 -Wall -Wextra -O2")

# yog interpreter executable
add_executable(yog ${YOG_SRC})

# yog install rule
install(TARGETS yog DESTINATION bin)


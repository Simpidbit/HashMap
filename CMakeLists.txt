cmake_minimum_required(VERSION 3.14)
project(HashMap VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
add_compile_options(-Wall -Wextra -O2)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 查找源文件
file(GLOB_RECURSE TEST_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/test/basic/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/functional/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/stl_compliance/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/performance/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/hash/*.cpp"
)
file(GLOB DEBUG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/debug/*.cpp")

# 主要库文件
set(HEADER_FILES 
    hashmap.hpp 
    utils/xxhash32.hpp 
    utils/rbtree.hpp 
    utils/bitmap.hpp 
    utils/__def.hpp 
    utils/__errs.hpp 
    utils/__iterator.hpp
)

# 测试程序目标
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${HEADER_FILES})
    add_test(NAME ${TEST_NAME}_test COMMAND ${TEST_NAME})
endforeach()

# 调试程序目标
foreach(DEBUG_SOURCE ${DEBUG_SOURCES})
    get_filename_component(DEBUG_NAME ${DEBUG_SOURCE} NAME_WE)
    add_executable(${DEBUG_NAME} ${DEBUG_SOURCE} ${HEADER_FILES})
    target_compile_options(${DEBUG_NAME} PRIVATE -g -DDEBUG)
endforeach()

# Debug 配置
option(BUILD_DEBUG "Build with debug flags" OFF)
if(BUILD_DEBUG)
    add_compile_definitions(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 测试目标
enable_testing()

# 清理目标 (类似于 make clean)
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning all build outputs"
)

# 格式化代码目标 (需要 clang-format)
add_custom_target(format
    COMMAND clang-format -i ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    COMMAND clang-format -i ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.hpp
    COMMAND clang-format -i ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
    COMMAND clang-format -i ${CMAKE_CURRENT_SOURCE_DIR}/debug/*.cpp
    COMMENT "Formatting code with clang-format"
)

# Valgrind 内存检查目标
add_custom_target(valgrind
    COMMAND valgrind --leak-check=full --show-leak-kinds=all ${CMAKE_CURRENT_SOURCE_DIR}/bin/test_hashmap
    DEPENDS test_hashmap
    COMMENT "Running memory leak check with valgrind"
)

# 文档目标 (需要 doxygen)
add_custom_target(docs
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/utils && doxygen Doxyfile
    COMMENT "Generating documentation with doxygen"
)
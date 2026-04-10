cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel")
endif()

# ============================================================================
# 内存优化设置（关键！）
# ============================================================================
# 使用 MinSizeRel 模式 (-Os) 替代 Release (-O3)
# 对于 Flash 受限的 STM32F103，大小优化比速度优化更重要

# 覆盖默认的 Release 标志（默认是 -O3）
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections")

# MinSizeRel 已经使用 -Os，额外添加链接优化
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-Wl,--gc-sections")

# 禁用未使用函数（重要优化）
add_compile_options(-ffunction-sections -fdata-sections)
add_link_options(-Wl,--gc-sections)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}}")

# Set the project name
set(CMAKE_PROJECT_NAME 9Mod_MCPBorad)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
add_subdirectory(components)
add_subdirectory(Bsp)

# 在添加 emMCP 之前设置配置文件路径
# 方式1：直接指定完整文件路径（推荐，支持任意文件名）
# 注意：这里不需要额外的引号，CMake 会自动处理
set(EMCP_USER_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/components/emMCP_config.h)

add_subdirectory(../../../emMCP emMCP)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)


# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    
    # Add user defined libraries
    components
    bsp
    emMCP
)



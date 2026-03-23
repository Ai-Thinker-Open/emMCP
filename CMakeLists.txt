# Set minimum CMake version
cmake_minimum_required(VERSION 3.22)

# Project name
add_library(emMCP INTERFACE)

# ============================================================================
# 用户配置选项
# ============================================================================

# 选项：用户可以通过 CMake 命令行或父项目设置此变量
# -DEMCP_USER_CONFIG_PATH:PATH=/path/to/your/config
#
# 或者通过 target_include_directories 在父项目中添加配置路径

# ============================================================================
# 源文件收集
# ============================================================================

file(GLOB COMPONENT_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp/*.c
${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp/cJSON/*.c
${CMAKE_CURRENT_SOURCE_DIR}/port/*.c
)
target_sources(emMCP INTERFACE ${COMPONENT_SRC_FILES})

# ============================================================================
# 包含目录设置
# ============================================================================

# ============================================================================
# 用户配置处理
# ============================================================================

# 检查用户是否指定了配置文件路径（方式1：直接指定文件）
if(DEFINED EMCP_USER_CONFIG_FILE)
    message(STATUS "emMCP: Using user config file: ${EMCP_USER_CONFIG_FILE}")
    # 将配置文件路径作为宏定义传递给编译器
    # 注意：需要确保路径被引号包围，否则 CMake 会将路径中的 '/' 解释为分隔符
    target_compile_definitions(emMCP INTERFACE 
        "EMCP_USER_CONFIG_FILE=\"${EMCP_USER_CONFIG_FILE}\""
    )
    # 同时添加配置文件所在目录到包含路径
    get_filename_component(CONFIG_DIR ${EMCP_USER_CONFIG_FILE} DIRECTORY)
    if(CONFIG_DIR)
        target_include_directories(emMCP INTERFACE ${CONFIG_DIR})
    endif()
    set(EMCP_CONFIG_PROCESSED TRUE)
endif()

# 检查用户是否指定了配置路径（方式2：指定目录，使用默认文件名）
if(NOT EMCP_CONFIG_PROCESSED AND DEFINED EMCP_USER_CONFIG_PATH)
    message(STATUS "emMCP: Using user config path: ${EMCP_USER_CONFIG_PATH}")
    target_include_directories(emMCP INTERFACE
        ${EMCP_USER_CONFIG_PATH}
    )
    # 定义 EMCP_USER_CONFIG_PATH 宏，让代码知道使用了路径方式
    target_compile_definitions(emMCP INTERFACE 
        EMCP_USER_CONFIG_PATH=1
    )
    set(EMCP_CONFIG_PROCESSED TRUE)
endif()

# ============================================================================
# 包含目录设置
# ============================================================================

target_include_directories(emMCP INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp
    ${CMAKE_CURRENT_SOURCE_DIR}/uart-mcp/cJSON
    ${CMAKE_CURRENT_SOURCE_DIR}/port
)

# ============================================================================
# 辅助函数：帮助用户设置 emMCP
# ============================================================================

# 函数：emcp_set_user_config
# 用法：在父项目的 CMakeLists.txt 中调用
# emcp_set_user_config(target_name /path/to/config/directory)
#
# 这会将配置目录添加到目标的包含路径中
function(emcp_set_user_config target config_dir)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "emcp_set_user_config: target '${target}' does not exist")
    endif()
    if(NOT EXISTS ${config_dir})
        message(FATAL_ERROR "emcp_set_user_config: config directory '${config_dir}' does not exist")
    endif()
    
    # 将配置目录添加到目标的头文件搜索路径
    target_include_directories(${target} BEFORE PRIVATE ${config_dir})
    message(STATUS "emMCP: Added config path '${config_dir}' to target '${target}'")
endfunction()

# 函数：emcp_add_definitions
# 用法：在父项目的 CMakeLists.txt 中调用
# emcp_add_definitions(target_name 
#     EMCP_USER_CONFIG_FILE=\"path/to/emMCP_port_config.h\"
# )
#
# 这会将宏定义传递给编译器，用于指定用户配置文件路径
function(emcp_add_definitions target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "emcp_add_definitions: target '${target}' does not exist")
    endif()
    
    # 获取传递给函数的所有额外参数
    set(definitions ${ARGN})
    
    if(definitions)
        target_compile_definitions(${target} PRIVATE ${definitions})
        message(STATUS "emMCP: Added definitions to target '${target}': ${definitions}")
    endif()
endfunction()

# ============================================================================
# 打印配置信息
# ============================================================================

message(STATUS "emMCP library configured")
message(STATUS "  Source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
if(DEFINED EMCP_USER_CONFIG_PATH)
    message(STATUS "  User config: ${EMCP_USER_CONFIG_PATH}")
else()
    message(STATUS "  User config: (not set)")
endif()

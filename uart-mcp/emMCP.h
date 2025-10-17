/**
 * @file emMCP.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#ifndef __EM_MCP_H__
#define __EM_MCP_H__

#include "emMCPLOG.h"
#include "cJSON.h"
#include <stdbool.h>
#include "stdint.h"
#include <string.h>

#define emMCP_VERSION "1.0.0" // emMCP版本号

#define MCP_SERVER_TOOL_NUMBLE_MAX 10			 // 工具数量
#define MCP_SERVER_TOOL_PROPERTIES_NUM 5		 // 属性数量
#define MCP_SERVER_TOOL_METHODS_NUM 5			 // 方法数量
#define MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM 5 // 方法参数数量

/**
 * @brief MCP 服务器工具类型枚举
 *
 */
typedef enum
{
	MCP_SERVER_TOOL_TYPE_FALSE = 0,
	MCP_SERVER_TOOL_TYPE_TRUE,
	MCP_SERVER_TOOL_TYPE_NULL,
	MCP_SERVER_TOOL_TYPE_NUMBER,
	MCP_SERVER_TOOL_TYPE_STRING,
	MCP_SERVER_TOOL_TYPE_ARRAY,
	MCP_SERVER_TOOL_TYPE_OBJECT,
	MCP_SERVER_TOOL_TYPE_TEXT,
	MCP_SERVER_TOOL_TYPE_BOOLEAN,
	MCP_SERVER_TOOL_TYPE_MAX,
} mcp_server_tool_type_t;

/**
 * @brief 属性结构体
 *
 */
typedef struct
{
	char *name;					 // 属性名称
	char *description;			 // 属性描述
	mcp_server_tool_type_t type; // 属性类型
} properties_t;

typedef struct
{
	char *name;					 // 参数名称
	char *description;			 // 参数描述
	mcp_server_tool_type_t type; // 参数类型
} parameters_t;
/**
 * @brief 方法结构体
 *
 */
typedef struct
{
	char *name;														 // 方法名称
	char *description;												 // 参数描述
	parameters_t parameters[MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM]; // 方法参数
} methods_t;
/**
 * @brief 输入参数结构体
 *
 */
typedef struct
{
	properties_t properties[MCP_SERVER_TOOL_PROPERTIES_NUM]; // 属性
	methods_t methods[MCP_SERVER_TOOL_METHODS_NUM];			 // 方法
} inputSchema_t;
/**
 * @brief MCP 服务器工具结构体
 *
 */
typedef struct
{
	int error_code; // 错误码
	char value[64]; // 内容
	char *type;		//  类型
} returnValues_t;
/**
 * @brief MCP 服务器工具结构体
 *
 */
typedef struct
{
	char *name;									 // 工具名称
	char *description;							 // 工具描述
	returnValues_t (*setRequestHandler)(void *); // 工具回调函数
	void (*checkRequestHandler)(void *);		 // 工具回调函数
	inputSchema_t inputSchema;					 // 输入参数
} emMCP_tool_t;

typedef struct
{
	char *emMCPVersion;
	cJSON *tools_root;
	cJSON *tools_arry;
	cJSON_Hooks *emMCP_Hooks;
	const char *tools_str;
} emMCP_t;

extern char uart_data_buf[512];
/**
 * @brief 初始化MCP服务器
 *
 * @param emMCP
 * @return int
 */
int emMCP_init(emMCP_t *emMCP);
/**
 * @brief 串口状态处理函数
 *
 */
void uartPortStateHandle(void);
/**
 * @brief 添加工具到工具列表
 *
 * @param toolsList
 * @param tool
 * @return int
 */
int emMCP_add_tool_to_toolList(emMCP_tool_t *tool);
/**
 * @brief MCP 服务器响应工具请求
 *
 * @param tool_name
 * @param arguments
 * @return returnValues_t
 */
returnValues_t emMCP_responsive_tool_request(char *tool_name, cJSON *arguments);
/**
 * @brief 获取参数,放在回调当中使用
 *
 * @param params
 * @param param_name
 * @return cJSON*
 */
cJSON *emMCP_get_param(cJSON *params, char *param_name);
/**
 * @brief 注册MCP工具到AI设备
 *
 * @return int
 */
int emMCP_registration_tools(void);
/**
 * @brief 设置波特率
 *
 * @param baudrate
 * @return int
 */
int emMCP_set_baudrate(uint16_t baudrate);
#endif // !__EM_MCP_H__
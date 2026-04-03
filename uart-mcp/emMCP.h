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

#include "cJSON.h"
#include "emMCPLOG.h"
#include "stdint.h"
#include <stdbool.h>
#include <string.h>

#define emMCP_VERSION "1.0.0" // emMCP版本号

#define MCP_SERVER_TOOL_NUMBLE_MAX 4             // 工具数量
#define MCP_SERVER_TOOL_PROPERTIES_NUM 6         // 属性数量
#define MCP_SERVER_TOOL_METHODS_NUM 5            // 方法数量
#define MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM 5 // 方法参数数量

#define emMCP_CTRL_OK "true"
#define emMCP_CTRL_ERROR "false"

#define __emMCPWeak __attribute__((weak)) // 弱函数
/**
 * @brief MCP 服务器工具类型枚举
 *
 */
typedef enum {
  MCP_SERVER_TOOL_TYPE_NONE = -1,
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

typedef enum {
  emMCP_EVENT_NONE = 0,
  emMCP_EVENT_CMD_OK,
  emMCP_EVENT_CMD_ERROR,
  emMCP_EVENT_AI_START,
  emMCP_EVENT_AI_NETCFG,
  emMCP_EVENT_AI_NETERR,
  emMCP_EVENT_AI_WIFI_CONNNECT,
  emMCP_EVENT_AI_WIFI_CONNECTED,
  emMCP_EVENT_AI_WIFI_GOT_IP,
  emMCP_EVENT_AI_WIFI_DISCONNECT,
  emMCP_EVENT_AI_WAKE,
  emMCP_EVENT_AI_SLEEP,
  emMCP_EVENT_AI_OTAUPDATE,
  emMCP_EVENT_AI_OTAOK,
  emMCP_EVENT_AI_OTAERR,
  emMCP_EVENT_AI_MCP_CMD,
  emMCP_EVENT_AI_MCP_Text,
  emMCP_EVENT_AI_MCP_CHECK,  // MCP 检查命令
} emMCP_event_t;
/**
 * @brief 属性结构体
 *
 */
typedef struct {
  char *name;                  // 属性名称
  char *description;           // 属性描述
  mcp_server_tool_type_t type; // 属性类型
} properties_t;

typedef struct {
  char *name;                  // 参数名称
  char *description;           // 参数描述
  mcp_server_tool_type_t type; // 参数类型
} parameters_t;
/**
 * @brief 方法结构体
 *
 */
typedef struct {
  char *name;                                                      // 方法名称
  char *description;                                               // 参数描述
  parameters_t parameters[MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM]; // 方法参数
} methods_t;
/**
 * @brief 输入参数结构体
 *
 */
typedef struct {
  properties_t properties[MCP_SERVER_TOOL_PROPERTIES_NUM]; // 属性
  methods_t methods[MCP_SERVER_TOOL_METHODS_NUM];          // 方法
} inputSchema_t;

/**
 * @brief MCP 服务器工具结构体
 *
 */
typedef struct emMCP_tool {
  char *name;                          // 工具名称
  char *description;                   // 工具描述
  void (*setRequestHandler)(void *);   // 工具回调函数
  void (*checkRequestHandler)(void *); // 工具回调函数
  inputSchema_t inputSchema;           // 输入参数
  struct emMCP_tool *next;             // 下一个工具
} emMCP_tool_t;
/**
 * @brief emMCP 结构体
 *
 */
typedef struct {
  char *emMCPVersion;
  cJSON *tools_root;
  cJSON *tools_arry;
  cJSON_Hooks *emMCP_Hooks;
  char *tools_str;
  bool isUartRecv;
  void (*emMCPEventCallback)(emMCP_event_t, mcp_server_tool_type_t, void *);
} emMCP_t;

typedef struct {
  const char *str;     // 字符串常量（Flash存储）
  uint8_t len;         // 字符串长度（提前计算，避免运行时strlen）
  emMCP_event_t event; // 对应事件
  bool is_prefix;      // 是否为前缀匹配（仅用于"ERROR"）
} status_map_entry_t;

/**
 * @brief emMCP 串口数据缓存
 *
 */
extern char *uart_data_buf;
/**
 * @brief 初始化MCP服务器
 *
 * @param emMCP
 * @return int
 */
int emMCP_Init(emMCP_t *emMCP);
/**
 * @brief MCP 心跳处理函数
 * @brief 需要循环调用
 *
 */
void emMCP_TickHandle(int delay_ms);
/**
 * @brief 添加工具到工具列表
 *
 * @param toolsList
 * @param tool
 * @return int
 */
int emMCP_AddToolToToolList(emMCP_tool_t *tool);
/**
 * @brief 获取参数,放在回调当中使用
 *
 * @param params
 * @param param_name
 * @return cJSON*
 */
cJSON *emMCP_GetParam(cJSON *params, char *param_name);
/**
 * @brief 注册MCP工具到AI设备
 *
 * @return int
 */
int emMCP_RegistrationTools(void);
/**
 * @brief 设置波特率
 *
 * @param baudrate
 * @return int
 */
int emMCP_SetBaudrate(uint16_t baudrate);
/**
 * @brief 唤醒模组并设置唤醒时间
 *
 * @param WakeUp_Time
 * @return int
 */
int emMCP_SetAiWakeUp(uint8_t WakeUp_Time);
/**
 * @brief 设置AI音量
 *
 * @param volume
 * @return int
 */
int emMCP_SetAiVolume(uint8_t volume);
/**
 * @brief 获取AI音量
 *
 * @return uint8_t
 */
uint8_t emMCP_CheckAiVolume(void);
/**
 * @brief 检查串口发送状态
 *
 * @return int
 */
int emMCP_CheckUartSendStatus(void);
/**
 * @brief 更新串口接收状态
 *
 * @param isRecv
 */
void emMCP_UpdateUartRecv(bool isRecv);
/**
 * @brief 响应控制结果
 *
 * @param value
 * @return int
 */
int emMCP_ResponseValue(char *value);
#endif // !__EM_MCP_H__
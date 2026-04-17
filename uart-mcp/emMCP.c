/**
 * @file emMCP.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include "emMCP.h"
#include "log.h"
#include "uartPort.h"
#include <stddef.h>
#include <stdio.h>

#define DESCRIOTION "description"
#define METHODS "methods"

/**
 * @brief emMCP 链表工具，用来简单管理工具
 *
 */
emMCP_tool_t mcp_tool_arry[MCP_SERVER_TOOL_NUMBLE_MAX];
/**
 * @brief emMCP 服务器类型
 *
 */
emMCP_event_t emMCP_event = emMCP_EVENT_NONE;
/**
 * @brief emMCP JSON类型字符串
 *
 */
static char *mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_MAX] = {
    "true", "false", "null", "number", "string",
    "array", "object", "text", "boolean"};
/**
 * @brief emMCP 日志等级
 *
 */
emMCP_LogLevel log_level = emMCP_LOG_LEVEL_DEBUG;
/**
 * @brief emMCP 串口数据缓存区大小
 *
 */
/**
 * @brief emMCP 状态映射表
 *
 * 该数组用于将AI设备返回的字符串状态映射到对应的emMCP事件，
 * 用于事件的快速匹配和转换。高频场景优先放在前面，减少比较次数。
 */
// 内存优化：精简状态映射表，仅保留核心功能
// 用户可通过定义 EMCP_FULL_STATUS_MAP 启用完整功能
#ifndef EMCP_FULL_STATUS_MAP
static const status_map_entry_t status_map[] = {
    // 核心功能（约节省 50% Flash）
    {"OK", 2, emMCP_EVENT_CMD_OK, false},                           // 命令执行成功
    {"ERROR", 5, emMCP_EVENT_CMD_ERROR, true},                      // 命令执行错误
    {"AI Start", 8, emMCP_EVENT_AI_START, false},                   // AI设备启动完成
    {"2.WakeUP", 8, emMCP_EVENT_AI_WAKE, false},                    // AI设备唤醒
    {"3.Sleep", 6, emMCP_EVENT_AI_SLEEP, false},                    // AI设备进入睡眠
    {NULL, 0, emMCP_EVENT_NONE, false}                              // 结束标记
};
#else
static const status_map_entry_t status_map[] = {
    // 完整功能
    {"OK", 2, emMCP_EVENT_CMD_OK, false},
    {"ERROR", 5, emMCP_EVENT_CMD_ERROR, true},
    {"AI Start", 8, emMCP_EVENT_AI_START, false},
    {"WIFI_CONNECTED", 12, emMCP_EVENT_AI_WIFI_CONNECTED, false},
    {"WIFI_GOT_IP", 11, emMCP_EVENT_AI_WIFI_GOT_IP, false},
    {"1.WiFi connect OK", 16, emMCP_EVENT_AI_WIFI_CONNNECT, false},
    {"2.WakeUP", 8, emMCP_EVENT_AI_WAKE, false},
    {"3.Sleep", 6, emMCP_EVENT_AI_SLEEP, false},
    {"4.NetCFG", 7, emMCP_EVENT_AI_NETCFG, false},
    {"5.NetERR", 7, emMCP_EVENT_AI_NETERR, false},
    {"6.OTAUPDATE", 10, emMCP_EVENT_AI_OTAUPDATE, false},
    {"7.OTA OK", 7, emMCP_EVENT_AI_OTAOK, false},
    {"8.OTA ERR", 8, emMCP_EVENT_AI_OTAERR, false},
    {NULL, 0, emMCP_EVENT_NONE, false}
};
#endif
/**
 * @brief emMCP 串口数据缓存区
 *
 */
char *uart_data_buf = NULL;

/**
 * @brief emMCP 工具注册标志
 *
 */
emMCP_t *emMCP_dev = NULL;
/**
 * @brief emMCP 延时时间变量，用于延时函数
 *
 */
static int delay_time = 10;
/**
 * @brief emMCP 小安AI 音量值
 *
 */
static uint8_t emMCP_AiVolume = 0;
/**
 * @brief 函数声明区
 */
static emMCP_event_t emMCP_ReturnEvent(mcp_server_tool_type_t *param_type,
                                       char *text_param);
/**
 * @brief 事件回调函数
 *
 * @param event
 * @param param
 * @return __emMCPWeak
 */
__emMCPWeak void emMCP_EventCallback(emMCP_event_t event,
                                     mcp_server_tool_type_t type, void *param)
{
  char *param_str = (char *)param;
  emMCP_log_debug("emMCP_EventCallback: event:%d,type:%d,param:%s", event, type,
                  param_str);
}
/**
 * @brief emMCP
 * 设置回调函数提醒，如果你没有设置回调函数，该工具的检查函数都会调用这个函数
 *
 * @param arg
 * @return __emMCPWeak
 */
static void emMCP_Set_CMDCallback(void *arg)
{
  emMCP_log_warn("Please set the callback function");
  emMCP_ResponseValue(emMCP_CTRL_ERROR);
}
/**
 * @brief emMCP
 * 检查回调函数提醒，如果你没有设置回调函数，该工具的检查函数都会调用这个函数
 *
 * @param arg
 */
static void emMCP_check_CMDCallback(void *arg)
{
  emMCP_log_warn("Please set the callback function");
  emMCP_ResponseValue(emMCP_CTRL_ERROR);
}
/**
 * @brief emMCP 初始化
 *
 * @param emMCP
 * @return int
 */
/**
 * @brief emMCP 初始化函数
 * @details 该函数是emMCP库的核心初始化函数，用于初始化emMCP设备结构体，
 *          设置版本号，创建工具根对象和数组，以及初始化事件回调函数。
 *          在使用emMCP库的其他功能之前，必须先调用此函数。
 *
 * @param emMCP 指向emMCP_t结构体的指针，用于存储emMCP库的配置和状态信息
 * @return int 初始化结果
 *         - 0：初始化成功
 *         - -1：初始化失败（参数为NULL）
 */
int emMCP_Init(emMCP_t *emMCP)
{
  // 参数检查：确保传入的emMCP指针不为NULL
  if (emMCP == NULL)
  {
    emMCP_log_error("emMCP_init: emMCP is NULL"); // 记录错误日志
    return -1;                                    // 返回初始化失败
  }

  // 将传入的emMCP指针赋值给全局变量，方便其他函数访问
  emMCP_dev = emMCP;

  // 版本号设置：如果emMCP结构体中的版本号为NULL，则使用默认版本号
  if (emMCP_dev->emMCPVersion == NULL)
    emMCP_dev->emMCPVersion = emMCP_VERSION;

  // 工具根对象和数组初始化：如果尚未创建，则创建JSON对象和数组
  if (emMCP_dev->tools_root == NULL)
  {
    emMCP_dev->tools_root = cJSON_CreateObject(); // 创建工具根JSON对象
    emMCP_dev->tools_arry = cJSON_CreateArray();  // 创建工具列表JSON数组
    // 将工具数组添加到工具根对象中，键名为"tools"
    cJSON_AddItemToObject(emMCP_dev->tools_root, "tools", emMCP_dev->tools_arry);
  }

  // 初始化事件回调函数：设置默认的事件处理函数
  emMCP_dev->emMCPEventCallback = emMCP_EventCallback;

  return 0; // 返回初始化成功
}
/**
 * @brief 添加工具到工具列表
 *
 * @param toolsList
 * @param tool
 * @return int
 */
/**
 * @brief emMCP 添加工具到工具列表
 *
 * 该函数用于将工具对象添加到emMCP系统的工具列表中，包括内存中存储和JSON格式的工具描述
 *
 * @param tool 要添加的工具对象指针，包含工具的名称、描述、属性和方法等信息
 * @return int 成功返回0，失败返回-32604
 */
int emMCP_AddToolToToolList(emMCP_tool_t *tool)
{
  // 检查工具对象和工具列表是否为空
  if (tool == NULL || emMCP_dev->tools_arry == NULL)
  {
    emMCP_log_error("tool or toolsList is NULL");
    return -32604; // 参数错误，返回失败
  }

  emMCP_tool_t *tmp_tool = tool; // 临时工具对象指针，方便操作

  // 设置默认的请求处理回调函数（如果未提供）
  if (tmp_tool->setRequestHandler == NULL)
    tmp_tool->setRequestHandler = emMCP_Set_CMDCallback; // 设置默认的命令回调函数
  if (tmp_tool->checkRequestHandler == NULL)
    tmp_tool->checkRequestHandler = emMCP_check_CMDCallback; // 设置默认的命令检查回调函数

  // 将工具信息存储到内存数组中
  int tool_added = 0;
  if (mcp_tool_arry[0].name == NULL) // 检查工具数组是否为空
  {
    memcpy(&mcp_tool_arry[0], tmp_tool, sizeof(emMCP_tool_t)); // 复制到第一个位置
    tool_added = 1;
  }
  else // 工具数组不为空
  {
    // 查找数组中的空位置
    for (int i = 0; i < MCP_SERVER_TOOL_NUMBLE_MAX; i++)
    {
      if (mcp_tool_arry[i].name == NULL) // 找到空位置
      {
        memcpy(&mcp_tool_arry[i], tmp_tool, sizeof(emMCP_tool_t)); // 复制工具信息到空位置
        tool_added = 1;
        break;
      }
    }
  }
  // 检查是否成功添加工具（数组已满）
  if (!tool_added)
  {
    emMCP_log_error("Tool list is full, max %d tools", MCP_SERVER_TOOL_NUMBLE_MAX);
    return -32604; // 工具列表已满
  }

  // 创建JSON格式的工具对象
  cJSON *json_tool = cJSON_CreateObject();
  if (json_tool == NULL) // 检查JSON对象是否创建成功
  {
    emMCP_log_error("json_tool is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX); // 清空工具数组
    return -32604;                                                               // 创建失败，返回错误
  }

  cJSON_bool json_ret;
  // 将JSON工具对象添加到工具列表数组中
  if (emMCP_dev != NULL && emMCP_dev->tools_arry != NULL)
  {
    json_ret = cJSON_AddItemToArray(emMCP_dev->tools_arry, json_tool);
  }
  else // 检查工具设备或工具列表是否为空
  {
    emMCP_log_error("emMCP_dev or tools_arry is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX); // 清空工具数组
    return -32604;                                                               // 参数错误，返回失败
  }

  // 检查JSON对象添加是否成功
  if (json_ret == -1)
  {
    emMCP_log_error("json_toolsList add json_tool failed");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX); // 清空工具数组
    return -32604;                                                               // 添加失败，返回错误
  }

  // 添加工具名称到JSON对象
  if (tmp_tool->name != NULL)
  {
    cJSON_AddStringToObject(json_tool, "name", tmp_tool->name);
  }
  else // 工具名称为空
  {
    emMCP_log_error("tool name is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX); // 清空工具数组
    return -32604;                                                               // 参数错误，返回失败
  }

  // 添加工具描述到JSON对象
  cJSON_AddStringToObject(json_tool, DESCRIOTION, tmp_tool->description);

  // 创建inputSchema对象，用于描述工具的输入格式
  cJSON *inputSchema = cJSON_CreateObject();
  cJSON_AddItemToObject(json_tool, "inputSchema", inputSchema);

  // 添加properties参数到inputSchema
  cJSON *properties = cJSON_CreateObject();
  uint8_t properties_cnt = 0; // 属性计数器

  // 检查是否有属性需要添加
  if (sizeof(tmp_tool->inputSchema.properties) / sizeof(properties_t) > 0)
  {
    // 遍历所有属性
    for (properties_cnt = 0; properties_cnt < MCP_SERVER_TOOL_PROPERTIES_NUM; properties_cnt++)
    {
      // 检查属性名称是否为空
      if (tmp_tool->inputSchema.properties[properties_cnt].name != NULL)
      {
        // 创建单个属性的JSON对象
        cJSON *prop = cJSON_CreateObject();

        // 添加属性描述
        cJSON_AddStringToObject(prop, DESCRIOTION, tmp_tool->inputSchema.properties[properties_cnt].description);

        // 添加属性类型
        cJSON_AddStringToObject(prop, "type", mcp_sever_type_str[tmp_tool->inputSchema.properties[properties_cnt].type]);

        // 将属性添加到properties对象中
        cJSON_AddItemToObject(properties, tmp_tool->inputSchema.properties[properties_cnt].name, prop);
      }
      else
      {
        break; // 属性名称为空，跳出循环
      }
    }
  }

  // 如果有有效属性，将properties对象添加到inputSchema
  if (properties_cnt > 0)
    cJSON_AddItemToObject(inputSchema, "properties", properties);

  // 添加methods参数到inputSchema
  cJSON *methods = cJSON_CreateObject();
  uint8_t methods_num = 0; // 方法计数器

  // 检查是否有方法需要添加
  if (sizeof(tmp_tool->inputSchema.methods) / sizeof(methods_t) > 0)
  {
    // 遍历所有方法
    for (methods_num = 0; methods_num < MCP_SERVER_TOOL_METHODS_NUM; methods_num++)
    {
      // 检查方法名称是否为空
      if (tmp_tool->inputSchema.methods[methods_num].name != NULL)
      {
        // 创建单个方法的JSON对象
        cJSON *method = cJSON_CreateObject();

        // 将方法添加到methods对象中
        cJSON_AddItemToObject(methods, tmp_tool->inputSchema.methods[methods_num].name, method);

        // 添加方法描述
        cJSON_AddStringToObject(method, DESCRIOTION, tmp_tool->inputSchema.methods[methods_num].description);

        // 检查方法是否有参数
        if (sizeof(tmp_tool->inputSchema.methods[methods_num].parameters) / sizeof(parameters_t) > 0)
        {
          // 创建参数对象
          cJSON *parameters = cJSON_CreateObject();
          cJSON_AddItemToObject(method, "parameters", parameters); // 将参数对象添加到方法中

          // 遍历所有参数
          for (size_t i = 0; i < MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM; i++)
          {
            // 检查参数名是否存在
            if (tmp_tool->inputSchema.methods[i].parameters[i].name != NULL)
            {
              // 创建单个参数对象
              cJSON *param = cJSON_CreateObject();

              // 将参数添加到参数集合中
              cJSON_AddItemToObject(parameters, tmp_tool->inputSchema.methods[i].parameters[i].name, param);

              // 添加参数描述
              cJSON_AddStringToObject(param, DESCRIOTION, tmp_tool->inputSchema.methods[i].parameters[i].description);

              // 添加参数类型
              cJSON_AddStringToObject(param, "type", mcp_sever_type_str[tmp_tool->inputSchema.methods[i].parameters[i].type]);
            }
          }
        }
      }
      else
      {
        break; // 方法名不存在，跳出循环
      }
    }
  }

  // 如果有有效方法，将methods对象添加到inputSchema
  if (methods_num > 0)
    cJSON_AddItemToObject(inputSchema, METHODS, methods);

  return 0; // 添加成功，返回0
}

/**
 * @brief Construct a new mcp server responsive tool request object
 * 		// 根据工具名称，找到对应的工具，并执行对应的请求
 *
 * @param tool_name 工具名称
 * @param arguments 参数
 * @return void
 */
static void
emMCP_ResponsiveToolRequest(char *tool_name, cJSON *arguments)
{ // 根据工具名称响应工具请求

  if (tool_name == NULL || arguments == NULL)
  {                                                    // 检查工具名称和参数是否为空
    emMCP_log_error("tool_name or arguments is NULL"); // 记录错误日志

    return; // 返回
  }
  // 开始定位工具
  uint8_t tools_numble = 0; // 工具编号计数器
  for (tools_numble = 0;
       tools_numble < MCP_SERVER_TOOL_NUMBLE_MAX; // 遍历工具数组
       tools_numble++)
  {
    if (mcp_tool_arry[tools_numble].name != NULL && // 检查工具名是否存在且匹配
        strcmp(mcp_tool_arry[tools_numble].name, tool_name) == 0)
    {
      break; // 找到匹配的工具，跳出循环
    }
  }

  // 检查是否找到匹配的工具，防止数组越界
  if (tools_numble >= MCP_SERVER_TOOL_NUMBLE_MAX)
  {
    emMCP_log_error("Tool not found: %s", tool_name); // 记录错误日志
    return;                                           // 返回
  }

  if (strcmp(mcp_tool_arry[tools_numble].name, tool_name) == 0)
  { // 确认找到了匹配的工具
    // 判断 arguments 参数是否为 NULL 或空对象
    if (arguments == NULL || (arguments->type == cJSON_Object && arguments->child == NULL))
    {                                                             // 如果参数为空或空对象
      mcp_tool_arry[tools_numble].checkRequestHandler(arguments); // 调用检查请求处理器
      return;                                                     // 返回
    }
    // 判断参数值是否为 null（如 {"sht3x.read":null}）
    if (arguments->child != NULL && arguments->child->type == cJSON_NULL)
    {
      mcp_tool_arry[tools_numble].checkRequestHandler(arguments); // 调用检查请求处理器
      return;
    }
    //  判断是否为 methods 参数
    cJSON *methods_item = cJSON_GetObjectItemCaseSensitive(arguments, METHODS);
    if (methods_item != NULL)
    {                                                // 检查是否有methods参数
      mcp_tool_arry[tools_numble].setRequestHandler( // 调用设置请求处理器
          methods_item);
    }
    else
    {
      mcp_tool_arry[tools_numble].setRequestHandler(arguments); // 调用设置请求处理器
    }
    return; // 返回
  }
}

/**
 * @brief 根据参数名称，获取参数
 *
 * @param params 参数对象
 * @param param_name 参数名称
 * @return cJSON* 参数对象指针
 */
cJSON *emMCP_GetParam(cJSON *params,
                      char *param_name)
{ // 根据参数名称获取参数值
  if (params == NULL || param_name == NULL)
  {              // 检查参数是否有效
    return NULL; // 返回空指针
  }
  return cJSON_GetObjectItemCaseSensitive(
      params, param_name); // 获取指定参数（区分大小写）
}

/**
 * @brief 检查UART数据是否发送成功
 *
 * @return int 成功返回1，失败返回0
 */
int emMCP_CheckUartSendStatus(void)
{                                                   // 检查串口发送状态
  return emMCP_event == emMCP_EVENT_CMD_OK ? 1 : 0; // 如果事件为命令成功则返回1，否则返回0
}

/**
 * @brief emMCP注册工具到AI设备
 *
 * @return int 成功返回0，失败返回-1
 */
int emMCP_RegistrationTools(void)
{ // 向AI设备注册工具
  if (emMCP_dev == NULL ||
      emMCP_dev->tools_root == NULL || // 检查设备及工具根对象是否有效
      emMCP_dev->tools_arry == NULL)
  {
    emMCP_log_error("emMCP_dev or tools is NULL"); // 记录错误日志
    return -1;                                     // 返回错误码
  }
  emMCP_dev->tools_str = cJSON_PrintUnformatted(emMCP_dev->tools_root);
  if (emMCP_dev->tools_str == NULL)
  {
    emMCP_log_error("Failed to print tools JSON");
    cJSON_Delete(emMCP_dev->tools_root);
    emMCP_dev->tools_root = NULL;
    return -1;
  }

  size_t tools_str_len = strlen(emMCP_dev->tools_str);
  size_t cmd_buf_size = tools_str_len + 64;
  char *cmd = emMCP_malloc(cmd_buf_size);

  if (cmd != NULL)
  {
    memset(cmd, 0, cmd_buf_size);
    int written = snprintf(cmd, cmd_buf_size, "mcp-tool {\"role\":\"MCU\",\"msgType\":\"MCP\",\"data\":%s}\r\n", emMCP_dev->tools_str);
    if (written > 0 && written < cmd_buf_size)
    { // 检查是否完整写入
      uartPortSendData(cmd, strlen(cmd));
    }
    else
    {
      emMCP_log_error("Command buffer too small");
    }
    emMCP_free(cmd);
  }
  else
  {
    emMCP_log_error("Failed to allocate memory for cmd buffer");
  }

  cJSON_free(emMCP_dev->tools_str);
  emMCP_dev->tools_str = NULL;
  cJSON_Delete(emMCP_dev->tools_root);
  emMCP_dev->tools_root = NULL;        // 将工具根对象设为空
  emMCP_dev->isUartRecv = false;       // 重置串口接收标志
  return 0;                            // 返回成功
}

/**
 * @brief 根据串口数据，返回事件类型
 *
 * @param param_type 参数类型指针
 * @param text_param 文本参数指针
 * @return emMCP_event_t 事件类型
 */
static emMCP_event_t emMCP_ReturnEvent(
    mcp_server_tool_type_t *param_type, // 解析串口数据并返回事件类型
    char *text_param)
{
  // 检查串口数据是否为0

  if (strlen(uart_data_buf) == 0)
  { // 检查串口数据缓冲区是否为空

    return emMCP_EVENT_NONE; // 返回无事件
  }
  // 检查串口数据是否为json格式
  cJSON *root = cJSON_Parse(uart_data_buf); // 解析串口数据为JSON对象
  if (root == NULL)
  {                          // 检查解析是否成功
    return emMCP_EVENT_NONE; // 返回无事件
  }
  // 检查串口数据是否为AI发送的数据
  cJSON *role = cJSON_GetObjectItemCaseSensitive(root, "role"); // 获取角色字段
  if (role == NULL ||
      strcmp(role->valuestring, "AI board") != 0)
  {                          // 检查是否为AI板发送
    cJSON_Delete(root);      // 删除JSON对象
    return emMCP_EVENT_NONE; // 返回无事件
  }
  cJSON *msgType = cJSON_GetObjectItemCaseSensitive(root, "msgType"); // 获取消息类型字段
  cJSON *msgType_param = NULL;                           // 消息参数指针
  // 消息类型为status
  if (msgType != NULL &&
      strcmp(msgType->valuestring, "status") == 0)
  {                                            // 如果是状态消息
    *param_type = MCP_SERVER_TOOL_TYPE_STRING; // 设置参数类型为字符串
    msgType_param =
        cJSON_GetObjectItemCaseSensitive(root, "data"); // 获取数据字段
    if (msgType_param != NULL)
    { // 检查数据字段是否存在
      const char *__restrict status_str =
          msgType_param->valuestring;               // 获取状态字符串
      emMCP_event = emMCP_EVENT_NONE;               // 重置事件为无事件
      const uint8_t input_len = strlen(status_str); // 获取状态字符串长度
      for (uint8_t i = 0; status_map[i].str != NULL; i++)
      {                                                   // 遍历状态映射表
        const status_map_entry_t *entry = &status_map[i]; // 获取当前状态条目
        if (!entry->is_prefix &&
            input_len < entry->len)
        {           // 检查是否为前缀匹配且长度不足
          continue; // 继续下一个条目
        }
        if (entry->is_prefix)
        { // 如果是前缀匹配
          if (strncmp(status_str, entry->str, entry->len) == 0)
          {                             // 比较前缀
            emMCP_event = entry->event; // 设置对应事件
            break;                      // 跳出循环
          }
        }
        else if (strncmp(status_str, entry->str, entry->len) ==
                 0)
        {                             // 如果是完全匹配
          emMCP_event = entry->event; // 设置对应事件
          break;                      // 跳出循环
        }
      }
      if (emMCP_event == emMCP_EVENT_CMD_OK)
      { // 如果是命令成功事件
        cJSON *status_parm =
            cJSON_GetObjectItemCaseSensitive(root, "volume"); // 获取音量参数
        if (status_parm != NULL &&
            cJSON_IsNumber(status_parm))
        {                                         // 检查音量参数是否为数字
          emMCP_AiVolume = status_parm->valueint; // 保存音量值
        }
      }
    }
  }
  else if (msgType != NULL && strcmp(msgType->valuestring, "mcp_set") == 0)
  {                                            // 如果是MCP设置消息
    emMCP_event = emMCP_EVENT_AI_MCP_CMD;      // 设置事件为MCP命令
    *param_type = MCP_SERVER_TOOL_TYPE_OBJECT; // 设置参数类型为对象
    cJSON *mcp_toolsname =
        cJSON_GetObjectItemCaseSensitive(root, "tools"); // 获取工具名
    if (mcp_toolsname == NULL ||
        mcp_toolsname->type != cJSON_String)
    {                          // 检查工具名是否为字符串
      cJSON_Delete(root);      // 删除JSON对象
      return emMCP_EVENT_NONE; // 返回无事件
    }
    msgType_param =
        cJSON_GetObjectItemCaseSensitive(root, "data"); // 获取数据参数
    if (msgType_param == NULL ||
        msgType_param->type != cJSON_Object)
    {                          // 检查数据参数是否为对象
      cJSON_Delete(root);      // 删除JSON对象
      return emMCP_EVENT_NONE; // 返回无事件
    }
    // 从MCP 工具中解析出参数
    // cJSON *param = cJSON_GetObjectItemCaseSensitive(msgType_param, "params");
    // if (param != NULL && param->type == cJSON_Object)
    // {
    //   cJSON *mcp_tool_name = cJSON_GetObjectItemCaseSensitive(param, "name");
    //   cJSON *arguments = cJSON_GetObjectItemCaseSensitive(param,
    //   "arguments"); if (arguments != NULL && arguments->type == cJSON_Object
    //   &&
    //       mcp_tool_name != NULL)
    //   {
    // 处理MCP工具
    emMCP_ResponsiveToolRequest(mcp_toolsname->valuestring,
                                msgType_param); // 响应工具请求
    //   }
    // }
    cJSON_Delete(root);
    return emMCP_EVENT_NONE; // 已内部处理，不触发 EventCallback
  }
  else if (msgType != NULL && strcmp(msgType->valuestring, "mcp_check") == 0)
  {                                               // 如果是MCP检查消息
    emMCP_event = emMCP_EVENT_AI_MCP_CHECK;       // 设置事件为MCP检查命令
    *param_type = MCP_SERVER_TOOL_TYPE_OBJECT;    // 设置参数类型为对象
    cJSON *mcp_toolsname =
        cJSON_GetObjectItemCaseSensitive(root, "tools"); // 获取工具名
    if (mcp_toolsname == NULL ||
        mcp_toolsname->type != cJSON_String)
    {                          // 检查工具名是否为字符串
      cJSON_Delete(root);      // 删除JSON对象
      return emMCP_EVENT_NONE; // 返回无事件
    }
    msgType_param =
        cJSON_GetObjectItemCaseSensitive(root, "data"); // 获取数据参数
    if (msgType_param == NULL ||
        msgType_param->type != cJSON_Object)
    {                          // 检查数据参数是否为对象
      cJSON_Delete(root);      // 删除JSON对象
      return emMCP_EVENT_NONE; // 返回无事件
    }
    // 处理MCP检查工具请求
    emMCP_ResponsiveToolRequest(mcp_toolsname->valuestring,
                                msgType_param); // 响应工具请求
    cJSON_Delete(root);
    return emMCP_EVENT_NONE; // 已内部处理，不触发 EventCallback
  }
  else if (msgType != NULL && strcmp(msgType->valuestring, "MCP Text") == 0)
  {                                          // 如果是MCP文本消息
    emMCP_event = emMCP_EVENT_AI_MCP_Text;   // 设置事件为MCP文本
    *param_type = MCP_SERVER_TOOL_TYPE_TEXT; // 设置参数类型为文本
    msgType_param =
        cJSON_GetObjectItemCaseSensitive(root, "data"); // 获取数据参数
    if (msgType_param == NULL ||
        msgType_param->type != cJSON_Object)
    {                          // 检查数据参数是否为对象
      cJSON_Delete(root);      // 删除JSON对象
      return emMCP_EVENT_NONE; // 返回无事件
    }
  }
  if (emMCP_event != emMCP_EVENT_NONE && text_param != NULL)
  { // 如果有事件发生且缓冲区有效

    if (*param_type == MCP_SERVER_TOOL_TYPE_STRING && msgType_param != NULL && msgType_param->valuestring != NULL)
    {                                                       // 如果参数类型为字符串
      strncpy(text_param, msgType_param->valuestring, 255); // 复制字符串值
    }
    else if (msgType_param != NULL)
    { // 否则
      char *param_str =
          cJSON_PrintUnformatted(msgType_param); // 将参数转为字符串
      if (param_str != NULL)
      {
        strncpy(text_param, param_str, 255); // 复制参数字符串
        cJSON_free(param_str);               // 释放参数字符串内存
      }
    }
    text_param[255] = '\0'; // 确保字符串结束
  }
  cJSON_Delete(root); // 删除JSON对象
  return emMCP_event; // 返回事件类型
}

/**
 * @brief emMCP 更新串口接收状态
 *
 * @param isRecv 接收状态
 */
void emMCP_UpdateUartRecv(bool isRecv)
{
  if (emMCP_dev != NULL)
  {
    emMCP_dev->isUartRecv = isRecv;
  }
} // 更新串口接收状态标志

/**
 * @brief emMCP 循环处理函数
 *
 * @param delay_ms 延时时间
 */
// 内存优化：调试变量可通过定义 EMCP_ENABLE_DEBUG_VARS 启用
#ifdef EMCP_ENABLE_DEBUG_VARS
volatile uint32_t g_emMCP_tick_count = 0;
volatile uint32_t g_emMCP_process_count = 0;
#endif

void emMCP_TickHandle(int delay_ms)
{ // emMCP主循环处理函数
#ifdef EMCP_ENABLE_DEBUG_VARS
  g_emMCP_tick_count++;
#endif
  
  if (emMCP_dev == NULL ||
      emMCP_dev->tools_arry == NULL || // 检查设备、工具数组和回调函数是否有效
      emMCP_dev->emMCPEventCallback == NULL)
  {
    emMCP_log_error("emMCP Partial parameter is NULL"); // 记录错误日志
    return;                                             // 返回
  }
  if (delay_ms != delay_time)
  {                        // 如果延时时间发生变化
    delay_time = delay_ms; // 更新延时时间
  }
  
  // 检查是否有新数据（从中断安全的双缓冲区获取）
  char *rx_data = uartPortGetRxData();
  if (rx_data != NULL)
  {
    // 将数据复制到 uart_data_buf 进行解析
    size_t data_len = strlen(rx_data);
    if (uart_data_buf != NULL)
    {
      emMCP_free(uart_data_buf);
      uart_data_buf = NULL;
    }
    uart_data_buf = (char *)emMCP_malloc(data_len + 1);
    if (uart_data_buf != NULL)
    {
      memcpy(uart_data_buf, rx_data, data_len + 1);
      emMCP_dev->isUartRecv = 1;
    }
    // 标记数据已处理，允许中断接收新数据
    uartPortClearRxData();
  }
  
  if (emMCP_dev->isUartRecv)
  { // 如果串口有数据接收
    mcp_server_tool_type_t _param_type =
        MCP_SERVER_TOOL_TYPE_STRING;            // 初始化参数类型
    char *uart_data_paramp = emMCP_malloc(256); // 分配参数缓冲区内存
    if (uart_data_paramp == NULL)
    {                               // 检查内存分配是否成功
      emMCP_log_error("Failed to allocate uart_data_paramp");
      emMCP_dev->isUartRecv = 0;  // 重置接收标志
      emMCP_event = emMCP_EVENT_NONE;
      return;
    }
    memset(uart_data_paramp, 0, 256);                  // 清零参数缓冲区
    emMCP_ReturnEvent(&_param_type, uart_data_paramp); // 解析串口数据并获取事件
    emMCP_dev->emMCPEventCallback(emMCP_event, _param_type,
                                  uart_data_paramp); // 调用事件回调函数
    emMCP_dev->isUartRecv = 0;                       // 重置接收标志
    emMCP_event = emMCP_EVENT_NONE;                  // 重置事件为无事件
    emMCP_free(uart_data_paramp);                    // 释放参数缓冲区内存
    // 清零 UART 数据缓冲区，防止残留数据影响下次接收
    if (uart_data_buf != NULL)
    {
      uart_data_buf[0] = '\0';
    }
  }
}
// 内存优化：可选功能，通过定义 EMCP_ENABLE_EXTRA_CMDS 启用
#ifdef EMCP_ENABLE_EXTRA_CMDS
/**
 * @brief 设置通讯波特率
 *
 * 该函数用于向AI设备发送设置波特率的命令，并等待设备返回确认结果
 *
 * @param baudrate 要设置的波特率值
 * @return int 成功返回0，失败返回-1
 */
int emMCP_SetBaudrate(uint16_t baudrate)
{
  // 检查波特率参数是否有效
  if (baudrate <= 0)
  {
    return -1;
  }

  // 定义命令缓冲区并初始化为0
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));

  // 构造设置波特率的JSON命令
  sprintf(cmd, "baudrate-set {\"role\":\"MCU\",\"msgType\":\"status\",\"data\":%d}\r\n", baudrate);

  // 通过串口发送命令
  uartPortSendData(cmd, strlen(cmd));

  // 等待AI设备返回结果，设置超时机制
  int timerout = 0;
  while (!emMCP_CheckUartSendStatus() && timerout < 4000 / 10)
  {
    emMCP_delay(delay_time); // 延时等待
    timerout++;              // 增加超时计数器
  }

  // 检查是否超时
  if (timerout >= 4000 / 10)
  {
    return -1; // 超时返回失败
  }

  return 0; // 成功返回
}
/**
 * @brief emMCP 设置唤醒
 *
 * 该函数用于向AI设备发送唤醒命令，并设置唤醒时间
 *
 * @param WakeUp_Time 唤醒时间值
 * @return int 成功返回0，失败返回-1
 */
int emMCP_SetAiWakeUp(uint8_t WakeUp_Time)
{
  // 定义命令缓冲区并初始化为0
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));

  // 构造唤醒命令的JSON格式
  sprintf(cmd, "wake-up {\"role\":\"MCU\",\"msgType\":\"wake-up\",\"data\":%d}\r\n", WakeUp_Time);

  // 通过串口发送命令，并获取返回值
  int ret = uartPortSendData(cmd, strlen(cmd));

  // 检查返回值，如果大于0表示发送失败
  if (ret > 0)
  {
    return -1; // 发送失败返回-1
  }
  else
  {
    return 0; // 发送成功返回0
  }
}
/**
 * @brief emMCP 设置音量
 *
 * @param volume
 * @return int
 */
int emMCP_SetAiVolume(uint8_t volume)
{
  if (volume > 100)
  {
    return -1;
  }
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));
  sprintf(cmd, "volume-set {\"role\":\"MCU\",\"msgType\":\"status\",\"data\":%d}\r\n", volume);
  int ret = uartPortSendData(cmd, strlen(cmd));
  if (ret > 0)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
/**
 * @brief emMCP 获取音量
 *
 * @return uint8_t
 */
uint8_t emMCP_CheckAiVolume(void)
{
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));
  sprintf(cmd, "volume-check {\"role\":\"MCU\",\"msgType\":\"data\"}\r\n");
  int ret = uartPortSendData(cmd, strlen(cmd));
  if (ret > 0)
  {
    return -1;
  }
  int timerout = 0;
  while (!emMCP_CheckUartSendStatus() && timerout < 4000 / delay_time)
  {
    emMCP_delay(delay_time);
    timerout++;
  }
  if (timerout >= 4000 / 10)
  {
    return -1;
  }
  return emMCP_AiVolume;
}
#endif /* EMCP_ENABLE_EXTRA_CMDS */
/**
 * @brief emMCP 响应控制结果
 *
 * @param value
 */
int emMCP_ResponseValue(char *value)
{
  if (emMCP_dev == NULL || value == NULL)
  {
    emMCP_log_error("emMCP_dev is NULL");
    return -1;
  }
  char cmd[256] = {0};
  memset(cmd, 0, sizeof(cmd));
  cJSON *value_type = cJSON_Parse(value);
  if (value_type == NULL)
  {
    sprintf(cmd, "mcp-responsive {\"role\":\"MCU\",\"msgType\":\"status\",\"data\":\"%s\"}\r\n", value);
  }
  else
  {
    sprintf(cmd, "mcp-responsive {\"role\":\"MCU\",\"msgType\":\"status\",\"data\":%s}\r\n", value);
  }
  if (value_type != NULL)
    cJSON_free(value_type);
  return uartPortSendData(cmd, strlen(cmd));
}
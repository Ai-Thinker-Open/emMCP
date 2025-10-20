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
#include "FreeRTOS.h"
#include "uartPort.h"
#include <stddef.h>
#include <stdio.h>

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
static char *mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_MAX] = {"true", "false", "null", "number", "string", "array", "object", "text", "boolean"};
/**
 * @brief emMCP 日志等级
 *
 */
emMCP_LogLevel log_level = emMCP_LOG_LEVEL_DEBUG;

/**
 * @brief emMCP 串口数据缓存区
 *
 */
char uart_data_buf[512] = {0};
/**
 * @brief emMCP 串口数据参数缓存区
 *
 */
static char uart_data_paramp[256];
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
static emMCP_event_t emMCP_ReturnEvent(mcp_server_tool_type_t *param_type);
/**
 * @brief 事件回调函数
 *
 * @param event
 * @param param
 * @return __emMCPWeak
 */
__emMCPWeak void emMCP_EventCallback(emMCP_event_t event, mcp_server_tool_type_t type, void *param)
{
  char *param_str = (char *)param;
  emMCP_log_debug("emMCP_EventCallback: event:%d,type:%d,param:%s", event, type, param_str);
}
/**
 * @brief emMCP 设置回调函数提醒，如果你没有设置回调函数，该工具的检查函数都会调用这个函数
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
 * @brief emMCP 检查回调函数提醒，如果你没有设置回调函数，该工具的检查函数都会调用这个函数
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
int emMCP_Init(emMCP_t *emMCP)
{
  if (emMCP == NULL)
  {
    emMCP_log_error("emMCP_init: emMCP is NULL");
    return -1;
  }
  emMCP_dev = emMCP;
  if (emMCP_dev->emMCPVersion == NULL)
    emMCP_dev->emMCPVersion = emMCP_VERSION;

  // 初始化emMCP
  if (emMCP_dev->tools_root == NULL)
  {
    emMCP_dev->tools_root = cJSON_CreateObject();
    emMCP_dev->tools_arry = cJSON_CreateArray();
    cJSON_AddItemToObject(emMCP_dev->tools_root, "tools", emMCP_dev->tools_arry);
  }
  // 初始化回调函数
  emMCP_dev->emMCPEventCallback = emMCP_EventCallback;
  return 0;
}
/**
 * @brief 添加工具到工具列表
 *
 * @param toolsList
 * @param tool
 * @return int
 */
int emMCP_AddToolToToolList(emMCP_tool_t *tool)
{

  if (tool == NULL || emMCP_dev->tools_arry == NULL)
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: tool or toolsList is NULL");
    return -32604;
  }
  emMCP_tool_t *tmp_tool = tool; // 临时工具对象
  // 判断回调的工具是否已经存在
  if (tmp_tool->setRequestHandler == NULL)
    tmp_tool->setRequestHandler = emMCP_Set_CMDCallback;
  if (tmp_tool->checkRequestHandler == NULL)
    tmp_tool->checkRequestHandler = emMCP_check_CMDCallback;

  if (mcp_tool_arry[0].name == NULL)
  {
    memcpy(&mcp_tool_arry[0], tmp_tool, sizeof(emMCP_tool_t));
  }
  else
  {
    for (int i = 0; i < MCP_SERVER_TOOL_NUMBLE_MAX; i++)
    {
      if (mcp_tool_arry[i].name == NULL)
      {
        memcpy(&mcp_tool_arry[i], tmp_tool, sizeof(emMCP_tool_t));
        break;
      }
    }
  }
  cJSON *json_tool = cJSON_CreateObject();
  if (json_tool == NULL)
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: json_tool is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX);
    return -32604;
  }

  cJSON_bool json_ret;
  if (emMCP_dev != NULL && emMCP_dev->tools_arry != NULL)
  {
    json_ret = cJSON_AddItemToArray(emMCP_dev->tools_arry, json_tool);
  }
  else
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: emMCP_dev or tools_arry is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX);
    return -32604;
  }
  if (json_ret == -1)
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: json_toolsList add json_tool failed");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX);
    return -32604;
  }
  if (tmp_tool->name != NULL)
  {
    cJSON_AddStringToObject(json_tool, "name", tmp_tool->name);
  }
  else
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: tool name is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX);

    return -32604;
  }

  cJSON_AddStringToObject(json_tool, "description", tmp_tool->description);
  // inputSchema 对象
  cJSON *inputSchema = cJSON_CreateObject();
  cJSON_AddItemToObject(json_tool, "inputSchema", inputSchema);

  // 添加properties参数
  cJSON *properties = cJSON_CreateObject();
  uint8_t properties_cnt = 0;
  if (sizeof(tmp_tool->inputSchema.properties) / sizeof(properties_t) > 0)
  {
    for (properties_cnt = 0; properties_cnt < MCP_SERVER_TOOL_PROPERTIES_NUM; properties_cnt++)
    {
      if (tmp_tool->inputSchema.properties[properties_cnt].name != NULL) // 判断是否为空
      {
        cJSON *prop = cJSON_CreateObject();
        cJSON_AddStringToObject(prop, "description", tmp_tool->inputSchema.properties[properties_cnt].description);
        cJSON_AddStringToObject(prop, "type", mcp_sever_type_str[tmp_tool->inputSchema.properties[properties_cnt].type]);
        cJSON_AddItemToObject(properties, tmp_tool->inputSchema.properties[properties_cnt].name, prop);
      }
      else
      {
        break;
      }
    }
  }
  if (properties_cnt > 0)
    cJSON_AddItemToObject(inputSchema, "properties", properties);
  // 添加methods参数

  cJSON *methods = cJSON_CreateObject();

  uint8_t methods_num = 0;
  if (sizeof(tmp_tool->inputSchema.methods) / sizeof(methods_t) > 0)
  {
    for (methods_num = 0; methods_num < MCP_SERVER_TOOL_METHODS_NUM; methods_num++)
    {
      if (tmp_tool->inputSchema.methods[methods_num].name != NULL) // 判断是否为空
      {
        cJSON *method = cJSON_CreateObject();
        cJSON_AddItemToObject(methods, tmp_tool->inputSchema.methods[methods_num].name, method);
        cJSON_AddStringToObject(method, "description", tmp_tool->inputSchema.methods[methods_num].description);
        // 添加parameters参数
        if (sizeof(tmp_tool->inputSchema.methods[methods_num].parameters) / sizeof(parameters_t) > 0)
        {
          cJSON *parameters = cJSON_CreateObject();
          cJSON_AddItemToObject(method, "parameters", parameters);
          for (size_t i = 0; i < MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM; i++)
          {
            if (tmp_tool->inputSchema.methods[i].parameters[i].name != NULL) // 判断是否为空
            {
              cJSON *param = cJSON_CreateObject();
              cJSON_AddItemToObject(parameters, tmp_tool->inputSchema.methods[i].parameters[i].name, param);
              cJSON_AddStringToObject(param, "description", tmp_tool->inputSchema.methods[i].parameters[i].description);
              cJSON_AddStringToObject(param, "type", mcp_sever_type_str[tmp_tool->inputSchema.methods[i].parameters[i].type]);
            }
          }
        }
      }
      else
      {
        break;
      }
    }
  }
  if (methods_num > 0)
    cJSON_AddItemToObject(inputSchema, "methods", methods);
  return 0;
}

/**
 * @brief Construct a new mcp server responsive tool request object
 * 		// 根据工具名称，找到对应的工具，并执行对应的请求
 *
 * @param tool_name
 * @param arguments
 * @return returnValues_t
 */
static void emMCP_ResponsiveToolRequest(char *tool_name, cJSON *arguments)
{

  if (tool_name == NULL || arguments == NULL)
  {
    emMCP_log_error("emMCP_responsive_tool_request: tool_name or arguments is NULL");

    return;
  }
  // 开始定位工具
  uint8_t tools_numble = 0;
  for (tools_numble = 0; tools_numble < MCP_SERVER_TOOL_NUMBLE_MAX; tools_numble++)
  {
    if (mcp_tool_arry[tools_numble].name != NULL && strcmp(mcp_tool_arry[tools_numble].name, tool_name) == 0)
    {
      break;
    }
  }

  if (strcmp(mcp_tool_arry[tools_numble].name, tool_name) == 0)
  {
    // 判断是否为 methods 参数
    if (cJSON_GetObjectItem(arguments, mcp_tool_arry[tools_numble].inputSchema.methods[0].parameters[0].name) != NULL ||
        cJSON_GetObjectItem(arguments, mcp_tool_arry[tools_numble].inputSchema.methods[0].name) != NULL ||
        cJSON_GetObjectItem(arguments, "methods") != NULL)
    {
      // 执行请求
      if (cJSON_GetObjectItem(arguments, "methods") != NULL)
      {
        cJSON *method = cJSON_GetObjectItem(cJSON_GetObjectItem(arguments, "methods"), mcp_tool_arry[tools_numble].inputSchema.methods[0].name);
        mcp_tool_arry[tools_numble].setRequestHandler(method == NULL ? arguments : method);
      }
      else if (cJSON_GetObjectItem(arguments, mcp_tool_arry[tools_numble].inputSchema.methods[0].parameters[0].name) != NULL)
      {
        cJSON *method = cJSON_GetObjectItem(arguments, mcp_tool_arry[tools_numble].inputSchema.methods[0].parameters[0].name);
        mcp_tool_arry[tools_numble].setRequestHandler(method == NULL ? arguments : method);
      }
      else
      {
        mcp_tool_arry[tools_numble].setRequestHandler(arguments);
      }
    }
    else if (cJSON_GetObjectItem(arguments, mcp_tool_arry[tools_numble].inputSchema.properties[0].name) != NULL || arguments->child == NULL)
    {
      cJSON *prop = cJSON_GetObjectItem(arguments, mcp_tool_arry[tools_numble].inputSchema.properties[0].name);

      if (prop != NULL && prop->type != cJSON_NULL)
      {
        mcp_tool_arry[tools_numble].setRequestHandler(arguments);
      }
      else
      {
        mcp_tool_arry[tools_numble].checkRequestHandler(arguments);
      }
    }
    return;
  }
}
/**
 * @brief 根据参数名称，获取参数
 *
 * @param params
 * @param param_name
 * @return cJSON*
 */
cJSON *emMCP_GetParam(cJSON *params, char *param_name)
{
  if (params == NULL || param_name == NULL)
  {
    return NULL;
  }
  return cJSON_GetObjectItem(params, param_name);
}

/**
 * @brief 检查UART数据是否发送成功
 *
 */
int emMCP_CheckUartSendStatus(void)
{
  return emMCP_event == emMCP_EVENT_CMD_OK ? 1 : 0;
}
/**
 * @brief emMCP注册工具到AI设备
 *
 * @return int
 */
int emMCP_RegistrationTools(void)
{
  if (emMCP_dev->tools_root == NULL || emMCP_dev == NULL || emMCP_dev->tools_arry == NULL)
  {
    emMCP_log_error("emMCP_registration_tools: tools_root is NULL");
    return -1;
  }
  emMCP_dev->tools_str = cJSON_PrintUnformatted(emMCP_dev->tools_root);
  char *cmd = emMCP_malloc(strlen(emMCP_dev->tools_str) + 64);
  if (cmd != NULL && emMCP_dev->tools_str != NULL)
  {
    memset(cmd, 0, strlen(emMCP_dev->tools_str) + 64);
    sprintf(cmd, "mcp-tool {\"role\":\"MCU\",\"msgType\":\"MCP\",\"MCP\":%s}\r\n", emMCP_dev->tools_str);

    memset(uart_data_buf, 0, sizeof(uart_data_buf));
    uartPortSendData(cmd, strlen(cmd));
  }
  emMCP_free(cmd);
  if (emMCP_dev->tools_str != NULL)
    cJSON_free(emMCP_dev->tools_str);
  cJSON_Delete(emMCP_dev->tools_root);
  emMCP_dev->tools_root = NULL;
  emMCP_dev->isUartRecv = false;
  return 0;
}
/**
 * @brief
 *
 * @return emMCP_event_t
 */
static emMCP_event_t emMCP_ReturnEvent(mcp_server_tool_type_t *param_type)
{
  // 检查串口数据是否为0

  if (strlen(uart_data_buf) == 0)
  {

    return emMCP_EVENT_NONE;
  }
  // 检查串口数据是否为json格式
  cJSON *root = cJSON_Parse(uart_data_buf);
  if (root == NULL)
  {

    return emMCP_EVENT_NONE;
  }
  // 检查串口数据是否为AI发送的数据
  cJSON *role = cJSON_GetObjectItem(root, "role");
  if (role == NULL || strcmp(role->valuestring, "AI board") != 0)
  {

    cJSON_Delete(root);
    return emMCP_EVENT_NONE;
  }
  cJSON *msgType = cJSON_GetObjectItem(root, "msgType");
  cJSON *msgType_param = NULL;
  // 消息类型为status
  if (msgType != NULL && strcmp(msgType->valuestring, "status") == 0)
  {
    *param_type = MCP_SERVER_TOOL_TYPE_STRING;
    msgType_param = cJSON_GetObjectItem(root, "status");
    if (msgType_param != NULL)
    {
      if (strcmp(msgType_param->valuestring, "OK") == 0)
      {
        cJSON *status_parm = cJSON_GetObjectItem(root, "volume");
        if (status_parm != NULL && status_parm->type == cJSON_Number)
        {
          emMCP_AiVolume = status_parm->valueint;
        }
        emMCP_event = emMCP_EVENT_CMD_OK;
      }
      else if (strncmp(msgType_param->valuestring, "ERROR", 5) == 0)
      {
        emMCP_event = emMCP_EVENT_CMD_ERROR;
      }
      else if (strcmp(msgType_param->valuestring, "AI Start") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_START;
      }
      else if (strcmp(msgType_param->valuestring, "1.WiFi connect OK") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_WIFI_CONNNECT;
      }
      else if (strcmp(msgType_param->valuestring, "2.WakeUP") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_WAKE;
      }
      else if (strcmp(msgType_param->valuestring, "3.Sleep") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_SLEEP;
      }
      else if (strcmp(msgType_param->valuestring, "4.NetCFG") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_NETCFG;
      }
      else if (strcmp(msgType_param->valuestring, "5.NetERR") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_NETERR;
      }
      else if (strcmp(msgType_param->valuestring, "6.OTAUPDATE") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_OTAUPDATE;
      }
      else if (strcmp(msgType_param->valuestring, "7.OTA OK") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_OTAOK;
      }
      else if (strcmp(msgType_param->valuestring, "8.OTA ERR") == 0)
      {
        emMCP_event = emMCP_EVENT_AI_OTAERR;
      }
    }
  }
  else if (msgType != NULL && strcmp(msgType->valuestring, "MCP") == 0)
  {
    emMCP_event = emMCP_EVENT_AI_MCP_CMD;
    *param_type = MCP_SERVER_TOOL_TYPE_OBJECT;
    msgType_param = cJSON_GetObjectItem(root, "MCP");
    if (msgType_param == NULL || msgType_param->type != cJSON_Object)
    {

      cJSON_Delete(root);
      memset(uart_data_paramp, 0, sizeof(uart_data_paramp));
      return emMCP_EVENT_NONE;
    }
    // 从MCP 工具中解析出参数
    cJSON *param = cJSON_GetObjectItem(msgType_param, "params");
    if (param != NULL && param->type == cJSON_Object)
    {
      cJSON *mcp_tool_name = cJSON_GetObjectItem(param, "name");
      cJSON *arguments = cJSON_GetObjectItem(param, "arguments");
      if (arguments != NULL && arguments->type == cJSON_Object && mcp_tool_name != NULL)
      {
        // 处理MCP工具
        emMCP_ResponsiveToolRequest(mcp_tool_name->valuestring, arguments);
      }
    }
  }
  else if (msgType != NULL && strcmp(msgType->valuestring, "MCP Text") == 0)
  {
    emMCP_event = emMCP_EVENT_AI_MCP_Text;
    *param_type = MCP_SERVER_TOOL_TYPE_TEXT;
    msgType_param = cJSON_GetObjectItem(root, "MCP Text");
    if (msgType_param == NULL || msgType_param->type != cJSON_Object)
    {

      cJSON_Delete(root);
      memset(uart_data_paramp, 0, sizeof(uart_data_paramp));
      return emMCP_EVENT_NONE;
    }
  }

  // _exit:
  if (emMCP_event != emMCP_EVENT_NONE)
  {
    memset(uart_data_paramp, 0, sizeof(uart_data_paramp));
    if (*param_type == MCP_SERVER_TOOL_TYPE_STRING)
      strcpy(uart_data_paramp, msgType_param->valuestring);
    else
    {
      char *param_str = cJSON_PrintUnformatted(msgType_param);
      strcpy(uart_data_paramp, param_str);
      cJSON_free(param_str);
    }
  }

  cJSON_Delete(root);

  return emMCP_event;
}
/**
 * @brief emMCP 更新串口接收状态
 *
 * @param isRecv
 */
void emMCP_UpdateUartRecv(bool isRecv)
{
  emMCP_dev->isUartRecv = isRecv;
}
/**
 * @brief emMCP 循环
 *
 */
void emMCP_TickHandle(int delay_ms)
{
  if (emMCP_dev == NULL || emMCP_dev->tools_arry == NULL || emMCP_dev->emMCPEventCallback == NULL)
  {
    emMCP_log_error("emMCP Partial parameter is NULL");
    return;
  }
  if (delay_ms != delay_time)
  {
    delay_time = delay_ms;
  }

  if (emMCP_dev->isUartRecv)
  {
    mcp_server_tool_type_t _param_type = MCP_SERVER_TOOL_TYPE_STRING;
    emMCP_ReturnEvent(&_param_type);
    emMCP_dev->emMCPEventCallback(emMCP_event, _param_type, uart_data_paramp);
    emMCP_dev->isUartRecv = 0;
    emMCP_event = emMCP_EVENT_NONE;
  }
}
/**
 * @brief 设置通讯波特率
 *
 */
int emMCP_SetBaudrate(uint16_t baudrate)
{
  if (baudrate <= 0)
  {
    return -1;
  }
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));
  sprintf(cmd, "baudrate-set {\"role\":\"MCU\",\"msgType\":\"status\",\"status\":\"%d\"}\r\n", baudrate);
  uartPortSendData(cmd, strlen(cmd));
  // 等待AI设备返回结果
  int timerout = 0;
  while (!emMCP_CheckUartSendStatus() && timerout < 4000 / 10)
  {
    emMCP_delay(delay_time);
    timerout++;
  }
  if (timerout >= 4000 / 10)
  {
    return -1;
  }
  return 0;
}
/**
 * @brief emMCP 设置唤醒
 *
 * @return int
 */
int emMCP_SetAiWakeUp(uint8_t WakeUp_Time)
{
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));
  sprintf(cmd, "wake-up {\"role\":\"MCU\",\"msgType\":\"wake-up\",\"wake-up\":\"%d\"}\r\n", WakeUp_Time);
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
  sprintf(cmd, "volume-set {\"role\":\"MCU\",\"msgType\":\"status\",\"status\":\"%d\"}\r\n", volume);
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
  sprintf(cmd, "volume-check {\"role\":\"MCU\",\"msgType\":\"status\"}\r\n");
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
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));
  sprintf(cmd, "mcp-responsive {\"role\":\"MCU\",\"msgType\":\"status\",\"status\":\"%s\"}\r\n", value);
  return uartPortSendData(cmd, strlen(cmd));
}

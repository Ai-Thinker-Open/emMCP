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
 * @brief emMCP 工具数组，用来简单管理工具
 *
 */
emMCP_tool_t mcp_tool_arry[MCP_SERVER_TOOL_NUMBLE_MAX] = {0};
/**
 * @brief emMCP JSON类型字符串
 *
 */
static char *mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_MAX] = {"true", "false", "null", "number", "string", "array", "object", "text", "boolean"};
/**
 * @brief emMCP 返回值
 *
 */
static returnValues_t ret = {0};
/**
 * @brief emMCP 日志等级
 *
 */
#ifdef DEBUG
emMCP_LogLevel log_level = emMCP_LOG_LEVEL_DEBUG;
#else
emMCP_LogLevel log_level = emMCP_LOG_LEVEL_INFO;
#endif
/**
 * @brief emMCP 串口数据缓存区
 *
 */
char uart_data_buf[512] = {0};
/**
 * @brief emMCP 工具注册标志
 *
 */
emMCP_t *emMCP_dev = NULL;

/**
 * @brief emMCP 初始化
 *
 * @param emMCP
 * @return int
 */
int emMCP_init(emMCP_t *emMCP)
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
  emMCP_log_info("emMCP_init: emMCP init success");
  return 0;
}
/**
 * @brief 添加工具到工具列表
 *
 * @param toolsList
 * @param tool
 * @return int
 */
int emMCP_add_tool_to_toolList(emMCP_tool_t *tool)
{

  if (tool == NULL || emMCP_dev->tools_arry == NULL)
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: tool or toolsList is NULL");
    return -32604;
  }
  // cJSON *json_toolsList = emMCP_dev->tools_arry;
  emMCP_tool_t *tmp_tool = tool; // 临时工具对象
  // 添加工具对象到mcp_tool_arry
  emMCP_log_debug("add tool: %s", tmp_tool->name);
  if (mcp_tool_arry[0].name == NULL)
  {
    emMCP_log_debug("tools:\"%s\" add first", tmp_tool->name);
    memcpy(&mcp_tool_arry[0], tmp_tool, sizeof(emMCP_tool_t));
  }
  else
  {
    for (int i = 0; i < MCP_SERVER_TOOL_NUMBLE_MAX; i++)
    {
      if (mcp_tool_arry[i].name == NULL)

      {
        emMCP_log_debug("tools:\"%s\" add %d", tmp_tool->name, i);
        memcpy(&mcp_tool_arry[i], tmp_tool, sizeof(emMCP_tool_t));
        break;
      }
    }
  }
  emMCP_log_debug("tools:\"%s\" add success", tmp_tool->name);
  // 创建并添加工具对象到toolsList
  cJSON *json_tool = cJSON_CreateObject();
  if (json_tool == NULL)
  {
    emMCP_log_error("emMCP_add_tool_to_toolList: json_tool is NULL");
    memset(mcp_tool_arry, 0, sizeof(emMCP_tool_t) * MCP_SERVER_TOOL_NUMBLE_MAX);
    return -32604;
  }
  emMCP_log_debug("add tool name:\"%s\" to arry", tmp_tool->name);

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
  emMCP_log_debug("add tool name :\"%s\" to arry %s", tmp_tool->name, emMCP_dev->tools_arry->string);
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
  emMCP_log_debug("add tool name :\"%s\" to json_tool", tmp_tool->name);
  cJSON_AddStringToObject(json_tool, "description", tmp_tool->description);
  emMCP_log_debug("add tool description :\"%s\" to json_tool", tmp_tool->description);
  // inputSchema 对象
  cJSON *inputSchema = cJSON_CreateObject();
  cJSON_AddItemToObject(json_tool, "inputSchema", inputSchema);
  emMCP_log_debug("add tool inputSchema to json_tool");
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
        if (sizeof(tmp_tool->inputSchema.methods[methods_num].parameters) /
                sizeof(parameters_t) >
            0)
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
  log_debug("json_tool:\n%s", cJSON_PrintUnformatted(json_tool));
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
returnValues_t emMCP_responsive_tool_request(char *tool_name,
                                             cJSON *arguments)
{

  if (tool_name == NULL || arguments == NULL)
  {
    emMCP_log_error("emMCP_responsive_tool_request: tool_name or arguments is NULL");
    ret.error_code = -32602;
    return ret;
  }
  // 开始定位工具
  // 计算当前有多少个工具
  uint8_t tools_numble = 0;
  for (; tools_numble < MCP_SERVER_TOOL_NUMBLE_MAX; tools_numble++)
  {
    if (mcp_tool_arry[tools_numble].name == NULL)
    {
      break;
    }
  }
  if (tools_numble == 0)
  {
    emMCP_log_error("tools numble is 0");
    ret.error_code = -32602;

    return ret;
  }
  for (int i = 0; i < tools_numble; i++)
  {
    if (strcmp(mcp_tool_arry[i].name, tool_name) == 0)
    {
      // 判断是否为 methods 参数
      if (cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[0].parameters[0].name) != NULL ||
          cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[0].name) != NULL ||
          cJSON_GetObjectItem(arguments, "methods") != NULL)
      {
        // 执行请求
        if (cJSON_GetObjectItem(arguments, "methods") != NULL)
        {
          cJSON *method = cJSON_GetObjectItem(cJSON_GetObjectItem(arguments, "methods"), mcp_tool_arry[i].inputSchema.methods[0].name);
          ret = mcp_tool_arry[i].setRequestHandler(method == NULL ? arguments : method);
        }
        else if (cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[0].parameters[0].name) != NULL)
        {
          cJSON *method = cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[0].parameters[0].name);
          ret = mcp_tool_arry[i].setRequestHandler(method == NULL ? arguments : method);
        }
        else
        {
          ret = mcp_tool_arry[i].setRequestHandler(arguments);
        }

        if (ret.type == NULL)
        {
          ret.type = "text";
        }
      }
      else if (cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.properties[0].name) != NULL || arguments->child == NULL)
      {
        cJSON *prop = cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.properties[0].name);
        if (prop != NULL && prop->type != cJSON_NULL)
        {
          ret = mcp_tool_arry[i].setRequestHandler(arguments);
          if (ret.type == NULL)
          {
            ret.type = "text";
          }
        }
        else
        {
          mcp_tool_arry[i].checkRequestHandler(&ret);
        }
      }
      return ret;
    }
  }
  // 回调
  ret.error_code = -32602;
  return ret;
}
/**
 * @brief 根据参数名称，获取参数
 *
 * @param params
 * @param param_name
 * @return cJSON*
 */
cJSON *emMCP_get_param(cJSON *params, char *param_name)
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
int emMCP_check_uart_send_status(void)
{
  return 0;
}
/**
 * @brief emMCP注册工具到AI设备
 *
 * @return int
 */
int emMCP_registration_tools(void)
{
  if (emMCP_dev->tools_root == NULL || emMCP_dev == NULL || emMCP_dev->tools_arry == NULL)
  {
    emMCP_log_error("emMCP_registration_tools: tools_root is NULL");
    return -1;
  }
  emMCP_dev->tools_str = cJSON_PrintUnformatted(emMCP_dev->tools_root);
  emMCP_log_debug("emMCP_registration_tools: tools_str:");
  emMCP_log_debug(emMCP_dev->tools_str);
  char *cmd = (char *)emMCP_malloc(strlen(emMCP_dev->tools_str) + 64);
  if (cmd != NULL && emMCP_dev->tools_str != NULL)
  {
    memset(cmd, 0, strlen(emMCP_dev->tools_str) + 64);
    sprintf(cmd, "mcp-tool {\"role\":\"MCU\",\"msgType\":\"MCP\",\"MCP\":%s}\r\n", emMCP_dev->tools_str);
    emMCP_log_debug("cmd: %s", cmd);
  }

  uartPortSendData(cmd, strlen(cmd));
  emMCP_free(cmd);
  if (emMCP_dev->tools_str != NULL)
    cJSON_free(emMCP_dev->tools_str);
  cJSON_Delete(emMCP_dev->tools_root);
  emMCP_log_debug("emMCP_registration_tools: send cmd to AI device");
  emMCP_dev->tools_root = NULL;

  // 等待AI设备返回注册结果

  return 0;
}

/**
 * @brief 设置通讯波特率
 *
 */
int emMCP_set_baudrate(uint16_t baudrate)
{
  if (baudrate <= 0)
  {
    return -1;
  }
  char cmd[128] = {0};
  memset(cmd, 0, sizeof(cmd));
  sprintf(cmd, "baudrate-set {\"role\":\"MCU\",\"msgType\":\"status\",\"status\":\"%d\"}\r\n", baudrate);
  uartPortSendData(cmd, strlen(cmd));
  return 0;
}

/**
 * @brief 状态机运行函数，需要在主循环中调用
 *
 */
void uartPortStateHandle(void) {}

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
#include <stdio.h>
#include "emMCP.h"
#include <string.h>
#include <string.h>
#include <stdint.h>
#include <emMCPLOG.h>

emMCP_tool_t mcp_tool_arry[MCP_SERVER_TOOL_NUMBLE_MAX] = {0};
static char *mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_MAX] = {"false", "true", "null", "number", "string", "array", "object", "text", "boolean"};
static returnValues_t ret = {0};
static cJSON *root;
emMCP_LogLevel log_level;
int emMCP_init(emMCP_t *emMCP)
{
  if (emMCP == NULL)
  {
    emMCP_log_error("emMCP_init: emMCP is NULL");
    return -1;
  }
  // 初始化emMCP
  root = cJSON_CreateObject();
  if (emMCP->tools_root == NULL)
  {
    emMCP->tools_root = root;
    emMCP->tools_arry = cJSON_CreateArray();
  }
  return 0;
}
/**
 * @brief 添加工具到工具列表
 * 
 * @param toolsList 
 * @param tool 
 * @return int 
 */
int emMCP_add_tool_to_toolList(void *toolsList, emMCP_tool_t *tool)
{

  if (tool == NULL || toolsList == NULL)
  {

    return -32604;
  }
  cJSON *json_toolsList = (cJSON *)toolsList;
  emMCP_tool_t *tmp_tool = tool; // 临时工具对象
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

  // 创建并添加工具对象到toolsList
  cJSON *json_tool = cJSON_CreateObject();
  cJSON_AddItemToArray(json_toolsList, json_tool);
  cJSON_AddStringToObject(json_tool, "name", tmp_tool->name);
  cJSON_AddStringToObject(json_tool, "description", tmp_tool->description);
  // inputSchema 对象
  cJSON *inputSchema = cJSON_CreateObject();
  cJSON_AddItemToObject(json_tool, "inputSchema", inputSchema);
  // 添加properties参数
  cJSON *properties = cJSON_CreateObject();
  cJSON_AddItemToObject(inputSchema, "properties", properties);
  if (sizeof(tmp_tool->inputSchema.properties) / sizeof(properties_t) > 0)
  {

    for (size_t i = 0; i < MCP_SERVER_TOOL_PROPERTIES_NUM; i++)
    {
      if (tmp_tool->inputSchema.properties[i].name != NULL) // 判断是否为空
      {
        cJSON *prop = cJSON_CreateObject();
        cJSON_AddStringToObject(prop, "description", tmp_tool->inputSchema.properties[i].description);
        cJSON_AddStringToObject(prop, "type", mcp_sever_type_str[tmp_tool->inputSchema.properties[i].type]);
        cJSON_AddItemToObject(properties, tmp_tool->inputSchema.properties[i].name, prop);
      }
    }
  }
  cJSON *methods = cJSON_CreateObject();
  cJSON_AddItemToObject(inputSchema, "methods", methods);
  if (sizeof(tmp_tool->inputSchema.methods) / sizeof(methods_t) > 0)
  {
    for (size_t i = 0; i < MCP_SERVER_TOOL_METHODS_NUM; i++)
    {
      if (tmp_tool->inputSchema.methods[i].name != NULL) // 判断是否为空
      {
        cJSON *method = cJSON_CreateObject();
        cJSON_AddItemToObject(methods, tmp_tool->inputSchema.methods[i].name, method);
        cJSON_AddStringToObject(method, "description", tmp_tool->inputSchema.methods[i].description);
        // 添加parameters参数
        if (sizeof(tmp_tool->inputSchema.methods[i].parameters) / sizeof(parameters_t) > 0)
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
    }
  }

  return 0;
}
/**
 * @brief 添加串口工具到工具列表，但不会添加到设备内部工具列表进行管理
 *
 * @param toolsList
 * @param tool
 * @return int
 */
int mcp_server_add_uart_tool_to_toolList(void *toolsList, cJSON *tool)
{

  if (tool == NULL || toolsList == NULL)
  {

    return -32604;
  }
  cJSON *json_toolsList = (cJSON *)toolsList;
  if (tool == NULL)
  {
    return -32604;
  }
  cJSON_AddItemToArray(json_toolsList, tool);
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
returnValues_t mcp_server_responsive_tool_request(char *tool_name, cJSON *arguments)
{

  if (tool_name == NULL || arguments == NULL)
  {
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
    printf("tools numble is 0\r\n");
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
cJSON *mcp_server_get_param(cJSON *params, char *param_name)
{
  if (params == NULL || param_name == NULL)
  {
    return NULL;
  }
  return cJSON_GetObjectItem(params, param_name);
}

/**
 * @file user_mcp.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include "user_mcp.h"
#include "log.h"
static emMCP_t emMCP_dev;

emMCP_tool_t led_tool;
static returnValues_t setLEDRequestHandler(void *arguments)
{
	// 获取参数
	returnValues_t ret = {0, "", NULL};
	if (arguments == NULL)
	{
		ret.error_code = -32602;
		return ret;
	}

	cJSON *params = (cJSON *)arguments; // 获取参数
	// 获取被设置参数值
	cJSON *enabled = emMCP_get_param(params, "enabled");
}

void user_mcp_init(void)
{
	emMCP_init(&emMCP_dev);
	led_tool.name = "Self.LED.switch";
	led_tool.description = "LED switch tool";
	led_tool.inputSchema.properties[0].name = "enable";
	led_tool.inputSchema.properties[0].description =
		"LED switch, true:on, false:off, when queried, attribute is null";
	led_tool.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
	int ret = emMCP_add_tool_to_toolList(&led_tool);
	if (ret != 0)
	{
		log_error("add tool failed");
		return;
	}
	ret = emMCP_registration_tools();
	if (ret != 0)
	{
		log_error("MCP registration tools failed");
		return;
	}
	log_info("MCP init success");
}
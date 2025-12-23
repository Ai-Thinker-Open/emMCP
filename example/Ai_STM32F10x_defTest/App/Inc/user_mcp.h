/**
 * @file user_mcp.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-10-15
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */

#ifndef __USER_MCP_H
#define __USER_MCP_H
#include "emMCP.h"
#include "main.h"

#define MCP_BUFFER_SIZE 256
extern char rxBuffer[MCP_BUFFER_SIZE];

void user_mcp_init(void);
#endif /* __USER_MCP_H */
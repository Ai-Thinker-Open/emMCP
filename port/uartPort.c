/**
 * @file uartPort.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include "uartPort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuffer.h"
/**
 * @brief 串口发送函数接口
 * 
 * @param data 
 * @param len 
 * @return int 
 */
int uartPortSendData(char* data, int len)
{
	// 在此处实现串口发送函数
    
    return 0;
}
/**
 * @brief 串口接收函数接口,把这个函数在串口接收中断或接收循环中调用
 *        
 * @param data 
 * @param len 
 * @return int 
 */
int uartPortRecvData(char ch)
{
	
    return 0;
}
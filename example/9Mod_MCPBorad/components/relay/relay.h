/**
 * @file relay.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __RELAY_H__
#define __RELAY_H__

#define ON 1
#define OFF 0

void axk_relay_init(void);
void axk_relay_set(unsigned char state);
unsigned char axk_relay_get(void);
void axk_relay_toggle(void);
#endif

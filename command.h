/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * USB keyboard adapter for reMarkable 2.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _COMMAND_H
#define _COMMAND_H

#include "app.h"

// Handle the command that's currently in rx_packet. Caller has to make sure
// that receiving has finished and that rx_packet contains a complete packet.
void rx_handle_command();

void cmd_handle_attribute_read();
void cmd_handle_get_auth_key();
void cmd_handle_enter_app();

void cmd_send_keep_alive();
void cmd_send_key(key_event_type_t type, uint8_t keycode);

#endif

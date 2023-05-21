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

#ifndef _APP_H
#define _APP_H

#include "pico/stdlib.h"

typedef enum app_mode {
  APP_NEGOTIATING = 0,
  APP_KEYBOARD = 1
} app_mode_t;

typedef struct app_state {
  app_mode_t mode;
  absolute_time_t last_keep_alive;
} app_state_t;

extern app_state_t app_state;

typedef enum key_event_type {
  KEY_UP = 0,
  KEY_DOWN = 1
} key_event_type_t;

typedef struct key_event {
  key_event_type_t type;
  uint8_t keycode; // TinyUSB HID_KEY_* constants.
} key_event_t;

// Push a keyboard event onto the queue. This discards data if the maximum
// queue capacity has been reached.
void app_push_key_event(key_event_t event);

// Pop a keyboard event off of the queue. This returns NULL if the queue is
// empty. Note that you should use the return value immediately, since the
// location it points to may be overwritten soon (this should be fine since
// this is all running in a single execution thread anyway...).
key_event_t *app_pop_key_event();

#endif

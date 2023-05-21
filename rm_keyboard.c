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

#include "rm_keyboard.h"
#include "command.h"
#include "tusb.h"

#define KEYCODE_INVALID 0xff

uint8_t keycodes[256];

void rmk_init() {
  // Setup the key map.
  for (int i = 0; i < 256; i++) {
    keycodes[i] = KEYCODE_INVALID;
  }

  // The HID_* constants can be found in the file src/class/hid/hid.h in the
  // TinyUSB source code.

  keycodes[HID_KEY_A] = KEY(3, 14);
  keycodes[HID_KEY_B] = KEY(0, 8);
  keycodes[HID_KEY_C] = KEY(1, 10);
  keycodes[HID_KEY_D] = KEY(0, 10);
  keycodes[HID_KEY_E] = KEY(2, 9);
  keycodes[HID_KEY_F] = KEY(1, 9);
  keycodes[HID_KEY_G] = KEY(1, 8);
  keycodes[HID_KEY_H] = KEY(1, 7);
  keycodes[HID_KEY_I] = KEY(3, 5);
  keycodes[HID_KEY_J] = KEY(5, 6);
  keycodes[HID_KEY_K] = KEY(4, 5);
  keycodes[HID_KEY_L] = KEY(5, 4);
  keycodes[HID_KEY_M] = KEY(6, 6);
  keycodes[HID_KEY_N] = KEY(0, 7);
  keycodes[HID_KEY_O] = KEY(4, 4);
  keycodes[HID_KEY_P] = KEY(4, 3);
  keycodes[HID_KEY_Q] = KEY(2, 12);
  keycodes[HID_KEY_R] = KEY(2, 10);
  keycodes[HID_KEY_S] = KEY(3, 13);
  keycodes[HID_KEY_T] = KEY(2, 8);
  keycodes[HID_KEY_U] = KEY(4, 6);
  keycodes[HID_KEY_V] = KEY(0, 9);
  keycodes[HID_KEY_W] = KEY(2, 11);
  keycodes[HID_KEY_X] = KEY(0, 11);
  keycodes[HID_KEY_Y] = KEY(3, 7);
  keycodes[HID_KEY_Z] = KEY(0, 12);

  keycodes[HID_KEY_0] = KEY(3, 4);
  keycodes[HID_KEY_1] = KEY(4, 12);
  keycodes[HID_KEY_2] = KEY(4, 11);
  keycodes[HID_KEY_3] = KEY(3, 11);
  keycodes[HID_KEY_4] = KEY(3, 10);
  keycodes[HID_KEY_5] = KEY(3, 9);
  keycodes[HID_KEY_6] = KEY(3, 8);
  keycodes[HID_KEY_7] = KEY(2, 7);
  keycodes[HID_KEY_8] = KEY(4, 7);
  keycodes[HID_KEY_9] = KEY(5, 5);

  keycodes[HID_KEY_ARROW_DOWN] = KEY(1, 4);
  keycodes[HID_KEY_ARROW_RIGHT] = KEY(0, 3);
  keycodes[HID_KEY_ARROW_UP] = KEY(3, 6);
  keycodes[HID_KEY_ARROW_LEFT] = KEY(2, 5);
  keycodes[HID_KEY_END] = KEY(1, 13);
  keycodes[HID_KEY_BACKSPACE] = KEY(2, 3);
  keycodes[HID_KEY_BACKSLASH] = KEY(2, 4);
  keycodes[HID_KEY_ENTER] = KEY(2, 6);
  keycodes[HID_KEY_EQUAL] = KEY(3, 3);
  keycodes[HID_KEY_HOME] = KEY(4, 2);
  keycodes[HID_KEY_SEMICOLON] = KEY(5, 3);
  keycodes[HID_KEY_GRAVE] = KEY(5, 7);
  keycodes[HID_KEY_TAB] = KEY(5, 12);
  keycodes[HID_KEY_SPACE] = KEY(6, 2);
  keycodes[HID_KEY_SLASH] = KEY(6, 3);
  keycodes[HID_KEY_PERIOD] = KEY(6, 4);
  keycodes[HID_KEY_COMMA] = KEY(6, 5);
  keycodes[HID_KEY_APOSTROPHE] = KEY(6, 7);

  keycodes[HID_KEY_CAPS_LOCK] = KEY(3, 12);
  keycodes[HID_KEY_CONTROL_LEFT] = KEY(2, 0);
  keycodes[HID_KEY_ALT_LEFT] = KEY(2, 1);
  keycodes[HID_KEY_SHIFT_LEFT] = KEY(5, 14);
}

void rmk_process_event(key_event_t *event) {
  uint8_t rm_code = keycodes[event->keycode];
  if (rm_code == KEYCODE_INVALID) {
    return;
  }

  cmd_send_key(event->type, rm_code);
}

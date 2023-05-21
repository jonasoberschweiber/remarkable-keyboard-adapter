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

#include <stdio.h>
#include <string.h>

#include "app.h"
#include "packet.h"
#include "attribute.h"
#include "command.h"

// These are the values that we're returning as responses to attribute reads.
static char *DATA_DEVICE_NAME = "rMkeyboard01";
static char *DATA_SERIAL_NUMBER = "RM712-311-11212";
static int32_t DATA_DEVICE_ID[] = { 0x3011040, 0xafaea528, 0x14160517, 0xf5000510 };
static uint16_t DATA_FIRMWARE_VERSION = 0x102;
static uint8_t DATA_LANGUAGE = 0x01;
static int32_t DATA_DEVICE_CLASS = 0x80000002;
static uint32_t DATA_IMAGE_START_ADDRESS = 0x40000;
static uint8_t DATA_KEY_LAYOUT = 0x01;
static char *DATA_AUTH_KEY = "@O8eO77%o^4*1GE@oeodd#WMa%8Kr6v@";

void rx_handle_command() {
  switch (rx_packet.command) {
    case CMD_ATTRIBUTE_READ:
      cmd_handle_attribute_read();
      break;

    case CMD_GET_AUTH_KEY:
      cmd_handle_get_auth_key();
      break;

    case CMD_ENTER_APP:
      cmd_handle_enter_app();
      break;

    default:
      printf("ERROR: Do not know how to handle command: %s\n", command_name(rx_packet.command));
      break;
  }
}

void cmd_handle_attribute_read() {
  // Requested attribute IDs are in the data part of the packet, one byte per
  // ID. No separators between them. In our answer, we're expected to send the
  // attribute ID first, then a NULL byte. After that it's one byte for the data
  // type we're answering with. In some cases, an additional byte for the length
  // of the data. And then the data bytes of the answer.
  tx_packet.command = CMD_ATTRIBUTE_READ;
  tx_packet.data_length = 0;
  for (int i = 0; i < rx_packet.data_length; i++) {
    attribute_id_t attr_id = (attribute_id_t)rx_packet.data[i];

    switch (attr_id) {
      case ATTR_DEVICE_NAME:
        tx_add_string_attribute(ATTR_DEVICE_NAME, DATA_DEVICE_NAME);
        break;

      case ATTR_FIRMWARE_VERSION:
        tx_add_uint16_attribute(ATTR_FIRMWARE_VERSION, DATA_FIRMWARE_VERSION);
        break;
      
      case ATTR_LANGUAGE:
        tx_add_enum8_attribute(ATTR_LANGUAGE, DATA_LANGUAGE);
        break;

      case ATTR_DEVICE_CLASS:
        tx_add_int32_attribute(ATTR_DEVICE_CLASS, DATA_DEVICE_CLASS);
        break;

      case ATTR_IMAGE_START_ADDRESS:
        tx_add_uint32_attribute(ATTR_IMAGE_START_ADDRESS, DATA_IMAGE_START_ADDRESS);
        break;

      case ATTR_KEY_LAYOUT:
        tx_add_uint8_attribute(ATTR_KEY_LAYOUT, DATA_KEY_LAYOUT);
        break;

      case ATTR_DEVICE_ID:
        tx_add_int32_array_attribute(ATTR_DEVICE_ID, DATA_DEVICE_ID, 4);
        break;

      case ATTR_SERIAL_NUMBER:
        tx_add_string_attribute(ATTR_SERIAL_NUMBER, DATA_SERIAL_NUMBER);
        break;

      default:
        printf("ERROR: Do not know how to read attribute %x\n", attr_id);
        return;
    }
  }

  tx_write_packet();
}

void cmd_handle_get_auth_key() {
  tx_packet.command = CMD_GET_AUTH_KEY;
  // Auth key expects a NULL terminator.
  tx_packet.data_length = strlen(DATA_AUTH_KEY) + 1;
  strcpy((char *)tx_packet.data, DATA_AUTH_KEY);

  tx_write_packet();
}

void cmd_handle_enter_app() {
  tx_packet.command = CMD_ENTER_APP;
  tx_packet.data_length = 0;

  tx_write_packet();

  app_state.mode = APP_KEYBOARD;
  app_state.last_keep_alive = get_absolute_time();
}

void cmd_send_keep_alive() {
  tx_packet.command = CMD_REPORT_ALIVE;
  tx_packet.data_length = 0;

  tx_write_packet();
}

void cmd_send_key(key_event_type_t type, uint8_t keycode) {
  tx_packet.command = CMD_REPORT_KEY;
  tx_packet.data_length = 2;
  tx_packet.data[0] = keycode | (uint8_t)type;
  tx_packet.data[1] = 0;

  tx_write_packet();
}

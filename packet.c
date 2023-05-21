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
#include "pico/stdlib.h"
#include "packet.h"

// Global variables for rx_process_byte.
uint16_t data_counter = 0;
rx_state_t rx_state;
uint8_t tx_buffer[TX_BUFFER_LEN];

packet_t rx_packet;
packet_t tx_packet;

static char *cmd_str_none = "CMD_NONE";
static char *cmd_str_fw_write_validate_image = "CMD_FW_WRITE_VALIDATE_IMAGE";
static char *cmd_str_enter_app = "CMD_ENTER_APP";
static char *cmd_str_enter_suspend = "CMD_ENTER_SUSPEND";
static char *cmd_str_fw_write_validate_crc = "CMD_FW_WRITE_VALIDATE_CRC";
static char *cmd_str_fw_write_packet = "CMD_FW_WRITE_PACKET";
static char *cmd_str_fw_write_init = "CMD_FW_WRITE_INIT";
static char *cmd_str_get_auth_key = "CMD_GET_AUTH_KEY";
static char *cmd_str_reboot = "CMD_REBOOT";
static char *cmd_str_attribute_read = "CMD_ATTRIBUTE_READ";
static char *cmd_str_attribute_write = "CMD_ATTRIBUTE_WRITE";
static char *cmd_str_report_alive = "CMD_REPORT_ALIVE";
static char *cmd_str_report_key = "CMD_REPORT_KEY";
static char *cmd_str_unknown = "UNKNOWN";

char *command_name(command_t command) {
  switch (command) {
    case CMD_NONE: return cmd_str_none;
    case CMD_FW_WRITE_VALIDATE_IMAGE: return cmd_str_fw_write_validate_image;
    case CMD_ENTER_APP: return cmd_str_enter_app;
    case CMD_ENTER_SUSPEND: return cmd_str_enter_suspend;
    case CMD_FW_WRITE_VALIDATE_CRC: return cmd_str_fw_write_validate_crc;
    case CMD_FW_WRITE_PACKET: return cmd_str_fw_write_packet;
    case CMD_FW_WRITE_INIT: return cmd_str_fw_write_init;
    case CMD_GET_AUTH_KEY: return cmd_str_get_auth_key;
    case CMD_REBOOT: return cmd_str_reboot;
    case CMD_ATTRIBUTE_READ: return cmd_str_attribute_read;
    case CMD_ATTRIBUTE_WRITE: return cmd_str_attribute_write;
    case CMD_REPORT_ALIVE: return cmd_str_report_alive;
    case CMD_REPORT_KEY: return cmd_str_report_key;

    default:
      return cmd_str_unknown;
  }
}

void print_packet(packet_t *packet, packet_direction_t direction) {
  char prefix = ' ';
  if (direction == DIRECTION_TX) {
    printf(">>>> TX PACKET\n");
    prefix = '>';
  } else {
    printf("<<<< RX PACKET\n");
    prefix = '<';
  }
  printf("%c Command: %s\n", prefix, command_name(packet->command));
  printf("%c Data length: %d\n", prefix, packet->data_length);
  printf("%c Data: ", prefix);
  for (int i = 0; i < packet->data_length; i++) {
    printf("%x ", packet->data[i]);
  }
  printf("\n");
  printf("%c Checksum: %d\n", prefix, packet->checksum);
  if (direction == DIRECTION_TX) {
    printf(">>>> END TX\n");
  } else {
    printf("<<<< END RX\n");
  }
}


void rx_switch_to_init_state() {
  rx_state = RX_INIT;
}

rx_result_t rx_process_byte(uint8_t data) {
  switch (rx_state) {
    case RX_INIT:
      // Received our first data, reinitialize the current package and switch
      // to key state.
      memset(rx_packet.data, 0, MAX_PACKET_DATA);
      rx_packet.command = 0;
      rx_packet.data_length = 0;
      rx_packet.checksum = 0;

      // Deliberate fall through.
      rx_state = RX_KEY;

    case RX_KEY:
      if (data == 0x3a) { // Next byte after 0x3a will be the low byte of length.
        rx_state = RX_LEN_LOW;
      }
      return RX_PACKET_RECEIVING;

    case RX_LEN_LOW:
      rx_packet.data_length += ((uint16_t)data << 0);
      rx_packet.checksum += data;
      rx_state = RX_LEN_HIGH;
      return RX_PACKET_RECEIVING;
    
    case RX_LEN_HIGH:
      rx_packet.data_length += ((uint16_t)data << 8);
      rx_packet.checksum += data;
      // TODO: In theory, the data length can be greater than MAX_PACKET_DATA.
      rx_state = RX_CMD;
      return RX_PACKET_RECEIVING;

    case RX_CMD:
      rx_packet.command = data;
      rx_packet.checksum += data;
      if (rx_packet.data_length > 0) {
        data_counter = 0;
        rx_state = RX_DATA;
      } else {
        rx_state = RX_CHECKSUM;
      }
      return RX_PACKET_RECEIVING;

    case RX_DATA:
      rx_packet.checksum += data;
      rx_packet.data[data_counter] = data;
      if (++data_counter >= rx_packet.data_length) {
        rx_state = RX_CHECKSUM;
      }
      return RX_PACKET_RECEIVING;

    case RX_CHECKSUM:
      rx_packet.checksum ^= 0xff;
      rx_packet.checksum++;

      rx_state = RX_INIT;

      if (rx_packet.checksum == data) {
        return RX_PACKET_RECEIVED;
      } else {
        printf("Checksum %x vs %x\n", rx_packet.checksum, data);
        return RX_PACKET_INVALID_CHECKSUM;
      }
  }

  return RX_PACKET_RECEIVING;
}


void tx_write_packet() {
  int i = 0;
  tx_buffer[0] = 0x2e;
  tx_buffer[1] = (tx_packet.data_length >> 0) & 0xff;
  tx_buffer[2] = (tx_packet.data_length >> 8) & 0xff;
  tx_buffer[3] = tx_packet.command;

  tx_packet.checksum = tx_buffer[1] + tx_buffer[2] + tx_buffer[3];

  for (i = 0; i < tx_packet.data_length; i++) {
    tx_buffer[4 + i] = tx_packet.data[i];
    tx_packet.checksum += tx_packet.data[i];
  }

  tx_packet.checksum ^= 0xff;
  tx_packet.checksum++;
  tx_buffer[4 + i] = tx_packet.checksum;

  printf("Sending packet\n");
  print_packet(&tx_packet, DIRECTION_TX);

  uart_write_blocking(uart1, tx_buffer, 5 + tx_packet.data_length);
}


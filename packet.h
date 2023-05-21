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

#ifndef _PACKET_H
#define _PACKET_H

#include "pico/stdlib.h"

#define TX_BUFFER_LEN 136
#define MAX_PACKET_DATA 128

// From linux/drivers/misc/rm-pogo/pogo.h
typedef enum _command {
  CMD_NONE = 0x00,
  CMD_FW_WRITE_VALIDATE_IMAGE = 0x02,
  CMD_ENTER_APP = 0x04,
  CMD_ENTER_SUSPEND = 0x05,
  CMD_FW_WRITE_VALIDATE_CRC = 0x06,
  CMD_FW_WRITE_PACKET = 0x07,
  CMD_FW_WRITE_INIT = 0x08,
  CMD_GET_AUTH_KEY = 0x09,
  CMD_REBOOT = 0x0f,
  CMD_ATTRIBUTE_READ = 0x20,
  CMD_ATTRIBUTE_WRITE = 0x21,

  // The next two are not defined as commands in the kernel driver. Instead
  // they're in packet_kb_report_t.
  CMD_REPORT_ALIVE = 0x40,
  CMD_REPORT_KEY = 0x51
} command_t;

typedef struct packet {
  command_t command; // First byte is the command.
  uint16_t data_length; // Next two bytes are the data length, low byte first.
  uint8_t checksum; // Next byte is the checksum.
  uint8_t data[MAX_PACKET_DATA]; // And arbitrary data after that.
} packet_t;

// There is one global incoming and one global outgoing packet.
extern packet_t rx_packet;
extern packet_t tx_packet;

// State machine enum for rx_process_byte.
typedef enum rx_state {
  RX_INIT = 0,
  RX_KEY = 1,
  RX_LEN_LOW,
  RX_LEN_HIGH,
  RX_CMD,
  RX_DATA,
  RX_CHECKSUM
} rx_state_t;

// Return value enum for rx_process_byte.
typedef enum rx_result {
  RX_PACKET_RECEIVING = 0,
  RX_PACKET_RECEIVED = 1,
  RX_PACKET_INVALID_CHECKSUM = 2
} rx_result_t;

// Enum for print_packet.
typedef enum packet_direction {
  DIRECTION_TX,
  DIRECTION_RX
} packet_direction_t;

void print_packet(packet_t *packet, packet_direction_t direction);

void rx_switch_to_init_state();
rx_result_t rx_process_byte(uint8_t data);

void tx_write_packet();

char *command_name(command_t command);

#endif

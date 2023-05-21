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

#include <string.h>

#include "packet.h"
#include "attribute.h"

void tx_add_attribute_header(attribute_id_t attribute_id, attribute_type_t type) {
  tx_packet.data[tx_packet.data_length++] = attribute_id;
  tx_packet.data[tx_packet.data_length++] = 0x00;
  tx_packet.data[tx_packet.data_length++] = type;
}

void tx_add_int32_attribute(attribute_id_t attribute_id, int32_t value) {
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 4 < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_INT32);
  tx_packet.data[tx_packet.data_length++] = (value >> 0) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 8) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 16) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 24) & 0xff;
}

void tx_add_uint8_attribute(attribute_id_t attribute_id, uint8_t value) {
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 1 < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_UINT8);
  tx_packet.data[tx_packet.data_length++] = value;
}

void tx_add_uint16_attribute(attribute_id_t attribute_id, uint16_t value) {
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 2 < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_UINT16);
  tx_packet.data[tx_packet.data_length++] = (value >> 0) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 8) & 0xff;
}

void tx_add_uint32_attribute(attribute_id_t attribute_id, uint32_t value) {
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 4 < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_UINT32);
  tx_packet.data[tx_packet.data_length++] = (value >> 0) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 8) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 16) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (value >> 24) & 0xff;
}

void tx_add_enum8_attribute(attribute_id_t attribute_id, uint8_t value) {
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 1 < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_ENUM8);
  tx_packet.data[tx_packet.data_length++] = value;
}

void tx_add_string_attribute(attribute_id_t attribute_id, char *string) {
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 1 + strlen(string) < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_STRING);
  tx_packet.data[tx_packet.data_length++] = strlen(string);
  memcpy((uint8_t *)(tx_packet.data + tx_packet.data_length), string, strlen(string));
  tx_packet.data_length += strlen(string);
}

void tx_add_int32_array_attribute(attribute_id_t attribute_id, int32_t *data, size_t len) {
  size_t actual_len = len * 4;
  assert(tx_packet.data_length + ATTR_HEADER_LENGTH + 3 + actual_len < MAX_PACKET_DATA);

  tx_add_attribute_header(attribute_id, ATTR_TYPE_ARRAY);
  tx_packet.data[tx_packet.data_length++] = ATTR_TYPE_INT32; // Data type for array
  // Next is length as uint16_t.
  tx_packet.data[tx_packet.data_length++] = (len >> 0) & 0xff;
  tx_packet.data[tx_packet.data_length++] = (len >> 8) & 0xff;
  // And the data.
  memcpy((uint8_t *)(tx_packet.data + tx_packet.data_length), data, actual_len);
  tx_packet.data_length += actual_len;
}


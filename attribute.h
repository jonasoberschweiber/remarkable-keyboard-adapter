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

#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include "pico/stdlib.h"

#define ATTR_HEADER_LENGTH 3

typedef enum attribute_id {
  ATTR_FIRMWARE_VERSION = 0x02,
  ATTR_DEVICE_CLASS = 0x04,
  ATTR_DEVICE_ID = 0x05,
  ATTR_IMAGE_START_ADDRESS = 0x06,
  ATTR_DEVICE_NAME = 0x07,
  ATTR_KEY_LAYOUT = 0x10,
  ATTR_LANGUAGE = 0x11,
  ATTR_SERIAL_NUMBER = 0x12
} attribute_id_t;

typedef enum attribute_type {
  ATTR_TYPE_INT8 = 0x08,
  ATTR_TYPE_INT16 = 0x09,
  ATTR_TYPE_INT32 = 0x0b,
  ATTR_TYPE_BOOL = 0x10,
  ATTR_TYPE_UINT8 = 0x18,
  ATTR_TYPE_UINT16 = 0x19,
  ATTR_TYPE_UINT32 = 0x1b,
  ATTR_TYPE_ENUM8 = 0x30,
  ATTR_TYPE_STRING = 0x42,
  ATTR_TYPE_ARRAY = 0x48
} attribute_type_t;

void tx_add_attribute_header(attribute_id_t attribute_id, attribute_type_t type);
void tx_add_int32_attribute(attribute_id_t attribute_id, int32_t value);
void tx_add_uint8_attribute(attribute_id_t attribute_id, uint8_t value);
void tx_add_uint16_attribute(attribute_id_t attribute_id, uint16_t value);
void tx_add_uint32_attribute(attribute_id_t attribute_id, uint32_t value);
void tx_add_enum8_attribute(attribute_id_t attribute_id, uint8_t value);
void tx_add_string_attribute(attribute_id_t attribute_id, char *string);
void tx_add_int32_array_attribute(attribute_id_t attribute_id, int32_t *data, size_t len);

#endif

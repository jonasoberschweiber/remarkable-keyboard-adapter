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

#ifndef _USB_KEYBOARD_H
#define _USB_KEYBOARD_H

#include "tusb_config.h"
#include "tusb.h"

void usb_process_keyboard_report(hid_keyboard_report_t const *report);

#endif

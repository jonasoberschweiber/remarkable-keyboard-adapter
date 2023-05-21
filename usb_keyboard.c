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

#include "usb_keyboard.h"
#include "pico/stdlib.h"
#include "app.h"

// TinyUSB sends us reports about HID deviceEs. Each HID device instance can have
// multiple reports. We need to store these reports somewhere, which is what
// hid_report_count and hid_report_info are for. CFG_TUH_HID is the maximum
// number of HID devices supported by TinyUSB. It's defined in tusb_config.h
#define MAX_REPORT 4
static uint8_t hid_report_count[CFG_TUH_HID];
static tuh_hid_report_info_t hid_report_info[CFG_TUH_HID][MAX_REPORT];

#define MAX_KEY 6

void hid_app_task() {
}

// TinyUSB calls this when a new device with an HID interface is mounted.
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  printf("USB: Device mounted\n");
  hid_report_count[instance] = tuh_hid_parse_report_descriptor(
    hid_report_info[instance],
    MAX_REPORT,
    desc_report,
    desc_len
  );

  // We need to tell TinyUSB that we're interested in further reports for this.
  tuh_hid_receive_report(dev_addr, instance);
}

// TinyUSB calls with when a device with an HID interface is unmounted.
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  printf("USB: Device unmounted\n");
}

// TinyUSB calls this when we receive a report from a mounted HID device. The
// report contains information about the keys that have been pressed.
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  uint8_t const report_count = hid_report_count[instance];
  tuh_hid_report_info_t *report_infos = hid_report_info[instance];
  tuh_hid_report_info_t *report_info = NULL;

  if (report_count == 1 && report_infos[0].report_id == 0) {
    // Simple report.
    report_info = &report_infos[0];
  } else {
    // Composite report. First byte is report ID, data starts at second byte.
    uint8_t const report_id = report[0];
    for (uint8_t i = 0; i < report_count; i++) {
      if (report_id == report_infos[i].report_id) {
        report_info = &report_infos[i];
        break;
      }
    }
    
    report++;
    len--;
  }

  if (report_info &&
      report_info->usage_page == HID_USAGE_PAGE_DESKTOP &&
      report_info->usage == HID_USAGE_DESKTOP_KEYBOARD) {
    usb_process_keyboard_report((hid_keyboard_report_t const *)report);
  }

  // Request to receive further reports.
  tuh_hid_receive_report(dev_addr, instance);
}

static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode) {
  for (uint8_t i = 0; i < MAX_KEY; i++) {
    if (report->keycode[i] == keycode) {
      return true;
    }
  }

  return false;
}

static inline bool find_mod_in_report(hid_keyboard_report_t const *report, uint8_t mod_mask) {
  return report->modifier & mod_mask;
}

// Define helpers, since we want to treat left and right modifiers the same.
#define MOD_CTRL (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL)
#define MOD_SHIFT (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT)
#define MOD_ALT (KEYBOARD_MODIFIER_LEFTALT | KEYBOARD_MODIFIER_RIGHTALT)

// A global variable that will hold the key event that we'll publish. This
// should be fine since we're passing the struct by value anyways, so a copy
// will be created.
static key_event_t tmp_key_event;

void publish_key_event(key_event_type_t type, uint8_t key) {
  tmp_key_event.type = type;
  tmp_key_event.keycode = key;

  app_push_key_event(tmp_key_event);
}

void usb_process_keyboard_report(hid_keyboard_report_t const *report) {
  static hid_keyboard_report_t prev_report = {0, 0, {0}};

  for (uint8_t i = 0; i < MAX_KEY; i++) {
    uint8_t key = report->keycode[i];
    if (key != 0 && !find_key_in_report(&prev_report, key)) {
      publish_key_event(KEY_DOWN, key);
    }
  }
  for (uint8_t i = 0; i < MAX_KEY; i++) {
    uint8_t key = prev_report.keycode[i];
    if (key != 0 && !find_key_in_report(report, key)) {
      publish_key_event(KEY_UP, key);
    }
  }

  if (find_mod_in_report(report, MOD_CTRL) && !find_mod_in_report(&prev_report, MOD_CTRL)) {
    publish_key_event(KEY_DOWN, HID_KEY_CONTROL_LEFT);
  } else if (!find_mod_in_report(report, MOD_CTRL) && find_mod_in_report(&prev_report, MOD_CTRL)) {
    publish_key_event(KEY_UP, HID_KEY_CONTROL_LEFT);
  }

  if (find_mod_in_report(report, MOD_SHIFT) && !find_mod_in_report(&prev_report, MOD_SHIFT)) {
    publish_key_event(KEY_DOWN, HID_KEY_SHIFT_LEFT);
  } else if (!find_mod_in_report(report, MOD_SHIFT) && find_mod_in_report(&prev_report, MOD_SHIFT)) {
    publish_key_event(KEY_UP, HID_KEY_SHIFT_LEFT);
  }

  if (find_mod_in_report(report, MOD_ALT) && !find_mod_in_report(&prev_report, MOD_ALT)) {
    publish_key_event(KEY_DOWN, HID_KEY_ALT_LEFT);
  } else if (!find_mod_in_report(report, MOD_ALT) && find_mod_in_report(&prev_report, MOD_ALT)) {
    publish_key_event(KEY_UP, HID_KEY_ALT_LEFT);
  }

  prev_report = *report;
}

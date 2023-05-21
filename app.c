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

#include "app.h"
#include "packet.h"
#include "command.h"
#include "usb_keyboard.h"
#include "rm_keyboard.h"

app_state_t app_state;

#define MAX_KEY_EVENT 10

key_event_t key_event_queue[MAX_KEY_EVENT];
uint8_t key_event_write_ix, key_event_read_ix;

void app_push_key_event(key_event_t event) {
  if (app_state.mode != APP_KEYBOARD) {
    // Ignore all data unless we're in keyboard mode.
    return;
  }

  uint8_t next = key_event_write_ix + 1;
  if (next >= MAX_KEY_EVENT) {
    next = 0;
  }

  if (next == key_event_read_ix) {
    // Buffer is full, discard the data.
    return;
  }

  key_event_queue[key_event_write_ix] = event;
  key_event_write_ix = next;
}

key_event_t *app_pop_key_event() {
  if (key_event_write_ix == key_event_read_ix) {
    return NULL; // Buffer is currently empty.
  }

  uint8_t next = key_event_read_ix + 1;
  if (next >= MAX_KEY_EVENT) {
    next = 0;
  }

  key_event_t *evt = &key_event_queue[key_event_read_ix];
  key_event_read_ix = next;

  return evt;
}

int main() {
  stdio_uart_init();

  if (tusb_init()) {
    printf("TinyUSB initialized: %d\n", tuh_inited());
  } else {
    printf("ERROR: TinyUSB could not be initialized\n");
  }

  uart_init(uart1, 115200);
  gpio_set_function(4, GPIO_FUNC_UART);
  gpio_set_function(5, GPIO_FUNC_UART);

  rmk_init();

  key_event_write_ix = key_event_read_ix = 0;

  int c;

  app_state.mode = APP_NEGOTIATING;
  app_state.last_keep_alive = nil_time;

  absolute_time_t current_time = nil_time;

  key_event_t *key_event = NULL;

  while (true) {
    c = getchar_timeout_us(0);
    if (c == '.') {
      app_state.mode = APP_NEGOTIATING;
      rx_switch_to_init_state();
      uart_putc(uart1, 0xff);
      printf("\n\n=====");
    }

    tuh_task();

    if (uart_is_readable(uart1)) {
      char data = uart_getc(uart1);
      int packet_state = rx_process_byte((uint8_t)data);

      if (packet_state == RX_PACKET_RECEIVED) {
        printf("Packet received in full.\n");
        print_packet(&rx_packet, DIRECTION_RX);
        rx_handle_command();
      } else if (packet_state == RX_PACKET_INVALID_CHECKSUM) {
        printf("Packet received, but has invalid checksum.\n");
      }
    }

    if (app_state.mode == APP_KEYBOARD) {
      while ((key_event = app_pop_key_event()) != NULL) {
        printf("Received key event: %d %d\n", key_event->type, key_event->keycode);
        rmk_process_event(key_event);
      }

      current_time = get_absolute_time();
      if (absolute_time_diff_us(app_state.last_keep_alive, current_time) > 400000) {
        cmd_send_keep_alive();
        app_state.last_keep_alive = current_time;
      }
    }
  }

  return 0;
}

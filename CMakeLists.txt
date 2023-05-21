cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(rm_keyboard_adapter C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(-Wall
  -Wno-format
  -Wno-unused-function
  -Wno-maybe-unitialized
)

add_executable(rm_keyboard_adapter
  app.c
  packet.c
  attribute.c
  command.c
  usb_keyboard.c
  rm_keyboard.c
)

pico_enable_stdio_usb(rm_keyboard_adapter 0)
pico_enable_stdio_uart(rm_keyboard_adapter 1)

target_include_directories(rm_keyboard_adapter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

pico_add_extra_outputs(rm_keyboard_adapter)

target_link_libraries(rm_keyboard_adapter
  pico_stdlib
  tinyusb_board
  tinyusb_host
  )

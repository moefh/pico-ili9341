add_executable(pico_ili9341
  main.c
  core_msg.c
  screen.c
  game_data.c
  lib/ili9341.c
  lib/font.c
  lib/image.c
)

target_link_libraries(pico_ili9341
  pico_stdlib
  pico_multicore
)

pico_enable_stdio_usb(pico_ili9341 1)
pico_add_extra_outputs(pico_ili9341)

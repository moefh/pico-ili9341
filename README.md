# pico-ili9341

This is a demo using the Raspberry Pi Pico to send images to an ILI9341 LCD controller in 320x240 at 60 fps using the parallel bus.

![Screen](images/running.jpg)

This demo uses 8 color bits per pixel (even though the ILI9341 supports 16 or 18 bits) to save memory in the Pico, so we can have two framebuffers. This way we can send one framebuffer to the ILI9341 while we draw the next one. We put the ILI9341 in 16 bit color mode, but use only 3 red bits, 3 green bits and 2 blue bits, all the other low color bits are set to 0. This gets us 256 colors, an improvement over my [VGA code](https://github.com/moefh/pico-loser) which gets only 64 colors (since there we use 2 bits for vsync/hsync, leaving 6 bits for color data).

Initially the plan was to use PIO with DMA to make things fast, that's why the pin connections between the Pico and the ILI9341 are all out of order (all the high order bits for the color components must use sequential GPIO pins in order to use PIO). It turns out that just bit-banging the data out is fast enough to get 60 frames per second, so we're currently not using PIO or DMA (bit banging is fast enough for about 130fps, the bottleneck is copying the images from the flash to the framebuffer).

Pin connections between the Pico and the LCD module:

![ILI9341 board pinout](images/lcd-pinout.jpg)

|  Pico GPIO | ILI9341 module pin | used for                           |
|-----------:|--------------------|------------------------------------|
|          2 | DB0                | command bit 0 / unused red bit 0   |
|          3 | DB1                | command bit 1 / unused red bit 1   |
|          4 | DB5                | command bit 5 / unused green bit 0 |
|          5 | DB6                | command bit 6 / unused green bit 1 |
|          6 | DB7                | command bit 7 / unused green bit 2 |
|          7 | DB2                | command bit 2 / red bit 2          |
|          8 | DB3                | command bit 3 / red bit 3          |
|          9 | DB4                | command bit 4 / red bit 4          |
|         10 | DB8                | green bit 3                        |
|         11 | DB9                | green bit 4                        |
|         12 | DB10               | green bit 5                        |
|         13 | DB14               | blue bit 3                         |
|         14 | DB15               | blue bit 4                         |
|         15 | RST                | reset                              |
|         16 | CS                 | chip select                        |
|         17 | RS                 | command/data select                |
|         18 | WR                 | write select                       |
|         19 | RD                 | read select                        |
|        GND | DB11               | unused blue bit 0                  |
|        GND | DB12               | unused blue bit 1                  |
|        GND | DB13               | unused blue bit 2                  |

The color bits marked "unused" are driven low when sending color data, so only 8 color bits contain actual color data (3 red, 3 green and 2 blue).

![Screen](images/board.jpg)

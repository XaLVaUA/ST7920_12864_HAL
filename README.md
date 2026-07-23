# ST7920 12864 HAL Driver

A lightweight non-blocking STM32 HAL driver for ST7920 128×64 graphical LCDs using hardware SPI.

## Features

- Hardware SPI
- Non-blocking state machine
- Interrupt-driven SPI transfers (`HAL_SPI_Transmit_IT`)
- Automatic framebuffer rendering
- Configurable refresh period
- 1024-byte framebuffer
- Pixel drawing
- Primitive graphics
  - Lines
  - Rectangles
  - Filled rectangles
  - Circles
  - Filled circles
  - Thick lines
- Bitmap rendering
  - Row-based (MSB)
  - Row-based (LSB)
- Text rendering
  - MSB fonts
  - LSB fonts
  - Transparent or solid background
  - Adjustable character spacing

## Tested Hardware

### Microcontroller

- STM32F030F4P6

### LCD Module

- ST7920 12864-20M v3.8 (blue/white)

## Tested SPI Speeds

- 375 Kbit/s
- 500 Kbit/s
- 1.0 Mbit/s
- 1.5 Mbit/s
- 2.0 Mbit/s

## Basic Usage

Initialize the driver:

```c
static ST7920_12864_HAL_HandleTypeDef lcd;

ST7920_12864_HAL_InitHandle(
    &lcd,
    &hspi1,
    CS_GPIO_Port,
    CS_Pin,
    RST_GPIO_Port,
    RST_Pin,
    25      // Refresh period in milliseconds
);

ST7920_12864_HAL_StartInitDisplay(&lcd);
```

Run the state machine continuously:

```c
while (1)
{
    ST7920_12864_HAL_HandleState(&lcd);
}
```

Draw into the framebuffer at any time:

```c
ST7920_12864_HAL_ClearBuffer(&lcd);

ST7920_12864_HAL_DrawRect(&lcd, 5, 5, 40, 20, 1);

ST7920_12864_HAL_DrawStringLSB(
    &lcd,
    0,
    0,
    "Hello",
    &font,
    1,
    1,
    1
);
```

No explicit render function is required. The framebuffer is transferred to the display automatically by the internal state machine at the configured refresh interval.

## Rendering

The display refresh is fully asynchronous.

`ST7920_12864_HAL_HandleState()` performs three tasks:

- display initialization
- framebuffer transmission
- automatic timing control

The framebuffer is rendered every `renderTicks` milliseconds specified during initialization.

## Graphics API

The driver provides:

- Individual pixels
- Horizontal and vertical lines
- Arbitrary lines
- Rectangles
- Filled rectangles
- Circles
- Filled circles
- Thick lines
- Bitmap drawing (MSB/LSB, row/column layouts)
- Character rendering
- String rendering

## Framebuffer

Display resolution:

- 128 × 64 pixels

Framebuffer size:

- 1024 bytes

Applications draw directly into the framebuffer. Rendering is performed automatically in the background without blocking the CPU.

## Notes

- Rendering is fully interrupt-driven.
- No blocking delays are used during rendering.
- The framebuffer can be modified while the driver is idle between refreshes.
- During testing on the hardware listed above, a refresh period of **25 ms** produced stable output. Reducing the period to **20 ms** resulted in occasional rendering glitches in the lower half of the display.

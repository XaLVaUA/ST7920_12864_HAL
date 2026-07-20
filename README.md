# ST7920 12864 HAL Driver

A simple non-blocking STM32 HAL driver for ST7920 128×64 graphical LCDs using hardware SPI.

## Features

- Hardware SPI
- Non-blocking state machine
- Interrupt-driven SPI transfers (`HAL_SPI_Transmit_IT`)
- Framebuffer-based rendering
- Pixel-level drawing API
- No blocking delays during rendering

## Tested Hardware

### Microcontroller
- STM32F030F4P6

### LCD Module
- ST7920 12864-20M v3.8 (blue/white)

## Tested SPI Speeds

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
    RST_Pin
);

ST7920_12864_HAL_StartInitDisplay(&lcd);
```

Call the state machine continuously from the main loop:

```c
while (1)
{
    ST7920_12864_HAL_HandleState(&lcd);
}
```

Draw into the framebuffer:

```c
ST7920_12864_HAL_ClearBuffer(&lcd);

ST7920_12864_HAL_SetPixel(&lcd, 10, 10, 1);
ST7920_12864_HAL_SetPixel(&lcd, 11, 10, 1);
ST7920_12864_HAL_SetPixel(&lcd, 12, 10, 1);
```

Start rendering:

```c
ST7920_12864_HAL_StartRender(&lcd);
```

## Notes

- Rendering is fully asynchronous; `ST7920_12864_HAL_StartRender()` only starts the update, while `ST7920_12864_HAL_HandleState()` advances the internal state machine.
- The framebuffer size is **1024 bytes** (128 × 64 pixels).
- During testing on the hardware listed above, calling `ST7920_12864_HAL_StartRender()` every **25 ms** produced stable output. At **20 ms**, slight rendering glitches were observed in the bottom half of the display.
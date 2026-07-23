#ifndef ST7920_12864_HAL_H_
#define ST7920_12864_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct
{
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *csPort;
    uint16_t csPin;
    GPIO_TypeDef *rstPort;
    uint16_t rstPin;
    uint8_t pixelBuffer[1024];
    uint8_t txBuffer[12];
    uint8_t txBufferSize;
    uint8_t state;
    uint32_t lastTick;
    uint8_t y;
    uint8_t xByte;
    uint16_t renderTicks;
    uint32_t lastRenderTick;
} ST7920_12864_HAL_HandleTypeDef;

typedef struct
{
    const uint8_t *fontBytes;
    uint8_t fontWidth;
    uint8_t fontHeight;
    uint8_t firstChar;
    uint8_t lastChar;
} ST7920_12864_HAL_FontTypeDef;

void ST7920_12864_HAL_InitHandle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *csPort,
    uint16_t csPin,
    GPIO_TypeDef *rstPort,
    uint16_t rstPin,
    uint16_t renderTicks
);

void ST7920_12864_HAL_StartInitDisplay(ST7920_12864_HAL_HandleTypeDef *lcd);
uint8_t ST7920_12864_HAL_IsReady(ST7920_12864_HAL_HandleTypeDef *lcd);
uint8_t ST7920_12864_HAL_HandleState(ST7920_12864_HAL_HandleTypeDef *lcd);
void ST7920_12864_HAL_ClearBuffer(ST7920_12864_HAL_HandleTypeDef *lcd);
void ST7920_12864_HAL_SetPixel(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y, uint8_t color);

void ST7920_12864_HAL_DrawFastHLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t color
);
void ST7920_12864_HAL_DrawFastVLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t h,
    uint8_t color
);
void ST7920_12864_HAL_DrawLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    uint8_t color
);
void ST7920_12864_HAL_DrawRect
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    uint8_t color
);
void ST7920_12864_HAL_FillRect
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    uint8_t color
);
void ST7920_12864_HAL_DrawCircle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t xc,
    uint8_t yc,
    uint8_t r,
    uint8_t color
);
void ST7920_12864_HAL_FillCircle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t xc,
    uint8_t yc,
    uint8_t r,
    uint8_t color
);
void ST7920_12864_HAL_DrawThickLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    uint8_t thickness,
    uint8_t color
);
void ST7920_12864_HAL_DrawBitmap
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t *bmp
);
void ST7920_12864_HAL_DrawBitmapLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t *bmp
);
void ST7920_12864_HAL_DrawCharMSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    char c,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent
);
void ST7920_12864_HAL_DrawCharLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    char c,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent
);
uint8_t ST7920_12864_HAL_DrawStringMSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    const char *text,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent,
    uint8_t spacing
);
uint8_t ST7920_12864_HAL_DrawStringLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    const char *text,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent,
    uint8_t spacing
);

#ifdef __cplusplus
}
#endif

#endif

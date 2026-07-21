#ifndef ST7920_12864_HAL_H_
#define ST7920_12864_HAL_H_

#include "stm32f0xx_hal.h"

typedef enum {
    ST7920_12864_HAL_STATE_Uninitialized,
    ST7920_12864_HAL_STATE_Ready,
    ST7920_12864_HAL_STATE_InitDisplayWaitReset,
    ST7920_12864_HAL_STATE_InitDisplayWaitAfterReset,
    ST7920_12864_HAL_STATE_InitDisplayWaitFunctionSet,
    ST7920_12864_HAL_STATE_InitDisplayWaitAfterFunctionSet,
    ST7920_12864_HAL_STATE_InitDisplayWaitDisplayClear,
    ST7920_12864_HAL_STATE_InitDisplayWaitAfterDisplayClear,
    ST7920_12864_HAL_STATE_InitDisplayWaitExtendedInstructionSet,
    ST7920_12864_HAL_STATE_InitDisplayWaitAfterExtendedInstructionSet,
    ST7920_12864_HAL_STATE_InitDisplayWaitGraphicDisplayOn,
    ST7920_12864_HAL_STATE_InitDisplayWaitAfterGraphicDisplayOn,
    ST7920_12864_HAL_STATE_RenderWaitSetAddressTop,
    ST7920_12864_HAL_STATE_RenderWaitPixelTop,
    ST7920_12864_HAL_STATE_RenderWaitSetAddressBottom,
    ST7920_12864_HAL_STATE_RenderWaitPixelBottom
} ST7920_12864_HAL_State;

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
    ST7920_12864_HAL_State state;
    uint32_t lastTick;
    uint8_t y;
    uint8_t xByte;
    uint16_t renderTicks;
    uint32_t lastRenderTick;
} ST7920_12864_HAL_HandleTypeDef;

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
    int x,
    int y,
    int w,
    uint8_t color
);
void ST7920_12864_HAL_DrawFastVLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int h,
    uint8_t color
);
void ST7920_12864_HAL_DrawLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x0,
    int y0,
    int x1,
    int y1,
    uint8_t color
);
void ST7920_12864_HAL_DrawRect
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    uint8_t color
);
void ST7920_12864_HAL_FillRect
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    uint8_t color
);
void ST7920_12864_HAL_DrawCircle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int xc,
    int yc,
    int r,
    uint8_t color
);
void ST7920_12864_HAL_FillCircle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int xc,
    int yc,
    int r,
    uint8_t color
);
void ST7920_12864_HAL_DrawThickLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x0,
    int y0,
    int x1,
    int y1,
    uint8_t thickness,
    uint8_t color
);
void ST7920_12864_HAL_DrawBitmap
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
);
void ST7920_12864_HAL_DrawBitmapLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
);
void ST7920_12864_HAL_DrawBitmapColumns
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
);
void ST7920_12864_HAL_DrawBitmapColumnsLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
);
void ST7920_12864_HAL_DrawChar
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    char c,
    const uint8_t *font,
    uint8_t fontWidth,
    uint8_t fontHeight,
    uint8_t firstChar,
    uint8_t lastChar,
    uint8_t color,
    uint8_t transparent
);
void ST7920_12864_HAL_DrawString
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    const char *text,
    const uint8_t *font,
    uint8_t fontWidth,
    uint8_t fontHeight,
    uint8_t firstChar,
    uint8_t lastChar,
    uint8_t color,
    uint8_t transparent,
    uint8_t spacing
);
void ST7920_12864_HAL_DrawCharLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    char c,
    const uint8_t *font,
    uint8_t fontWidth,
    uint8_t fontHeight,
    uint8_t firstChar,
    uint8_t lastChar,
    uint8_t color,
    uint8_t transparent
);
void ST7920_12864_HAL_DrawStringLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    const char *text,
    const uint8_t *font,
    uint8_t fontWidth,
    uint8_t fontHeight,
    uint8_t firstChar,
    uint8_t lastChar,
    uint8_t color,
    uint8_t transparent,
    uint8_t spacing
);

#endif

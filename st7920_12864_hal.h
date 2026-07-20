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
    uint8_t txBuffer[6];
    uint8_t txBufferSize;
    ST7920_12864_HAL_State state;
    uint32_t lastTick;
    uint8_t y;
    uint8_t xByte;
} ST7920_12864_HAL_HandleTypeDef;

void ST7920_12864_HAL_InitHandle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *csPort,
    uint16_t csPin,
    GPIO_TypeDef *rstPort,
    uint16_t rstPin
);

void ST7920_12864_HAL_StartInitDisplay(ST7920_12864_HAL_HandleTypeDef *lcd);
uint8_t ST7920_12864_HAL_IsReady(ST7920_12864_HAL_HandleTypeDef *lcd);
uint8_t ST7920_12864_HAL_HandleState(ST7920_12864_HAL_HandleTypeDef *lcd);
void ST7920_12864_HAL_ClearBuffer(ST7920_12864_HAL_HandleTypeDef *lcd);
void ST7920_12864_HAL_SetPixel(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y, uint8_t state);
void ST7920_12864_HAL_StartRender(ST7920_12864_HAL_HandleTypeDef *lcd);

#endif

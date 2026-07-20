#include <st7920_12864_hal.h>
#include <string.h>

void ST7920_12864_HAL_InitHandle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *csPort,
    uint16_t csPin,
    GPIO_TypeDef *rstPort,
    uint16_t rstPin
)
{
    memset(lcd, 0, sizeof(*lcd));
    lcd->spi = hspi;
    lcd->csPort = csPort;
    lcd->csPin = csPin;
    lcd->rstPort = rstPort;
    lcd->rstPin = rstPin;
    lcd->state = ST7920_12864_HAL_STATE_Ready;
}

uint8_t ST7920_12864_HAL_StartWrite(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    return HAL_SPI_Transmit_IT(lcd->spi, lcd->txBuffer, lcd->txBufferSize);
}

uint8_t ST7920_12864_HAL_StartWriteCommand(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t command)
{
    lcd->txBuffer[0] = 0xF8;
    lcd->txBuffer[1] = command & 0xF0;
    lcd->txBuffer[2] = command << 4;
    lcd->txBufferSize = 3;
    return ST7920_12864_HAL_StartWrite(lcd);
}

uint8_t ST7920_12864_HAL_StartWriteData(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t data)
{
    lcd->txBuffer[0] = 0xFA;
    lcd->txBuffer[1] = data & 0xF0;
    lcd->txBuffer[2] = data << 4;
    lcd->txBufferSize = 3;
    return ST7920_12864_HAL_StartWrite(lcd);
}

uint8_t ST7920_12864_HAL_StartSetGDRAMAddress(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y)
{
    uint8_t yCommand = (0x80 | y);
    uint8_t xCommand = (0x80 | x);
    lcd->txBuffer[0] = 0xF8;
    lcd->txBuffer[1] = yCommand & 0xF0;
    lcd->txBuffer[2] = yCommand << 4;
    lcd->txBuffer[3] = 0xF8;
    lcd->txBuffer[4] = xCommand & 0xF0;
    lcd->txBuffer[5] = xCommand << 4;
    lcd->txBufferSize = 6;
    return ST7920_12864_HAL_StartWrite(lcd);
}

void ST7920_12864_HAL_StartSetGDRAMAddressTop(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y)
{
    ST7920_12864_HAL_StartSetGDRAMAddress(lcd, x, y);
}

void ST7920_12864_HAL_StartSetGDRAMAddressBottom(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y)
{
    ST7920_12864_HAL_StartSetGDRAMAddress(lcd, (x + 8), (y - 32));
}

void ST7920_12864_HAL_StartInitDisplay(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    lcd->rstPort->BRR = lcd->rstPin;
    lcd->csPort->BSRR = lcd->csPin;
    lcd->lastTick = HAL_GetTick();
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitReset;
}

void ST7920_12864_HAL_HandleInitDisplayWaitReset(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 100) return;
    lcd->rstPort->BSRR = lcd->rstPin;
    lcd->lastTick = HAL_GetTick();
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitAfterReset;
}

void ST7920_12864_HAL_HandleInitDisplayWaitAfterReset(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 10) return;
    ST7920_12864_HAL_StartWriteCommand(lcd, 0x30);
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitFunctionSet;
}

void ST7920_12864_HAL_HandleInitDisplayWaitFunctionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitAfterFunctionSet;
}

void ST7920_12864_HAL_HandleInitDisplayWaitAfterFunctionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 1) return;
    ST7920_12864_HAL_StartWriteCommand(lcd, 0x01);
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitDisplayClear;
}

void ST7920_12864_HAL_HandleInitDisplayWaitDisplayClear(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitAfterDisplayClear;
}

void ST7920_12864_HAL_HandleInitDisplayWaitAfterDisplayClear(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 3) return;
    ST7920_12864_HAL_StartWriteCommand(lcd, 0x34);
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitExtendedInstructionSet;
}

void ST7920_12864_HAL_HandleInitDisplayWaitExtendedInstructionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitAfterExtendedInstructionSet;
}

void ST7920_12864_HAL_HandleInitDisplayWaitAfterExtendedInstructionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 1) return;
    ST7920_12864_HAL_StartWriteCommand(lcd, 0x36);
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitGraphicDisplayOn;
}

void ST7920_12864_HAL_HandleInitDisplayWaitGraphicDisplayOn(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = ST7920_12864_HAL_STATE_InitDisplayWaitAfterGraphicDisplayOn;
}

void ST7920_12864_HAL_HandleInitDisplayWaitAfterGraphicDisplayOn(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 1) return;
    lcd->csPort->BRR = lcd->csPin;
    lcd->state = ST7920_12864_HAL_STATE_Ready;
}

void ST7920_12864_HAL_StartRender(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (lcd->state != ST7920_12864_HAL_STATE_Ready) return;
    lcd->csPort->BSRR = lcd->csPin;
    lcd->y = 0;
    lcd->xByte = 0;
    ST7920_12864_HAL_StartSetGDRAMAddressTop(lcd, 0, 0);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitSetAddressTop;
}

void ST7920_12864_HAL_HandleRenderWaitSetAddressTop(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    uint16_t idx = (uint16_t)lcd->y * 16;
    ST7920_12864_HAL_StartWriteData(lcd, lcd->pixelBuffer[idx]);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelTop;
}

void ST7920_12864_HAL_HandleRenderWaitPixelTop(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;

    ++lcd->xByte;

    if (lcd->xByte == 16)
    {
        lcd->xByte = 0;
        ++lcd->y;

        if (lcd->y == 32)
        {
            ST7920_12864_HAL_StartSetGDRAMAddressBottom(lcd, 0, lcd->y);
            lcd->state = ST7920_12864_HAL_STATE_RenderWaitSetAddressBottom;
        }
        else
        {
            ST7920_12864_HAL_StartSetGDRAMAddressTop(lcd, 0, lcd->y);
            lcd->state = ST7920_12864_HAL_STATE_RenderWaitSetAddressTop;
        }

        return;
    }

    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    ST7920_12864_HAL_StartWriteData(lcd, lcd->pixelBuffer[idx]);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelTop;
}

void ST7920_12864_HAL_HandleRenderWaitSetAddressBottom(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    uint16_t idx = (uint16_t)lcd->y * 16;
    ST7920_12864_HAL_StartWriteData(lcd, lcd->pixelBuffer[idx]);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelBottom;
}

void ST7920_12864_HAL_HandleRenderWaitPixelBottom(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;

    ++lcd->xByte;

    if (lcd->xByte == 16)
    {
        lcd->xByte = 0;
        ++lcd->y;

        if (lcd->y == 64)
        {
            lcd->csPort->BRR = lcd->csPin;
            lcd->state = ST7920_12864_HAL_STATE_Ready;
        }
        else
        {
            ST7920_12864_HAL_StartSetGDRAMAddressBottom(lcd, 0, lcd->y);
            lcd->state = ST7920_12864_HAL_STATE_RenderWaitSetAddressBottom;
        }

        return;
    }

    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    ST7920_12864_HAL_StartWriteData(lcd, lcd->pixelBuffer[idx]);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelBottom;
}

void ST7920_12864_HAL_ClearBuffer(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    memset(lcd->pixelBuffer, 0x00, sizeof(lcd->pixelBuffer));
}

void ST7920_12864_HAL_SetPixel(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y, uint8_t state)
{
    uint16_t byteIdx = (uint16_t)y * 16 + (x / 8);
    uint8_t  bitPos  = 7 - (x % 8);

    if (state)
        lcd->pixelBuffer[byteIdx] |=  (1 << bitPos);
    else
        lcd->pixelBuffer[byteIdx] &= ~(1 << bitPos);
}

uint8_t ST7920_12864_HAL_HandleState(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    switch (lcd->state)
    {
        case ST7920_12864_HAL_STATE_Uninitialized: return 0;
        case ST7920_12864_HAL_STATE_Ready: return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitReset:
            ST7920_12864_HAL_HandleInitDisplayWaitReset(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitAfterReset:
            ST7920_12864_HAL_HandleInitDisplayWaitAfterReset(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitFunctionSet:
            ST7920_12864_HAL_HandleInitDisplayWaitFunctionSet(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitAfterFunctionSet:
            ST7920_12864_HAL_HandleInitDisplayWaitAfterFunctionSet(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitDisplayClear:
            ST7920_12864_HAL_HandleInitDisplayWaitDisplayClear(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitAfterDisplayClear:
            ST7920_12864_HAL_HandleInitDisplayWaitAfterDisplayClear(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitExtendedInstructionSet:
            ST7920_12864_HAL_HandleInitDisplayWaitExtendedInstructionSet(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitAfterExtendedInstructionSet:
            ST7920_12864_HAL_HandleInitDisplayWaitAfterExtendedInstructionSet(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitGraphicDisplayOn:
            ST7920_12864_HAL_HandleInitDisplayWaitGraphicDisplayOn(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_InitDisplayWaitAfterGraphicDisplayOn:
            ST7920_12864_HAL_HandleInitDisplayWaitAfterGraphicDisplayOn(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_RenderWaitSetAddressTop:
            ST7920_12864_HAL_HandleRenderWaitSetAddressTop(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_RenderWaitPixelTop:
            ST7920_12864_HAL_HandleRenderWaitPixelTop(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_RenderWaitSetAddressBottom:
            ST7920_12864_HAL_HandleRenderWaitSetAddressBottom(lcd);
            return 0;
        case ST7920_12864_HAL_STATE_RenderWaitPixelBottom:
            ST7920_12864_HAL_HandleRenderWaitPixelBottom(lcd);
            return 0;
    }

    return lcd->state == ST7920_12864_HAL_STATE_Ready;
}

uint8_t ST7920_12864_HAL_IsReady(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    return lcd->state == ST7920_12864_HAL_STATE_Ready;
}


#include <st7920_12864_hal.h>
#include <stdlib.h>
#include <string.h>

void ST7920_12864_HAL_InitHandle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *csPort,
    uint16_t csPin,
    GPIO_TypeDef *rstPort,
    uint16_t rstPin,
    uint16_t renderTicks
)
{
    memset(lcd, 0, sizeof(*lcd));
    lcd->spi = hspi;
    lcd->csPort = csPort;
    lcd->csPin = csPin;
    lcd->rstPort = rstPort;
    lcd->rstPin = rstPin;
    lcd->renderTicks = renderTicks;
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
    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    uint8_t byte2 = lcd->pixelBuffer[idx + 1];
    uint8_t byte3 = lcd->pixelBuffer[idx + 2];
    uint8_t byte4 = lcd->pixelBuffer[idx + 3];
    lcd->txBuffer[0] = 0xFA;
    lcd->txBuffer[1] = lcd->pixelBuffer[idx] & 0xF0;
    lcd->txBuffer[2] = lcd->pixelBuffer[idx] << 4;
    lcd->txBuffer[3] = 0xFA;
    lcd->txBuffer[4] = byte2 & 0xF0;
    lcd->txBuffer[5] = byte2 << 4;
    lcd->txBuffer[6] = 0xFA;
    lcd->txBuffer[7] = byte3 & 0xF0;
    lcd->txBuffer[8] = byte3 << 4;
    lcd->txBuffer[9] = 0xFA;
    lcd->txBuffer[10] = byte4 & 0xF0;
    lcd->txBuffer[11] = byte4 << 4;
    lcd->txBufferSize = 12;
    ST7920_12864_HAL_StartWrite(lcd);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelTop;
}

void ST7920_12864_HAL_HandleRenderWaitPixelTop(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;

    lcd->xByte += 4;

    if (lcd->xByte >= 16)
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
    uint8_t byte2 = lcd->pixelBuffer[idx + 1];
    uint8_t byte3 = lcd->pixelBuffer[idx + 2];
    uint8_t byte4 = lcd->pixelBuffer[idx + 3];
    lcd->txBuffer[0] = 0xFA;
    lcd->txBuffer[1] = lcd->pixelBuffer[idx] & 0xF0;
    lcd->txBuffer[2] = lcd->pixelBuffer[idx] << 4;
    lcd->txBuffer[3] = 0xFA;
    lcd->txBuffer[4] = byte2 & 0xF0;
    lcd->txBuffer[5] = byte2 << 4;
    lcd->txBuffer[6] = 0xFA;
    lcd->txBuffer[7] = byte3 & 0xF0;
    lcd->txBuffer[8] = byte3 << 4;
    lcd->txBuffer[9] = 0xFA;
    lcd->txBuffer[10] = byte4 & 0xF0;
    lcd->txBuffer[11] = byte4 << 4;
    lcd->txBufferSize = 12;
    ST7920_12864_HAL_StartWrite(lcd);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelTop;
}

void ST7920_12864_HAL_HandleRenderWaitSetAddressBottom(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    uint8_t byte2 = lcd->pixelBuffer[idx + 1];
    uint8_t byte3 = lcd->pixelBuffer[idx + 2];
    uint8_t byte4 = lcd->pixelBuffer[idx + 3];
    lcd->txBuffer[0] = 0xFA;
    lcd->txBuffer[1] = lcd->pixelBuffer[idx] & 0xF0;
    lcd->txBuffer[2] = lcd->pixelBuffer[idx] << 4;
    lcd->txBuffer[3] = 0xFA;
    lcd->txBuffer[4] = byte2 & 0xF0;
    lcd->txBuffer[5] = byte2 << 4;
    lcd->txBuffer[6] = 0xFA;
    lcd->txBuffer[7] = byte3 & 0xF0;
    lcd->txBuffer[8] = byte3 << 4;
    lcd->txBuffer[9] = 0xFA;
    lcd->txBuffer[10] = byte4 & 0xF0;
    lcd->txBuffer[11] = byte4 << 4;
    lcd->txBufferSize = 12;
    ST7920_12864_HAL_StartWrite(lcd);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelBottom;
}

void ST7920_12864_HAL_HandleRenderWaitPixelBottom(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;

    lcd->xByte += 4;

    if (lcd->xByte >= 16)
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
    uint8_t byte2 = lcd->pixelBuffer[idx + 1];
    uint8_t byte3 = lcd->pixelBuffer[idx + 2];
    uint8_t byte4 = lcd->pixelBuffer[idx + 3];
    lcd->txBuffer[0] = 0xFA;
    lcd->txBuffer[1] = lcd->pixelBuffer[idx] & 0xF0;
    lcd->txBuffer[2] = lcd->pixelBuffer[idx] << 4;
    lcd->txBuffer[3] = 0xFA;
    lcd->txBuffer[4] = byte2 & 0xF0;
    lcd->txBuffer[5] = byte2 << 4;
    lcd->txBuffer[6] = 0xFA;
    lcd->txBuffer[7] = byte3 & 0xF0;
    lcd->txBuffer[8] = byte3 << 4;
    lcd->txBuffer[9] = 0xFA;
    lcd->txBuffer[10] = byte4 & 0xF0;
    lcd->txBuffer[11] = byte4 << 4;
    lcd->txBufferSize = 12;
    ST7920_12864_HAL_StartWrite(lcd);
    lcd->state = ST7920_12864_HAL_STATE_RenderWaitPixelBottom;
}

void ST7920_12864_HAL_ClearBuffer(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    memset(lcd->pixelBuffer, 0x00, sizeof(lcd->pixelBuffer));
}

void ST7920_12864_HAL_SetPixel(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y, uint8_t color)
{
    uint16_t byteIdx = (uint16_t)y * 16 + (x / 8);
    uint8_t  bitPos  = 7 - (x % 8);

    if (color)
        lcd->pixelBuffer[byteIdx] |=  (1 << bitPos);
    else
        lcd->pixelBuffer[byteIdx] &= ~(1 << bitPos);
}

void ST7920_12864_HAL_HandleReady(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastRenderTick < lcd->renderTicks) return;
    lcd->lastRenderTick = HAL_GetTick();
    ST7920_12864_HAL_StartRender(lcd);
}

uint8_t ST7920_12864_HAL_HandleState(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    switch (lcd->state)
    {
        case ST7920_12864_HAL_STATE_Uninitialized: return 0;
        case ST7920_12864_HAL_STATE_Ready:
            ST7920_12864_HAL_HandleReady(lcd);
            return 1;
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

    return 0;
}

uint8_t ST7920_12864_HAL_IsReady(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    return lcd->state == ST7920_12864_HAL_STATE_Ready;
}

void ST7920_12864_HAL_DrawFastHLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    uint8_t color
)
{
    if (w <= 0)
        return;

    int xEnd = x + w - 1;

    uint16_t startByte = y * 16 + (x >> 3);
    uint16_t endByte   = y * 16 + (xEnd >> 3);

    uint8_t startMask = 0xFF >> (x & 7);
    uint8_t endMask   = 0xFF << (7 - (xEnd & 7));

    if (startByte == endByte)
    {
        uint8_t mask = startMask & endMask;

        if (color)
            lcd->pixelBuffer[startByte] |= mask;
        else
            lcd->pixelBuffer[startByte] &= ~mask;

        return;
    }

    if (color)
        lcd->pixelBuffer[startByte] |= startMask;
    else
        lcd->pixelBuffer[startByte] &= ~startMask;

    for (uint16_t i = startByte + 1; i < endByte; ++i)
    {
        lcd->pixelBuffer[i] = color ? 0xFF : 0x00;
    }

    if (color)
        lcd->pixelBuffer[endByte] |= endMask;
    else
        lcd->pixelBuffer[endByte] &= ~endMask;
}

void ST7920_12864_HAL_DrawFastVLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int h,
    uint8_t color
)
{
    for (int yy = y; yy < y + h; ++yy)
    {
        ST7920_12864_HAL_SetPixel(lcd, x, yy, color);
    }
}

void ST7920_12864_HAL_DrawLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x0,
    int y0,
    int x1,
    int y1,
    uint8_t color
)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1)
    {
        ST7920_12864_HAL_SetPixel(lcd, x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err * 2;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void ST7920_12864_HAL_DrawRect
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    uint8_t color
)
{
    ST7920_12864_HAL_DrawFastHLine(lcd, x, y, w, color);

    if (h > 1)
        ST7920_12864_HAL_DrawFastHLine(lcd, x, y + h - 1, w, color);

    if (h > 2)
    {
        ST7920_12864_HAL_DrawFastVLine(lcd, x, y + 1, h - 2, color);
        ST7920_12864_HAL_DrawFastVLine(lcd, x + w - 1, y + 1, h - 2, color);
    }
}

void ST7920_12864_HAL_FillRect
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    uint8_t color
)
{
    for (int yy = y; yy < y + h; ++yy)
    {
        ST7920_12864_HAL_DrawFastHLine(lcd, x, yy, w, color);
    }
}

void ST7920_12864_HAL_DrawCircle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int xc,
    int yc,
    int r,
    uint8_t color
)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x)
    {
        ST7920_12864_HAL_SetPixel(lcd, xc + x, yc + y, color);
        ST7920_12864_HAL_SetPixel(lcd, xc - x, yc + y, color);
        ST7920_12864_HAL_SetPixel(lcd, xc + x, yc - y, color);
        ST7920_12864_HAL_SetPixel(lcd, xc - x, yc - y, color);

        ST7920_12864_HAL_SetPixel(lcd, xc + y, yc + x, color);
        ST7920_12864_HAL_SetPixel(lcd, xc - y, yc + x, color);
        ST7920_12864_HAL_SetPixel(lcd, xc + y, yc - x, color);
        ST7920_12864_HAL_SetPixel(lcd, xc - y, yc - x, color);

        ++x;

        if (d > 0)
        {
            --y;
            d += 4 * (x - y) + 10;
        }
        else
        {
            d += 4 * x + 6;
        }
    }
}

void ST7920_12864_HAL_FillCircle
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int xc,
    int yc,
    int r,
    uint8_t color
)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x)
    {
        ST7920_12864_HAL_DrawLine(lcd, xc - x, yc - y, xc + x, yc - y, color);
        ST7920_12864_HAL_DrawLine(lcd, xc - y, yc - x, xc + y, yc - x, color);
        ST7920_12864_HAL_DrawLine(lcd, xc - y, yc + x, xc + y, yc + x, color);
        ST7920_12864_HAL_DrawLine(lcd, xc - x, yc + y, xc + x, yc + y, color);

        ++x;

        if (d > 0)
        {
            --y;
            d += 4 * (x - y) + 10;
        }
        else
        {
            d += 4 * x + 6;
        }
    }
}

void ST7920_12864_HAL_DrawThickLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x0,
    int y0,
    int x1,
    int y1,
    uint8_t thickness,
    uint8_t color
)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    int r = thickness / 2;

    while (1)
    {
        ST7920_12864_HAL_FillCircle(lcd, x0, y0, r, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err * 2;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void ST7920_12864_HAL_DrawBitmap
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
)
{
    int bytesPerRow = (w + 7) / 8;

    for (int yy = 0; yy < h; ++yy)
    {
        for (int xx = 0; xx < w; ++xx)
        {
            uint8_t byte =
                bmp[yy * bytesPerRow + xx / 8];

            if (byte & (0x80 >> (xx & 7)))
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + xx,
                    y + yy,
                    1
                );
            }
        }
    }
}

void ST7920_12864_HAL_DrawBitmapLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
)
{
    int bytesPerRow = (w + 7) / 8;

    for (int yy = 0; yy < h; ++yy)
    {
        for (int xx = 0; xx < w; ++xx)
        {
            uint8_t byte = bmp[yy * bytesPerRow + xx / 8];

            if (byte & (1 << (xx & 7)))
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + xx,
                    y + yy,
                    1
                );
            }
        }
    }
}

void ST7920_12864_HAL_DrawBitmapColumns
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
)
{
    int bytesPerColumn = (h + 7) / 8;

    for (int xx = 0; xx < w; ++xx)
    {
        for (int yy = 0; yy < h; ++yy)
        {
            uint8_t byte = bmp[xx * bytesPerColumn + yy / 8];

            if (byte & (0x80 >> (yy & 7)))
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + xx,
                    y + yy,
                    1
                );
            }
        }
    }
}

void ST7920_12864_HAL_DrawBitmapColumnsLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    int x,
    int y,
    int w,
    int h,
    const uint8_t *bmp
)
{
    int bytesPerColumn = (h + 7) / 8;

    for (int xx = 0; xx < w; ++xx)
    {
        for (int yy = 0; yy < h; ++yy)
        {
            uint8_t byte = bmp[xx * bytesPerColumn + yy / 8];

            if (byte & (1 << (yy & 7)))
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + xx,
                    y + yy,
                    1
                );
            }
        }
    }
}

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
)
{
    if ((uint8_t)c < firstChar || (uint8_t)c > lastChar)
        c = '?';

    uint32_t offset = ((uint8_t)c - firstChar) * fontWidth;

    for (uint8_t col = 0; col < fontWidth; col++)
    {
        uint8_t bits = font[offset + col];

        for (uint8_t row = 0; row < fontHeight; row++)
        {
            uint8_t pixel = bits & (1 << (7 - row));

            if (pixel)
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + col,
                    y + row,
                    color
                );
            }
            else if (!transparent)
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + col,
                    y + row,
                    !color
                );
            }
        }
    }
}

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
)
{
    int startX = x;

    while (*text)
    {
        if (*text == '\n')
        {
            x = startX;
            y += fontHeight + spacing;
            text++;
            continue;
        }

        ST7920_12864_HAL_DrawChar
        (
            lcd,
            x,
            y,
            *text,
            font,
            fontWidth,
            fontHeight,
            firstChar,
            lastChar,
            color,
            transparent
        );

        x += fontWidth + spacing;
        text++;
    }
}

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
)
{
    if ((uint8_t)c < firstChar || (uint8_t)c > lastChar)
        c = '?';

    uint32_t offset = ((uint8_t)c - firstChar) * fontWidth;

    for (uint8_t col = 0; col < fontWidth; col++)
    {
        uint8_t bits = font[offset + col];

        for (uint8_t row = 0; row < fontHeight; row++)
        {
            uint8_t pixel = bits & (1 << row);

            if (pixel)
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + col,
                    y + row,
                    color
                );
            }
            else if (!transparent)
            {
                ST7920_12864_HAL_SetPixel
                (
                    lcd,
                    x + col,
                    y + row,
                    !color
                );
            }
        }
    }
}

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
)
{
    int startX = x;

    while (*text)
    {
        if (*text == '\n')
        {
            x = startX;
            y += fontHeight + spacing;
            text++;
            continue;
        }

        ST7920_12864_HAL_DrawCharLSB
        (
            lcd,
            x,
            y,
            *text,
            font,
            fontWidth,
            fontHeight,
            firstChar,
            lastChar,
            color,
            transparent
        );

        x += fontWidth + spacing;
        text++;
    }
}


#include <st7920_12864_hal.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_PER_WRITE 4

typedef enum {
    Uninitialized,
    Ready,
    InitDisplayWaitReset,
    InitDisplayWaitAfterReset,
    InitDisplayWaitFunctionSet,
    InitDisplayWaitAfterFunctionSet,
    InitDisplayWaitDisplayClear,
    InitDisplayWaitAfterDisplayClear,
    InitDisplayWaitExtendedInstructionSet,
    InitDisplayWaitAfterExtendedInstructionSet,
    InitDisplayWaitGraphicDisplayOn,
    InitDisplayWaitAfterGraphicDisplayOn,
    RenderWaitSetAddressTop,
    RenderWaitPixelTop,
    RenderWaitSetAddressBottom,
    RenderWaitPixelBottom
} State;

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
    lcd->state = Ready;
}

void SetNextData(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t byte)
{
    lcd->txBuffer[lcd->txBufferSize] = 0xFA;
    lcd->txBuffer[lcd->txBufferSize + 1] = byte & 0xF0;
    lcd->txBuffer[lcd->txBufferSize + 2] = byte << 4;
    lcd->txBufferSize += 3;
}

uint8_t StartWrite(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    return HAL_SPI_Transmit_IT(lcd->spi, lcd->txBuffer, lcd->txBufferSize);
}

uint8_t StartWriteCommand(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t command)
{
    lcd->txBuffer[0] = 0xF8;
    lcd->txBuffer[1] = command & 0xF0;
    lcd->txBuffer[2] = command << 4;
    lcd->txBufferSize = 3;
    return StartWrite(lcd);
}

uint8_t StartSetGDRAMAddress(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y)
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
    return StartWrite(lcd);
}

void StartSetGDRAMAddressTop(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y)
{
    StartSetGDRAMAddress(lcd, x, y);
}

void StartSetGDRAMAddressBottom(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y)
{
    StartSetGDRAMAddress(lcd, (x + 8), (y - 32));
}

void ST7920_12864_HAL_StartInitDisplay(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    lcd->rstPort->BRR = lcd->rstPin;
    lcd->csPort->BSRR = lcd->csPin;
    lcd->lastTick = HAL_GetTick();
    lcd->state = InitDisplayWaitReset;
}

void HandleInitDisplayWaitReset(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 100) return;
    lcd->rstPort->BSRR = lcd->rstPin;
    lcd->lastTick = HAL_GetTick();
    lcd->state = InitDisplayWaitAfterReset;
}

void HandleInitDisplayWaitAfterReset(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 10) return;
    StartWriteCommand(lcd, 0x30);
    lcd->state = InitDisplayWaitFunctionSet;
}

void HandleInitDisplayWaitFunctionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = InitDisplayWaitAfterFunctionSet;
}

void HandleInitDisplayWaitAfterFunctionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 1) return;
    StartWriteCommand(lcd, 0x01);
    lcd->state = InitDisplayWaitDisplayClear;
}

void HandleInitDisplayWaitDisplayClear(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = InitDisplayWaitAfterDisplayClear;
}

void HandleInitDisplayWaitAfterDisplayClear(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 3) return;
    StartWriteCommand(lcd, 0x34);
    lcd->state = InitDisplayWaitExtendedInstructionSet;
}

void HandleInitDisplayWaitExtendedInstructionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = InitDisplayWaitAfterExtendedInstructionSet;
}

void HandleInitDisplayWaitAfterExtendedInstructionSet(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 1) return;
    StartWriteCommand(lcd, 0x36);
    lcd->state = InitDisplayWaitGraphicDisplayOn;
}

void HandleInitDisplayWaitGraphicDisplayOn(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    lcd->lastTick = HAL_GetTick();
    lcd->state = InitDisplayWaitAfterGraphicDisplayOn;
}

void HandleInitDisplayWaitAfterGraphicDisplayOn(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastTick < 1) return;
    lcd->csPort->BRR = lcd->csPin;
    lcd->state = Ready;
}

void StartRender(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (lcd->state != Ready) return;
    lcd->csPort->BSRR = lcd->csPin;
    lcd->y = 0;
    lcd->xByte = 0;
    StartSetGDRAMAddressTop(lcd, 0, 0);
    lcd->state = RenderWaitSetAddressTop;
}

void HandleRenderWaitSetAddressTop(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    lcd->txBufferSize = 0;
    for (uint8_t i = 0; i < BYTES_PER_WRITE; ++i)
    {
        SetNextData(lcd, lcd->pixelBuffer[idx + i]);
    }
    StartWrite(lcd);
    lcd->state = RenderWaitPixelTop;
}

void HandleRenderWaitPixelTop(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;

    lcd->xByte += BYTES_PER_WRITE;

    if (lcd->xByte >= 16)
    {
        lcd->xByte = 0;
        ++lcd->y;

        if (lcd->y >= 32)
        {
            StartSetGDRAMAddressBottom(lcd, 0, lcd->y);
            lcd->state = RenderWaitSetAddressBottom;
        }
        else
        {
            StartSetGDRAMAddressTop(lcd, 0, lcd->y);
            lcd->state = RenderWaitSetAddressTop;
        }

        return;
    }

    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    lcd->txBufferSize = 0;
    for (uint8_t i = 0; i < BYTES_PER_WRITE; ++i)
    {
        SetNextData(lcd, lcd->pixelBuffer[idx + i]);
    }
    StartWrite(lcd);
    lcd->state = RenderWaitPixelTop;
}

void HandleRenderWaitSetAddressBottom(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;
    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    lcd->txBufferSize = 0;
    for (uint8_t i = 0; i < BYTES_PER_WRITE; ++i)
    {
        SetNextData(lcd, lcd->pixelBuffer[idx + i]);
    }
    StartWrite(lcd);
    lcd->state = RenderWaitPixelBottom;
}

void HandleRenderWaitPixelBottom(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_SPI_GetState(lcd->spi) != HAL_SPI_STATE_READY) return;

    lcd->xByte += BYTES_PER_WRITE;

    if (lcd->xByte >= 16)
    {
        lcd->xByte = 0;
        ++lcd->y;

        if (lcd->y >= 64)
        {
            lcd->csPort->BRR = lcd->csPin;
            lcd->state = Ready;
        }
        else
        {
            StartSetGDRAMAddressBottom(lcd, 0, lcd->y);
            lcd->state = RenderWaitSetAddressBottom;
        }

        return;
    }

    uint16_t idx = (uint16_t)lcd->y * 16 + lcd->xByte;
    lcd->txBufferSize = 0;
    for (uint8_t i = 0; i < BYTES_PER_WRITE; ++i)
    {
        SetNextData(lcd, lcd->pixelBuffer[idx + i]);
    }
    StartWrite(lcd);
    lcd->state = RenderWaitPixelBottom;
}

void ST7920_12864_HAL_ClearBuffer(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    memset(lcd->pixelBuffer, 0x00, sizeof(lcd->pixelBuffer));
}

void SetPixel(ST7920_12864_HAL_HandleTypeDef *lcd, uint8_t x, uint8_t y, uint8_t color)
{
    uint16_t byteIdx = (uint16_t)y * 16 + (x / 8);
    uint8_t  bitPos  = 7 - (x % 8);
    if (color) lcd->pixelBuffer[byteIdx] |=  (1 << bitPos);
    else lcd->pixelBuffer[byteIdx] &= ~(1 << bitPos);
}

void HandleReady(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    if (HAL_GetTick() - lcd->lastRenderTick < lcd->renderTicks) return;
    lcd->lastRenderTick = HAL_GetTick();
    StartRender(lcd);
}

uint8_t ST7920_12864_HAL_HandleState(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    switch ((State)lcd->state)
    {
        case Uninitialized: return 0;
        case Ready:
            HandleReady(lcd);
            return 1;
        case InitDisplayWaitReset:
            HandleInitDisplayWaitReset(lcd);
            return 0;
        case InitDisplayWaitAfterReset:
            HandleInitDisplayWaitAfterReset(lcd);
            return 0;
        case InitDisplayWaitFunctionSet:
            HandleInitDisplayWaitFunctionSet(lcd);
            return 0;
        case InitDisplayWaitAfterFunctionSet:
            HandleInitDisplayWaitAfterFunctionSet(lcd);
            return 0;
        case InitDisplayWaitDisplayClear:
            HandleInitDisplayWaitDisplayClear(lcd);
            return 0;
        case InitDisplayWaitAfterDisplayClear:
            HandleInitDisplayWaitAfterDisplayClear(lcd);
            return 0;
        case InitDisplayWaitExtendedInstructionSet:
            HandleInitDisplayWaitExtendedInstructionSet(lcd);
            return 0;
        case InitDisplayWaitAfterExtendedInstructionSet:
            HandleInitDisplayWaitAfterExtendedInstructionSet(lcd);
            return 0;
        case InitDisplayWaitGraphicDisplayOn:
            HandleInitDisplayWaitGraphicDisplayOn(lcd);
            return 0;
        case InitDisplayWaitAfterGraphicDisplayOn:
            HandleInitDisplayWaitAfterGraphicDisplayOn(lcd);
            return 0;
        case RenderWaitSetAddressTop:
            HandleRenderWaitSetAddressTop(lcd);
            return 0;
        case RenderWaitPixelTop:
            HandleRenderWaitPixelTop(lcd);
            return 0;
        case RenderWaitSetAddressBottom:
            HandleRenderWaitSetAddressBottom(lcd);
            return 0;
        case RenderWaitPixelBottom:
            HandleRenderWaitPixelBottom(lcd);
            return 0;
    }

    return 0;
}

uint8_t ST7920_12864_HAL_IsReady(ST7920_12864_HAL_HandleTypeDef *lcd)
{
    return lcd->state == Ready;
}

void ST7920_12864_HAL_DrawFastHLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t color
)
{
    if (w <= 0)
        return;

    uint8_t xEnd = x + w - 1;

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
    uint8_t x,
    uint8_t y,
    uint8_t h,
    uint8_t color
)
{
    for (int yy = y; yy < y + h; ++yy)
    {
        SetPixel(lcd, x, yy, color);
    }
}

void ST7920_12864_HAL_DrawLine
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    uint8_t color
)
{
    uint8_t dx = abs(x1 - x0);
    uint8_t sx = x0 < x1 ? 1 : -1;
    uint8_t dy = -abs(y1 - y0);
    uint8_t sy = y0 < y1 ? 1 : -1;
    uint8_t err = dx + dy;

    while (1)
    {
        SetPixel(lcd, x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        uint8_t e2 = err * 2;

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
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
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
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
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
    uint8_t xc,
    uint8_t yc,
    uint8_t r,
    uint8_t color
)
{
    uint8_t x = 0;
    uint8_t y = r;
    uint8_t d = 3 - 2 * r;

    while (y >= x)
    {
        SetPixel(lcd, xc + x, yc + y, color);
        SetPixel(lcd, xc - x, yc + y, color);
        SetPixel(lcd, xc + x, yc - y, color);
        SetPixel(lcd, xc - x, yc - y, color);

        SetPixel(lcd, xc + y, yc + x, color);
        SetPixel(lcd, xc - y, yc + x, color);
        SetPixel(lcd, xc + y, yc - x, color);
        SetPixel(lcd, xc - y, yc - x, color);

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
    uint8_t xc,
    uint8_t yc,
    uint8_t r,
    uint8_t color
)
{
    uint8_t x = 0;
    uint8_t y = r;
    uint8_t d = 3 - 2 * r;

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
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    uint8_t thickness,
    uint8_t color
)
{
    uint8_t dx = abs(x1 - x0);
    uint8_t sx = x0 < x1 ? 1 : -1;
    uint8_t dy = -abs(y1 - y0);
    uint8_t sy = y0 < y1 ? 1 : -1;
    uint8_t err = dx + dy;

    uint8_t r = thickness / 2;

    while (1)
    {
        ST7920_12864_HAL_FillCircle(lcd, x0, y0, r, color);

        if (x0 == x1 && y0 == y1)
            break;

        uint8_t e2 = err * 2;

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
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t *bmp
)
{
    uint8_t bytesPerRow = (w + 7) / 8;

    for (int yy = 0; yy < h; ++yy)
    {
        for (int xx = 0; xx < w; ++xx)
        {
            uint8_t byte =
                bmp[yy * bytesPerRow + xx / 8];

            if (byte & (0x80 >> (xx & 7)))
            {
                SetPixel
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
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t *bmp
)
{
    uint8_t bytesPerRow = (w + 7) / 8;

    for (int yy = 0; yy < h; ++yy)
    {
        for (int xx = 0; xx < w; ++xx)
        {
            uint8_t byte = bmp[yy * bytesPerRow + xx / 8];

            if (byte & (1 << (xx & 7)))
            {
                SetPixel
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

uint8_t GetCharPixelMSB(uint8_t bits, uint8_t row)
{
    return bits & (1 << (7 - row));
}

uint8_t GetCharPixelLSB(uint8_t bits, uint8_t row)
{
    return bits & (1 << row);
}

void DrawChar
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    char c,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent,
    uint8_t (*getPixelFunc)(uint8_t bits, uint8_t row)
)
{
    if ((uint8_t)c < font->firstChar || (uint8_t)c > font->lastChar) c = '?';

    uint32_t offset = ((uint8_t)c - font->firstChar) * font->fontWidth;

    for (uint8_t col = 0; col < font->fontWidth; col++)
    {
        uint8_t bits = font->fontBytes[offset + col];

        for (uint8_t row = 0; row < font->fontHeight; row++)
        {
            uint8_t pixel = getPixelFunc(bits, row);

            if (pixel)
            {
                SetPixel
                (
                    lcd,
                    x + col,
                    y + row,
                    color
                );
            }
            else if (!transparent)
            {
                SetPixel
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

void ST7920_12864_HAL_DrawCharMSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    char c,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent
)
{
    DrawChar(lcd, x, y, c, font, color, transparent, GetCharPixelMSB);
}

void ST7920_12864_HAL_DrawCharLSB
(
    ST7920_12864_HAL_HandleTypeDef *lcd,
    uint8_t x,
    uint8_t y,
    char c,
    const ST7920_12864_HAL_FontTypeDef *font,
    uint8_t color,
    uint8_t transparent
)
{
    DrawChar(lcd, x, y, c, font, color, transparent, GetCharPixelLSB);
}

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
)
{
    uint8_t startX = x;

    while (*text)
    {
        if (*text == '\n')
        {
            x = startX;
            y += font->fontHeight + spacing;
            text++;
            continue;
        }

        ST7920_12864_HAL_DrawCharMSB
        (
            lcd,
            x,
            y,
            *text,
            font,
            color,
            transparent
        );

        x += font->fontWidth + spacing;
        text++;
    }

    return x;
}

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
)
{
    uint8_t startX = x;

    while (*text)
    {
        if (*text == '\n')
        {
            x = startX;
            y += font->fontHeight + spacing;
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
            color,
            transparent
        );

        x += font->fontWidth + spacing;
        text++;
    }

    return x;
}


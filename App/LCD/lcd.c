#include "spi.h"
#include "lcd.h"
#include "font.h"

uint8_t spi_tx_flag = 1;


// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
// {
//     if (hspi->Instance == SPI1)
//     {
//         spi_tx_flag = 0;
//     }
// }

static void LCD_Write_Cmd(uint8_t cmd)
{
    LCD_DC(0);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
}

static void LCD_Write_Byte(uint8_t data)
{
    LCD_DC(1);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
}

static void LCD_Write_HalfWord(const uint16_t h_word)
{
    LCD_DC(1);
    uint8_t data[2];
    data[0] = (h_word >> 8) & 0xFF;
    data[1] = h_word & 0xFF;
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
}

static void LCD_Write_Data(uint8_t *buff, uint32_t buff_size)
{
    while (buff_size > 0)
    {
        uint16_t chunk_size = buff_size > 4096 ? 4096 : buff_size;
#if USE_DMA
        spi_tx_flag = 1;
        SCB_CleanDCache_by_Addr((uint32_t *)buff, (chunk_size + 31) & ~31);
        __DSB();
        LCD_DC(1);
        HAL_SPI_Transmit_DMA(&hspi1, buff, chunk_size);
        while (spi_tx_flag == 1)
            __NOP();
#else
        LCD_DC(1);
        HAL_SPI_Transmit(&hspi1, buff, chunk_size, HAL_MAX_DELAY);
#endif
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

void LCD_Write_Data_16bit(uint16_t *buff, uint32_t buff_size)
{
    CLEAR_BIT(hspi1.Instance->CR1, SPI_CR1_SPE);
    while (READ_BIT(hspi1.Instance->CR1, SPI_CR1_SPE))
        ; // 等待关闭完成

    hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
    HAL_SPI_Init(&hspi1);
    while (buff_size > 0)
    {
        // 以 "数据项数" 来算 chunk size，不是字节
        uint16_t chunk_size = buff_size > 2048 ? 2048 : buff_size; // 一次最多 4096 字节 = 2048 个 uint16_t
#if USE_DMA
        spi_tx_flag = 1;
        uint32_t addr = (uint32_t)buff;
        uint32_t size = chunk_size * 2;
        SCB_CleanDCache_by_Addr((uint32_t *)(addr & ~31U), (size + 31U) & ~31U);
        __DSB();
        LCD_DC(1);
        HAL_SPI_Transmit_DMA(&hspi1, (uint16_t *)buff, chunk_size);
        while (spi_tx_flag == 1)
            __NOP();
#else
        LCD_DC(1);
        HAL_SPI_Transmit(&hspi1, (uint8_t *)buff, chunk_size, HAL_MAX_DELAY);
#endif
        buff += chunk_size;
        buff_size -= chunk_size;
    }
    CLEAR_BIT(hspi1.Instance->CR1, SPI_CR1_SPE);
    while (READ_BIT(hspi1.Instance->CR1, SPI_CR1_SPE))
        ;

    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&hspi1);

    SET_BIT(hspi1.Instance->CR1, SPI_CR1_SPE);
}

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // x2 -= 1;//0开始 172-1或320-1结束
    // y2 -= 1;
    switch (USE_HORIZONTAL)
    {
    case 0:
        LCD_Write_Cmd(0x2A); // Column addr set
        LCD_Write_HalfWord(x1 + 34);
        LCD_Write_HalfWord(x2 + 34);
        LCD_Write_Cmd(0x2B); // Row addr set
        LCD_Write_HalfWord(y1);
        LCD_Write_HalfWord(y2);
        LCD_Write_Cmd(0x2C); // Memory writeF
        break;
    case 1:
        LCD_Write_Cmd(0x2A); // Column addr set
        LCD_Write_HalfWord(x1 + 34);
        LCD_Write_HalfWord(x2 + 34);
        LCD_Write_Cmd(0x2B); // Row addr set
        LCD_Write_HalfWord(y1);
        LCD_Write_HalfWord(y2);
        LCD_Write_Cmd(0x2C); // Memory writeF
        break;
    case 2:
        LCD_Write_Cmd(0x2A); // Column addr set
        LCD_Write_HalfWord(x1);
        LCD_Write_HalfWord(x2);
        LCD_Write_Cmd(0x2B); // Row addr set
        LCD_Write_HalfWord(y1 + 34);
        LCD_Write_HalfWord(y2 + 34);
        LCD_Write_Cmd(0x2C); // Memory writeF
        break;
    case 3:
        LCD_Write_Cmd(0x2A); // Column addr set
        LCD_Write_HalfWord(x1);
        LCD_Write_HalfWord(x2);
        LCD_Write_Cmd(0x2B); // Row addr set
        LCD_Write_HalfWord(y1 + 34);
        LCD_Write_HalfWord(y2 + 34);
        LCD_Write_Cmd(0x2C); // Memory writeF
        break;
    default:
        break;
    }
}

void LCD_Init(void)
{
    LCD_BL(0);
    HAL_Delay(150);
    LCD_RST(0);
    HAL_Delay(150);
    LCD_RST(1);
    HAL_Delay(150);
    LCD_Write_Cmd(0x11); // Sleep out
    HAL_Delay(150);

    LCD_Write_Cmd(0x36); // Memory data access control
    if (USE_HORIZONTAL == 0)
        LCD_Write_Byte(0x00);
    else if (USE_HORIZONTAL == 1)
        LCD_Write_Byte(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_Write_Byte(0x70);
    else
        LCD_Write_Byte(0xA0);

    LCD_Write_Cmd(0x3A);  // Interface pixel format
    LCD_Write_Byte(0x55); // 16-bit RGB565

    LCD_Write_Cmd(0xB2); // Porch setting
    LCD_Write_Byte(0x0C);
    LCD_Write_Byte(0x0C);
    LCD_Write_Byte(0x00);
    LCD_Write_Byte(0x33);
    LCD_Write_Byte(0x33);

    LCD_Write_Cmd(0xB7); // Gate control
    LCD_Write_Byte(0x35);

    LCD_Write_Cmd(0xBB); // VCOM setting
    LCD_Write_Byte(0x19);

    LCD_Write_Cmd(0xC0); // LCM control
    LCD_Write_Byte(0x2C);

    LCD_Write_Cmd(0xC2); // VDV and VRH command enable
    LCD_Write_Byte(0x01);

    LCD_Write_Cmd(0xC3); // VRH set
    LCD_Write_Byte(0x12);

    LCD_Write_Cmd(0xC4); // VDV set
    LCD_Write_Byte(0x20);

    LCD_Write_Cmd(0xC6); // Frame rate control
    LCD_Write_Byte(0x0F);

    LCD_Write_Cmd(0xD0); // Power control 1
    LCD_Write_Byte(0xA4);
    LCD_Write_Byte(0xA1);

    LCD_Write_Cmd(0xE0); // Positive voltage gamma control
    LCD_Write_Byte(0xD0);
    LCD_Write_Byte(0x04);
    LCD_Write_Byte(0x0D);
    LCD_Write_Byte(0x11);
    LCD_Write_Byte(0x13);
    LCD_Write_Byte(0x2B);
    LCD_Write_Byte(0x3F);
    LCD_Write_Byte(0x54);
    LCD_Write_Byte(0x4C);
    LCD_Write_Byte(0x18);
    LCD_Write_Byte(0x0D);
    LCD_Write_Byte(0x0B);
    LCD_Write_Byte(0x1F);
    LCD_Write_Byte(0x23);

    LCD_Write_Cmd(0xE1); // Negative voltage gamma control
    LCD_Write_Byte(0xD0);
    LCD_Write_Byte(0x04);
    LCD_Write_Byte(0x0C);
    LCD_Write_Byte(0x11);
    LCD_Write_Byte(0x13);
    LCD_Write_Byte(0x2C);
    LCD_Write_Byte(0x3F);
    LCD_Write_Byte(0x44);
    LCD_Write_Byte(0x51);
    LCD_Write_Byte(0x2F);
    LCD_Write_Byte(0x1F);
    LCD_Write_Byte(0x1F);
    LCD_Write_Byte(0x20);
    LCD_Write_Byte(0x23);

    LCD_Write_Cmd(0x21); // Display inversion ON (ST7789 默认需要 inversion ON)
    // LCD_Write_Cmd(0x20);

    LCD_Write_Cmd(0x29); // Display ON

    LCD_Address_Set(0, 0, LCD_Width, LCD_Height);

    LCD_BL(1);
}

void LCD_Clear(uint16_t color)
{
    LCD_Fill(0, 0, LCD_Width, LCD_Height, color);
}

#if USE_DMA
//  在ld文件SECTIONS加
//  .axisram (NOLOAD) :
//  {
//    . = ALIGN(32);
//    *(.axisram)
//    . = ALIGN(32);
//  } >RAM
// __attribute__((aligned(32), section(".RAM"))) uint16_t display_buffer[LCD_Width * LCD_Height];
__attribute__((aligned(32), section(".axisram"))) uint16_t display_buffer[LCD_Width * LCD_Height];
#endif

void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint32_t total_pixels = (x_end - x_start) * (y_end - y_start);
    LCD_Address_Set(x_start, y_start, x_end, y_end);
#if USE_DMA
    /* 填充 16bit 数据 */
    for (uint32_t i = 0; i < total_pixels; i++)
    {
        // display_buffer[i] = __REV16(color); // 自动交换高低字节，兼容 LCD 通常要求高字节先发
        display_buffer[i] = color;
    }
    uint32_t transfer_size = total_pixels; // 16bit 模式下，传 total_pixels 个数据项
    uint32_t addr = (uint32_t)display_buffer;
    uint32_t size = transfer_size * 2;
    SCB_CleanDCache_by_Addr((uint32_t *)(addr & ~31U), (size + 31U) & ~31U);
    __DSB();
    // 16bit 发送
    LCD_Write_Data_16bit(display_buffer, transfer_size);
#else
    for (uint16_t i = 0; i < (y_end - y_start); i++)
    {
        for (uint16_t j = 0; j < (x_end - x_start); j++)
        {
            LCD_Write_HalfWord(color);
        }
    }
#endif
}

/**
 * @brief Write a char
 * @param  x&y -> cursor of the start point.
 * @param ch -> char to write
 * @param size  -> size of the string
 * @param color -> color of the char
 * @param bgcolor -> background color of the char
 * @return  none
 */
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint8_t size, uint16_t color, uint16_t bgcolor)
{
    uint8_t temp, t1, t, csize, sta;
    uint16_t colortemp;
    ch = ch - ' ';
    if ((x > (LCD_Width - size / 2)) || (y > (LCD_Height - size)))
        return;
    LCD_Address_Set(x, y, x + size / 2 - 1, y + size - 1);
    if ((size == 16) || (size == 32))
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
        for (t = 0; t < csize; t++)
        {
            if (size == 16)
                temp = asc2_1608[ch][t];
            else if (size == 32)
                temp = asc2_3216[ch][t];
            else
                return;
            for (t1 = 0; t1 < 8; t1++)
            {
                if (temp & 0x80)
                    colortemp = color;
                else
                    colortemp = bgcolor;
                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
    else if (size == 12)
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
        for (t = 0; t < csize; t++)
        {
            temp = asc2_1206[ch][t];
            for (t1 = 0; t1 < 6; t1++)
            {
                if (temp & 0x80)
                    colortemp = color;
                else
                    colortemp = bgcolor;
                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
    else if (size == 24)
    {
        csize = (size * 16) / 8;
        for (t = 0; t < csize; t++)
        {
            temp = asc2_2412[ch][t];
            if (t % 2 == 0)
                sta = 8;
            else
                sta = 4;
            for (t1 = 0; t1 < sta; t1++)
            {
                if (temp & 0x80)
                    colortemp = color;
                else
                    colortemp = bgcolor;
                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
}

static uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
        result *= m;

    return result;
}

void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color, uint16_t bgcolor)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, color, bgcolor);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, color, bgcolor);
    }
}

void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint16_t bgcolor)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' '))
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)
            break;
        LCD_ShowChar(x, y, *p, size, color, bgcolor);
        x += size / 2;
        p++;
    }
}

#ifdef USE_LVGL

void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p)
{
    LCD_Address_Set(x, y, x + width - 1, y + height - 1);
    for (int i = 0; i < width * height; i++)
    {
        display_buffer[i] = ((*(p + i * 2)) << 8) | (*(p + i * 2 + 1));
    }
    uint32_t addr = (uint32_t)display_buffer;
    uint32_t size = width * height * 2;
    SCB_CleanDCache_by_Addr((uint32_t *)(addr & ~31U), (size + 31U) & ~31U);
    __DSB();
    LCD_Write_Data_16bit(display_buffer, width * height);
}

void LCD_Fill_LVGL(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, lv_color_t *color_p)
{
}
#endif

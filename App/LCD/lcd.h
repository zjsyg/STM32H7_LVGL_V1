#ifndef __LCD_H
#define __LCD_H

#include "main.h"

#define USE_DMA 1

#define USE_HORIZONTAL 2// 0: 0 degree, 1: 180 degree, 2: 90 degree, 3: 270 degree  0:	上到下 左到右

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_Width 172
#define LCD_Height 320

#else
#define LCD_Width 320
#define LCD_Height 172
#endif

/**
 *Color of pen
 *If you want to use another color, you can choose one in RGB565 format.
 */

#define WHITE 			0xFFFF
#define BLACK 			0x0000
#define BLUE 			0x001F
#define BRED 			0XF81F
#define GRED 			0XFFE0
#define GBLUE 			0X07FF
#define RED 			0xF800
#define MAGENTA 		0xF81F
#define GREEN 			0x07E0
#define CYAN 			0x7FFF
#define YELLOW 			0xFFE0
#define BROWN 			0XBC40
#define BRRED 			0XFC07
#define GRAY			0X8430

#define DARKBLUE 		0X01CF
#define LIGHTBLUE 		0X7D7C
#define GRAYBLUE 		0X5458
#define LIGHTGREEN 		0X841F
#define LIGHTGRAY		0XEF5B
#define LGRAY 			0XC618

#define LGRAYBLUE 		0XA651
#define LBBLUE 			0X2B12

/*
		LCD_PWR:	PA2
		LCD_RST:	PA6
		LCD_DC:		PA4
		LCD_CS:		PA3
*/
#define LCD_BL(x) (x ? LL_GPIO_SetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin) \
					 : LL_GPIO_ResetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin))
#define LCD_RST(x) (x ? LL_GPIO_SetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin) \
					  : LL_GPIO_ResetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin))
#define LCD_DC(x) (x ? LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin) \
					 : LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin))
// #define LCD_CS(x) (x ? HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
//					: HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET))

extern uint8_t spi_tx_flag;

void LCD_Write_Data_16bit(uint16_t *buff, uint32_t buff_size);

void LCD_Init(void);
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void LCD_Clear(uint16_t color);
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint8_t size, uint16_t color, uint16_t bgcolor);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color, uint16_t bgcolor);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint16_t bgcolor);
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);

#endif
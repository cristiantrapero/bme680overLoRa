
#include "stm32l4xx_hal.h"
#include "fonts.h"

// Esta biblioteca fue escrita por Olivier Van den Eede 2016 y adaptado para su uso con
// Microcontroladores STM32 y utiliza las bibliotecas I2C HAL.
//
// Esta biblioteca está diseñado para utilizar una pantalla OLED ssd1306 impulsada.
// Para utilizar debe sin duda por debajo de las define sean correctas.
// Como puerto I2C utilizado y el tamaño de la pantalla.
//
// La biblioteca utiliza dos archivos (fonts.c / h), que describe tres fuentes.
// Estas fuentes se pueden utilizar como: 	- Font_7x10
// 											- Font_11x18
// 											- Font_16x26

#ifndef ssd1306
#define ssd1306

// I2C port usado en el programa.
#define SSD1306_I2C_PORT		hi2c3
// I2C address 
#define SSD1306_I2C_ADDR        0x78
// SSD1306 width in pixels
#define SSD1306_WIDTH           128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT          64

typedef enum {
	Black = 0x00, /*!< Black color, no pixel */
	White = 0x01  /*!< Pixel is set. Color depends on LCD puede ser Blue-Azul*/
} SSD1306_COLOR;

//	Struct para seguir los cambios
//
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

extern I2C_HandleTypeDef SSD1306_I2C_PORT;

//
uint8_t ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_ON(void);
void ssd1306_OFF(void);
uint8_t I2C_SSD_esta_Conectado(I2C_HandleTypeDef *hi2c, uint16_t DevAddress);
void ssd1306_WriteCommand(uint8_t command);
void ssd1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, SSD1306_COLOR color);
void ssd1306_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, SSD1306_COLOR color);
void ssd1306_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, SSD1306_COLOR color);
void ssd1306_DrawTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, SSD1306_COLOR color);
void ssd1306_DrawFilledTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, SSD1306_COLOR color);
void ssd1306_DrawCircle(int8_t x0, int8_t y0, int8_t r, SSD1306_COLOR c);
void ssd1306_DrawFilledCircle(int8_t x0, int8_t y0, int8_t r, SSD1306_COLOR color);



#endif


#include"ssd1306.h"


// Databuffer voor het scherm 
// Data-buffer para la pantalla o display.
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

// Een scherm-object om lokaal in te werken
// Una pantalla-objeto para trabajar en local 
static SSD1306_t SSD1306;


// Funcion test si I2C del OLED conectado Juanjo 21 Feb 2017
//
uint8_t I2C_SSD_esta_Conectado(I2C_HandleTypeDef *hi2c, uint16_t DevAddress){
	
	
	/* Check if device is ready for communication */
	if (HAL_I2C_IsDeviceReady(hi2c, DevAddress, 2, 5) != HAL_OK) {
		/* Return error */
		return 0;
	}
	
	return 1;
}



//
//	Een byte sturen naar het commando register
//	Kan niet gebruikt worden buiten deze file
// Enviar un byte en el registro de comando
// No se puede utilizar fuera de este archivo
//
void ssd1306_WriteCommand(uint8_t command)
{
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
	HAL_Delay(5);
}


//
//	Het scherm initialiseren voor gebruik
//
uint8_t ssd1306_Init(void)
{	
	// Even wachten zodat het scherm zeker opgestart is
	HAL_Delay(100);
	
	/* Init LCD */
	ssd1306_WriteCommand(0xAE); //display off
	ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode   
	ssd1306_WriteCommand(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
	ssd1306_WriteCommand(0x00); //---set low column address
	ssd1306_WriteCommand(0x10); //---set high column address
	ssd1306_WriteCommand(0x40); //--set start line address
	ssd1306_WriteCommand(0x81); //--set contrast control register
	ssd1306_WriteCommand(0xFF);
	ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127
	ssd1306_WriteCommand(0xA6); //--set normal display
	ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64)
	ssd1306_WriteCommand(0x3F); //
	ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_WriteCommand(0xD3); //-set display offset
	ssd1306_WriteCommand(0x00); //-not offset
	ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_WriteCommand(0xF0); //--set divide ratio
	ssd1306_WriteCommand(0xD9); //--set pre-charge period
	ssd1306_WriteCommand(0x22); //
	ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration
	ssd1306_WriteCommand(0x12);
	ssd1306_WriteCommand(0xDB); //--set vcomh
	ssd1306_WriteCommand(0x20); //0x20,0.77xVcc
	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x14); //
	ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel, display on
	
	/* Clearen scherm */
	ssd1306_Fill(Black);
	
	/* Update screen */
	ssd1306_UpdateScreen();
	
	/* Set default values */
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;
	
	/* Initialized OK */
	SSD1306.Initialized = 1;
	
	/* Return OK */
	return 1;
}

void ssd1306_ON(void){
	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x14); //
	ssd1306_WriteCommand(0xAF);	//--turn on SSD1306 panel
} 

void ssd1306_OFF(void){
	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x10); //
	ssd1306_WriteCommand(0xAE);	//display off
} 


//
// 	Ponemos todo el buffer a un determinado color
// 	Color => el color en el que todo debe verse
//	We zetten de hele buffer op een bepaalde kleur
// 	color 	=> de kleur waarin alles moet
//
void ssd1306_Fill(SSD1306_COLOR color) 
{
	/* Set memory */
	uint32_t i;

	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

//
//	Alle weizigingen in de buffer naar het scherm sturen
//	Enviar cualquier cambio en el b??fer de la pantalla
void ssd1306_UpdateScreen(void) 
{
	uint8_t i;
	
	for (i = 0; i < 8; i++) {
		ssd1306_WriteCommand(0xB0 + i);
		ssd1306_WriteCommand(0x00);
		ssd1306_WriteCommand(0x10);

		// We schrijven alles map per map weg
		HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH,100);
	}
}

//
//	1 pixel op het scherm tekenen  1 pixel op het scherm tekenen
//	X => X coordinaat
//	Y => Y coordinaat
//	color => kleur die pixel moet krijgen  color para obtener pixel
//
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
	{
		// No escribimos fuera de la pantalla
		return;
	}
	
	// Ver si el pixel ha de ser invertido
	if (SSD1306.Inverted) 
	{
		color = (SSD1306_COLOR)!color;
	}
	
	// Ponemos el color adecuado para el p??xel
	if (color == White)
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} 
	else 
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

//
//	We willen 1 char naar het scherm sturen
//	ch 		=> char om weg te schrijven
//	Font 	=> Font waarmee we gaan schrijven
//	color 	=> Black or White
// 		Enviar car??cter a la pantalla
// 		Ch => char a escribir
// 		Fuente => Tipo de letra a escribir
// 		Color => Negro o Blanco   // o azul?
//
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;
	
	// Kijken of er nog plaats is op deze lijn
	// A ver si hay espacio en esta l??nea
	//
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
	{
		// Er is geen plaats meer
		// No hay m??s
		//
		return 0;
	}
	
	// We gaan door het font  Vamos a trav??s de la fuente
	for (i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (j = 0; j < Font.FontWidth; j++)
		{
			if ((b << j) & 0x8000) 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			} 
			else 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
			}
		}
	}
	
	// De huidige positie is nu verplaatst
	// La posici??n actual se mueve
	//
	SSD1306.CurrentX += Font.FontWidth;
	
	// We geven het geschreven char terug voor validatie
	// Damos el caracter efectivo de nuevo para la validaci??n
	return ch;
}

//
//	Functie voor het wegschrijven van een hele string
// 	str => string om op het scherm te schrijven
//	Font => Het font dat gebruikt moet worden
//	color => Black or White
// 		Funci??n para escribir una cadena entera
// 		Str => string para escribir en la pantalla
// 		Fuente => La fuente que se va a utilizar
// 		Color => Negro o Blanco
//
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
	// We schrijven alle char tot een nulbyte  Mientras el caracter no sea nulo.
	while (*str) 
	{
		if (ssd1306_WriteChar(*str, Font, color) != *str)
		{
			// Het karakter is niet juist weggeschreven 
			// El caracter no est?? escrito correctamente
			return *str;
		}
		
		// Volgende char
		str++;
	}
	
	// Alles gelukt, we sturen dus 0 terug
	return *str;
}

//
//	Zet de cursor op een coordinaat
//
void ssd1306_SetCursor(uint8_t x, uint8_t y) 
{
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

/**
 * @brief  Draws line on LCD
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */
void ssd1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, SSD1306_COLOR color) {
	int8_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			ssd1306_DrawPixel(x0, i, color);
		}	
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			ssd1306_DrawPixel(i, y0, color);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		ssd1306_DrawPixel(x0, y0, color); 
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

/**
 * @brief  Draws rectangle on LCD
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */
void ssd1306_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, SSD1306_COLOR color) {
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	ssd1306_DrawLine(x, y, x + w, y, color);         /* Top line */
	ssd1306_DrawLine(x, y + h, x + w, y + h, color); /* Bottom line */
	ssd1306_DrawLine(x, y, x, y + h, color);         /* Left line */
	ssd1306_DrawLine(x + w, y, x + w, y + h, color); /* Right line */
}

/**
 * @brief  Draws filled rectangle on LCD
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */
void ssd1306_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, SSD1306_COLOR color) {
	uint8_t i;
	
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		ssd1306_DrawLine(x, y + i, x + w, y + i, color);
	}
}

/**
 * @brief  Draws triangle on LCD
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */
void ssd1306_DrawTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, SSD1306_COLOR color) {
	/* Draw lines */
	ssd1306_DrawLine(x1, y1, x2, y2, color);
	ssd1306_DrawLine(x2, y2, x3, y3, color);
	ssd1306_DrawLine(x3, y3, x1, y1, color);
}

/**
 * @brief  Draws filled triangle on LCD
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */
void ssd1306_DrawFilledTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, SSD1306_COLOR color) {
	int8_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ssd1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

/**
 * @brief  Draws circle to STM buffer
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */

void ssd1306_DrawCircle(int8_t x0, int8_t y0, int8_t r, SSD1306_COLOR color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    ssd1306_DrawPixel(x0, y0 + r, color);
    ssd1306_DrawPixel(x0, y0 - r, color);
    ssd1306_DrawPixel(x0 + r, y0, color);
    ssd1306_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_DrawPixel(x0 + x, y0 + y, color);
        ssd1306_DrawPixel(x0 - x, y0 + y, color);
        ssd1306_DrawPixel(x0 + x, y0 - y, color);
        ssd1306_DrawPixel(x0 - x, y0 - y, color);

        ssd1306_DrawPixel(x0 + y, y0 + x, color);
        ssd1306_DrawPixel(x0 - y, y0 + x, color);
        ssd1306_DrawPixel(x0 + y, y0 - x, color);
        ssd1306_DrawPixel(x0 - y, y0 - x, color);
    }
}

/**
 * @brief  Draws filled circle to STM buffer
 * @note   @ref ssd1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR enumeration
 * @retval None
 */
void ssd1306_DrawFilledCircle(int8_t x0, int8_t y0, int8_t r, SSD1306_COLOR color) {
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

    ssd1306_DrawPixel(x0, y0 + r, color);
    ssd1306_DrawPixel(x0, y0 - r, color);
    ssd1306_DrawPixel(x0 + r, y0, color);
    ssd1306_DrawPixel(x0 - r, y0, color);
    ssd1306_DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        ssd1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        ssd1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        ssd1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

/**
 *  File ends  JJSM
 **/


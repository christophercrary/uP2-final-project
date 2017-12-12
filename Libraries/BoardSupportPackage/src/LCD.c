/*
 * LCDLib.c
 *
 *  Created on: Mar 2, 2017
 *      Author: Danny
 */

#include "LCD.h"
#include "msp.h"
#include "driverlib.h"
#include "AsciiLib.h"

/************************************  Private Functions  *******************************************/

// test (REMOVE)
uint16_t adc_val_x;
uint16_t adc_val_y;

/*
 * Delay x ms
 */
static void Delay(unsigned long interval)
{
    while(interval > 0)
    {
        __delay_cycles(48000);
        interval--;
    }
}

/*******************************************************************************
 * Function Name  : LCD_initSPI
 * Description    : Configures LCD Control lines
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
static void LCD_initSPI()
{
    /* P10.1 - CLK
     * P10.2 - MOSI
     * P10.3 - MISO
     * P10.4 - LCD CS 
     * P10.5 - TP CS 
     */

    // create SPI configuration for master
    static const eUSCI_SPI_MasterConfig spiMasterConfig =
    {
            EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source

            // SMCLK is HFXT/4 (48MHz / 4)
            12000000,                                   // SMCLK = 12MHZ

            // see ILI3925C Timing Characteristics for SCLK limitations
            12000000,                                    // SPICLK = 12MHz

            // data is sent MSB first
            EUSCI_B_SPI_MSB_FIRST,                     // MSB first

            // data is setup on first clock edge, captured on next clock edge
            EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // phase

            // when data is not being transmitted, clock polarity is high
            EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, // high polarity

            // slave selects for LCD and TP used by GPIO pins P10.4/P10.5
            EUSCI_B_SPI_3PIN                           // 3-wire SPI Mode
    };

    // assign GPIO pins P10.4/P10.5 as outputs (used for LCD/TP SS pins)
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN4 | GPIO_PIN5);

    // default slave selects to false
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN4 | GPIO_PIN5);

    // configure P10 SPI pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,
    GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // Configure SPI for 3-wire master mode
    SPI_initMaster(EUSCI_B3_BASE, &spiMasterConfig);

    // Enable SPI module
    SPI_enableModule(EUSCI_B3_BASE);

    return;
}

/*******************************************************************************
 * Function Name  : LCD_reset
 * Description    : Resets LCD
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : Uses P10.0 for reset
 *******************************************************************************/
static void LCD_reset()
{
    P10DIR |= BIT0;
    P10OUT |= BIT0;  // high
    Delay(100);
    P10OUT &= ~BIT0; // low
    Delay(100);
    P10OUT |= BIT0;  // high
}

/*******************************************************************************
 * Function Name  : TP_ReadADC
 * Description    : Reads ADC conversion from Touch Panel on LCD.
 * Input          : uint8_t channel
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
static uint16_t TP_ReadADC(uint8_t channel)
{
    uint16_t adc_val;

    // start transmission
    SPI_CS_TP_LOW;

    // send start byte for Channel Y ADC reading
    SPISendByte(channel);

    // receive upper 7 bytes of 12-bit ADC reading (shift accordingly)
    // see pg. 23 of XPT2046 datasheet for more information
    adc_val = (SPISendRecvByte(0) << 5);

    // receive lower 5 bytes of 12-bit ADC reading (shift accordingly)
    // see pg. 23 of XPT2046 datasheet for more information
    adc_val |= (SPISendRecvByte(0) >> 3);

    // end transmission
    SPI_CS_TP_HIGH;

    // return ADC conversion value for selected channel
    return adc_val;
}


/************************************  Private Functions  *******************************************/


/************************************  Public Functions  *******************************************/

/*******************************************************************************
 * Function Name  : 8bit_to_16bit
 * Description    : convert an 8-bit color value into 16-bit color value
 * Input          : uint8_t color
 * Output         : None
 * Return         : None
 * Attention      :
 *******************************************************************************/
uint16_t LCD_8bit_to_16bit(uint8_t color)
{
    uint8_t red8 = (color >> 5) & 0b00000111;
    uint8_t green8 = (color >> 2) & 0b00000111;
    uint8_t blue8 = (color >> 0) & 0b00000011;

    uint16_t red16 = (red8 * 0b00011111)/(0b00000111);
    uint16_t green16 = (green8 * 0b00111111)/(0b00000111);
    uint16_t blue16 = (blue8 * 0b00011111)/(0b00000011);

    return ((red16 << 11) | (green16 << 5) | blue16);
}

/*******************************************************************************
 * Function Name  : LCD_DrawImage
 * Description    : Draw an image
 * Input          : uint8_t image
 * Output         : None
 * Return         : None
 * Attention      :
 *******************************************************************************/

/*******************************************************************************
 * Function Name  : LCD_DrawRectangle
 * Description    : Draw a rectangle as the specified color.
 *                  NOTE: This function truncates rectangles that are
 *                  out of bounds. Any wrapping of rectangles is handled in
 *                  specified functions in threads.h
 * Input          : xStart, xEnd, yStart, yEnd, Color
 * Output         : None
 * Return         : None
 * Attention      : Must draw from left to right, top to bottom!
 *******************************************************************************/
void LCD_DrawRectangle(int16_t xStart, int16_t xEnd, int16_t yStart, int16_t yEnd, uint16_t Color)
{

    /* Check special cases for out of bounds */
    if (xStart < MIN_SCREEN_X)
    {
        xStart = MIN_SCREEN_X;
    }
    if (xEnd > MAX_SCREEN_X)
    {
        xEnd = MAX_SCREEN_X;
    }
    if (yStart < MIN_SCREEN_Y)
    {
        yStart = MIN_SCREEN_Y;
    }
    if (yEnd > MAX_SCREEN_Y)
    {
        yEnd = MAX_SCREEN_Y;
    }

    // check if rectangle size is zero or negative
    // check here because xStart and yStart could have been updated above
    if ( (xStart >= xEnd) || (yStart >= yEnd) )
    {
        return;     // cannot draw rectangle
    }

    /* Set window area for high-speed RAM write */
    LCD_WriteReg(HOR_ADDR_START_POS, yStart);
    LCD_WriteReg(HOR_ADDR_END_POS, yEnd);
    LCD_WriteReg(VERT_ADDR_START_POS, xStart);
    LCD_WriteReg(VERT_ADDR_END_POS, xEnd);


    /* Set cursor */
    LCD_SetCursor(xStart, yStart);

    /* Set index to GRAM */ 
    LCD_WriteIndex(GRAM);

    /* Send out data only to the entire area */

    // select the LCD device
    SPI_CS_LCD_LOW;

    // write start byte for sequential GRAM data only ONCE!
    LCD_Write_Data_Start();

    // fill entire screen with color
    for (int16_t i = xStart; i <= xEnd; i++)
    {
        for (int16_t j = yStart; j <= yEnd; j++)
        {
            LCD_Write_Data_Only(Color); // write specified color to every pixel
        }
    }

    // de-select the LCD
    SPI_CS_LCD_HIGH;
}

/*******************************************************************************
 * Function Name  : LCD_DrawRectangleStructure
 * Description    : Draw a rectangle structure (eventually to be used as a
 *                  wrapper function for LCD_DrawRectangle)
 * Input          : Rectangle rectangle
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_DrawRectangleStructure(Rectangle rectangle)
{
    // call static helper function for drawing rectangle (not actually static yet)
    LCD_DrawRectangle(rectangle.xMin, rectangle.xMax, rectangle.yMin,
                      rectangle.yMax, rectangle.color);
}

/*******************************************************************************
 * Function Name  : LCD_DrawSection
 * Description    : Draw a section, which is defined to be an array of rectangles.
 * Input          : Rectangle* rectangles
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_DrawSection(const Rectangle* rectangles, uint16_t array_size)
{
    // draw the specified screen of rectangles
    for (uint8_t i = 0; i < array_size; i++)
    {
        LCD_DrawRectangleStructure(rectangles[i]);
    }
}

/******************************************************************************
 * Function Name  : LCD_PutChar
 * Description    : Lcd screen displays a character
 * Input          : - Xpos: Horizontal coordinate
 *                  - Ypos: Vertical coordinate
 *                  - ASCI: Displayed character
 *                  - charColor: Character color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
inline void LCD_PutChar(uint16_t Xpos, uint16_t Ypos, char ASCI, uint16_t charColor)
{
    uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* get font data */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* Character color */
            }
        }
    }
}

/******************************************************************************
 * Function Name  : LCD_Text
 * Description    : Displays the string
 * Input          : - Xpos: Horizontal coordinate
 *                  - Ypos: Vertical coordinate
 *                  - str: Displayed string
 *                  - charColor: Character color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Text(uint16_t Xpos, uint16_t Ypos, char *str, uint16_t Color)
{
    uint8_t TempChar;
    uint16_t Xpos_start = Xpos;
    uint16_t Ypos_start = Ypos;

    /* Set area back to span the entire LCD */
    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */
    do
    {
        // read in the next character from the given input string
        TempChar = *str++;

        // check if a user-defined escape sequence should occur
        if (TempChar == '~')
        {
            // if new-line should occur on LCD screen
            if (*str == 'n')
            {
                str++;      // do not output escape character to LCD screen

                // wrap to next line of LCD screen
                Xpos = Xpos_start;
                Ypos_start += 16;
                Ypos = Ypos_start;
            }
            // if tab should occur on LCD screen
            else if (*str == 't')
            {
                str++;      // do not output escape character to LCD screen

                // tab the output (Xpos += 10)
                Xpos += 10;
            }

            /* special characters unavaible in 8-bit ASCII */

            // up-arrow
            else if ((*str == '^') && (*(str+1) == '|'))
            {
                str+=2;     // increment past the '^' and '|' characters
                LCD_PutChar(Xpos, Ypos, '^', Color);
                Ypos_start += 1;
                Ypos = Ypos_start;
                LCD_PutChar(Xpos, Ypos, '|', Color);
            }
        }
        // otherwise, output character to LCD screen
        else
        {
            LCD_PutChar(Xpos, Ypos, TempChar, Color);

            // check if output needs to wrap about LCD screen
            if( Xpos < MAX_SCREEN_X - 8)
            {
                Xpos += 8;
            }
            else if ( Ypos < MAX_SCREEN_X - 16)
            {
                Xpos = 0;
                Ypos += 16;
            }
            else
            {
                Xpos = 0;
                Ypos = 0;
            }
        }
    }while ( *str != 0 );
}

/******************************************************************************
 * Function Name  : LCD_PrintTextStructure
 * Description    : Prints string within specified text structure array
 * Input          : Text text
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_PrintTextStructure(Text text)
{
    // IMPLEMENT ERROR CHECKING FOR TEXT COORDINATES

    // write text within given text structure
    LCD_Text(text.xStart, text.yStart, text.string, text.color);

    return;
}

/******************************************************************************
 * Function Name  : LCD_PrintTextSection
 * Description    : Prints all strings within specified Text structure array
 * Input          : Text *texts, uint16_t array_size
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_PrintTextSection(const Text *texts, uint16_t array_size)
{

    // write all texts within specified Text structure
    for (uint16_t i = 0; i < array_size; i++)
    {
        LCD_PrintTextStructure(texts[i]);
    }

    return;
}

/*******************************************************************************
 * Function Name  : LCD_Clear
 * Description    : Fill the screen as the specified color
 * Input          : - Color: Screen Color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    /* Set area back to span the entire LCD */
    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */
    
    /* Set cursor to (0,0) */ 
    LCD_SetCursor(0, 0);

    /* Set write index to GRAM */
    LCD_WriteIndex(GRAM);


    /* Start data transmission */

    // select the LCD device
    SPI_CS_LCD_LOW;

    // write start byte for sequential GRAM data only ONCE!
    LCD_Write_Data_Start();

    // fill entire screen with color
    for (uint32_t i = 0; i < SCREEN_SIZE; i++)
    {
        LCD_Write_Data_Only(Color); // write specified color to every pixel
    }

    // de-select the LCD
    SPI_CS_LCD_HIGH;
}

/******************************************************************************
 * Function Name  : LCD_SetPoint
 * Description    : Drawn at a specified point coordinates
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate
 * Output         : None
 * Return         : None
 * Attention      : 18N Bytes Written
 *******************************************************************************/
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t color)
{
    /* Should check for out of bounds */ 
    if ( (Xpos > MAX_SCREEN_X) || (Ypos > MAX_SCREEN_Y) )
    {
        return;     // no error returned
    }

    // otherwise...

    /* Set cursor to Xpos and Ypos */
    LCD_SetCursor(Xpos, Ypos);

    /* Write color to GRAM reg */ 
    LCD_WriteReg(GRAM, color);      // single GRAM address color update
}

/********************************************************************************
 * Function Name  : LCD_Write_Reg_Start
 * Description    : Start of register writing to the LCD controller
 * Input          : None
 * Output         : None
 * Return         : None
 ********************************************************************************/
inline void LCD_Write_Reg_Start (void)
{
    SPISendByte(SPI_START | SPI_WR | SPI_INDEX);
}

/********************************************************************************
 * Function Name  : LCD_Write_Data_Only
 * Description    : Data writing to the LCD controller
 * Input          : - data: data to be written
 * Output         : None
 * Return         : None
 ********************************************************************************/
inline void LCD_Write_Data_Only (uint16_t data)
{
    /* Send out MSB */
    SPISendByte(data >> 8);
    
    /* Send out LSB */
    SPISendByte(data & 0xFF);
}

/********************************************************************************
 * Function Name  : LCD_WriteData
 * Description    : LCD write register data
 * Input          : - data: register data
 * Output         : None
 * Return         : None
 ********************************************************************************/
inline void LCD_WriteData(uint16_t data)
{
    SPI_CS_LCD_LOW;
    {
        LCD_Write_Data_Start();         /* Write : RS = 1, RW = 0       */
        LCD_Write_Data_Only(data);      /* Write D0..D15                 */
    }
    SPI_CS_LCD_HIGH;
}


/********************************************************************************
 * Function Name  : LCD_ReadData
 * Description    : LCD read data
 * Input          : - data: data to be written
 * Output         : None
 * Return         : Returns data
 ********************************************************************************/
inline uint16_t LCD_ReadData()
{
    uint16_t value;
    SPI_CS_LCD_LOW;
    {
        LCD_Write_Data_Start();             /* Read: RS = 1, RW = 1   */
        SPISendByte(0);                 /* Dummy read 1           */
        SPISendByte(0);                 /* Dummy read 2           */
        SPISendByte(0);                 /* Dummy read 3           */
        SPISendByte(0);                 /* Dummy read 4           */
        SPISendByte(0);                 /* Dummy read 5           */
        
        value = (SPIRecvByte() << 8);  /* Read D8..D15           */
        value |= SPIRecvByte();        /* Read D0..D7            */
    }
    SPI_CS_LCD_HIGH;
    return value;
}

/*******************************************************************************
 * Function Name  : LCD_ReadReg
 * Description    : Reads the selected LCD Register.
 * Input          : - LCD_Reg: address of the selected register.
 * Output         : None
 * Return         : LCD Register Value.
 * Attention      : None
 *******************************************************************************/
inline uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    /* Write 16-bit Index */
    LCD_WriteIndex(LCD_Reg);

    /* Return 16-bit Reg using LCD_ReadData() */
    return LCD_ReadData();

}

/********************************************************************************
 * Function Name  : LCD_WriteIndex
 * Description    : LCD write register address
 * Input          : - index: register address
 * Output         : None
 * Return         : None
 ********************************************************************************/
inline void LCD_WriteIndex(uint16_t index)
{
    /* SPI write data */
    SPI_CS_LCD_LOW;
    {
        LCD_Write_Reg_Start();      /* Write : RS = 0, RW = 0  */
        LCD_Write_Data_Only(index); /* Write : Index */
    }
    SPI_CS_LCD_HIGH;
}

/********************************************************************************
 * Function Name  : SPIRecvByte
 * Description    : Receive one byte of data
 * Input          : None
 * Output         : None
 * Return         : Received value
 ********************************************************************************/
inline uint8_t SPIRecvByte (void)
{
    /* Send dummy byte of data */
    UCB3TXBUF = 0x00;
    
    /* Return received value*/
    while(!(UCB3IFG & UCRXIFG));
    return UCB3RXBUF;
}

/********************************************************************************
 * Function Name  : SPISendByte
 * Description    : Send one byte
 * Input          : uint8_t: byte
 * Output         : None
 * Return         : Received value
 ********************************************************************************/
inline void SPISendByte (uint8_t byte)
{
    /* Send byte of data */
    UCB3TXBUF = byte;
    while(!(UCB3IFG & UCTXIFG));
}

/*******************************************************************************
 * Function Name  : SPISendRecvByte
 * Description    : Send one byte then recv one byte of response
 * Input          : uint8_t: byte
 * Output         : None
 * Return         : Recieved value
 * Attention      : None
 *******************************************************************************/
inline uint8_t SPISendRecvByte(uint8_t byte)
{
    // send byte of data
    MAP_SPI_transmitData(EUSCI_B3_BASE, byte);

    // wait as long as SPI bus is busy (receiving byte of data)
    //while (EUSCI_B_SPI_isBusy(EUSCI_B3_BASE));

    while(UCB3STATW & UCBUSY);

    // return received value (SOMI pushed data during transmission)
    return MAP_SPI_receiveData(EUSCI_B3_BASE);
}

/********************************************************************************
 * Function Name  : LCD_Write_Data_Start
 * Description    : Start of data writing to the LCD controller
 * Input          : None
 * Output         : None
 * Return         : None
 *********************************************************************************/
inline void LCD_Write_Data_Start(void)
{
    /* Write : RS = 1, RW = 0 */
    SPISendByte(SPI_START | SPI_WR | SPI_DATA);
}


/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Writes to the selected LCD register.
 * Input          : - LCD_Reg: address of the selected register.
 *                  - LCD_RegValue: value to write to the selected register.
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
inline void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    /* Write 16-bit Index */
    LCD_WriteIndex(LCD_Reg);

    /* Write 16-bit Reg Data */
    LCD_WriteData(LCD_RegValue);

    return;
}

/*******************************************************************************
 * Function Name  : LCD_SetCursor
 * Description    : Sets the cursor position.
 * Input          : - Xpos: specifies the X position.
 *                  - Ypos: specifies the Y position.
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
inline void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos )
{
    /* Set horizonal GRAM coordinate (Ypos) */ 
    LCD_WriteReg(HORIZONTAL_GRAM_SET, Ypos);

    /* Set vertical GRAM coordinate (Xpos) */
    LCD_WriteReg(VERTICAL_GRAM_SET, Xpos);

}

/*******************************************************************************
 * Function Name  : LCD_Init
 * Description    : Configures LCD Control lines, sets whole screen black
 * Input          : bool usingTP: determines whether or not to enable TP interrupt 
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_Init(bool usingTP)
{
    LCD_initSPI();

    if (usingTP)
    {
        /* Configure falling-edge interrupt on P4.0 for TP */
        MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0);
        GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN0, GPIO_HIGH_TO_LOW_TRANSITION);

        // enable interrupts for daughter board buttons
        GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

        // clear interrupt flag for extra security
        GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN0);

        // enable the PORT4 IRQ
        //NVIC_EnableIRQ(PORT4_IRQn);
    }

    LCD_reset();

    LCD_WriteReg(0xE5, 0x78F0); /* set SRAM internal timing */
    LCD_WriteReg(DRIVER_OUTPUT_CONTROL, 0x0100); /* set Driver Output Control */
    LCD_WriteReg(DRIVING_WAVE_CONTROL, 0x0700); /* set 1 line inversion */
    LCD_WriteReg(ENTRY_MODE, 0x1038); /* set GRAM write direction and BGR=1 */
    LCD_WriteReg(RESIZING_CONTROL, 0x0000); /* Resize register */
    LCD_WriteReg(DISPLAY_CONTROL_2, 0x0207); /* set the back porch and front porch */
    LCD_WriteReg(DISPLAY_CONTROL_3, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
    LCD_WriteReg(DISPLAY_CONTROL_4, 0x0000); /* FMARK function */
    LCD_WriteReg(RGB_DISPLAY_INTERFACE_CONTROL_1, 0x0000); /* RGB interface setting */
    LCD_WriteReg(FRAME_MARKER_POSITION, 0x0000); /* Frame marker Position */
    LCD_WriteReg(RGB_DISPLAY_INTERFACE_CONTROL_2, 0x0000); /* RGB interface polarity */

    /* Power On sequence */
    LCD_WriteReg(POWER_CONTROL_1, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(POWER_CONTROL_2, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(POWER_CONTROL_3, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(POWER_CONTROL_4, 0x0000); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(DISPLAY_CONTROL_1, 0x0001);
    Delay(200);

    /* Dis-charge capacitor power voltage */
    LCD_WriteReg(POWER_CONTROL_1, 0x1090); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(POWER_CONTROL_2, 0x0227); /* Set DC1[2:0], DC0[2:0], VC[2:0] */
    Delay(50); /* Delay 50ms */
    LCD_WriteReg(POWER_CONTROL_3, 0x001F);
    Delay(50); /* Delay 50ms */
    LCD_WriteReg(POWER_CONTROL_4, 0x1500); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(POWER_CONTROL_7, 0x0027); /* 04 VCM[5:0] for VCOMH */
    LCD_WriteReg(FRAME_RATE_AND_COLOR_CONTROL, 0x000D); /* Set Frame Rate */
    Delay(50); /* Delay 50ms */
    LCD_WriteReg(GRAM_HORIZONTAL_ADDRESS_SET, 0x0000); /* GRAM horizontal Address */
    LCD_WriteReg(GRAM_VERTICAL_ADDRESS_SET, 0x0000); /* GRAM Vertical Address */

    /* Adjust the Gamma Curve */
    LCD_WriteReg(GAMMA_CONTROL_1,    0x0000);
    LCD_WriteReg(GAMMA_CONTROL_2,    0x0707);
    LCD_WriteReg(GAMMA_CONTROL_3,    0x0307);
    LCD_WriteReg(GAMMA_CONTROL_4,    0x0200);
    LCD_WriteReg(GAMMA_CONTROL_5,    0x0008);
    LCD_WriteReg(GAMMA_CONTROL_6,    0x0004);
    LCD_WriteReg(GAMMA_CONTROL_7,    0x0000);
    LCD_WriteReg(GAMMA_CONTROL_8,    0x0707);
    LCD_WriteReg(GAMMA_CONTROL_9,    0x0002);
    LCD_WriteReg(GAMMA_CONTROL_10,   0x1D04);

    /* Set GRAM area */
    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */
    LCD_WriteReg(GATE_SCAN_CONTROL_0X60, 0x2700); /* Gate Scan Line */
    LCD_WriteReg(GATE_SCAN_CONTROL_0X61, 0x0001); /* NDL,VLE, REV */
    LCD_WriteReg(GATE_SCAN_CONTROL_0X6A, 0x0000); /* set scrolling line */

    /* Partial Display Control */
    LCD_WriteReg(PART_IMAGE_1_DISPLAY_POS, 0x0000);
    LCD_WriteReg(PART_IMG_1_START_END_ADDR_0x81, 0x0000);
    LCD_WriteReg(PART_IMG_1_START_END_ADDR_0x82, 0x0000);
    LCD_WriteReg(PART_IMAGE_2_DISPLAY_POS, 0x0000);
    LCD_WriteReg(PART_IMG_2_START_END_ADDR_0x84, 0x0000);
    LCD_WriteReg(PART_IMG_2_START_END_ADDR_0x85, 0x0000);

    /* Panel Control */
    LCD_WriteReg(PANEL_ITERFACE_CONTROL_1, 0x0010);
    LCD_WriteReg(PANEL_ITERFACE_CONTROL_2, 0x0600);
    LCD_WriteReg(DISPLAY_CONTROL_1, 0x0133); /* 262K color and display ON */
    Delay(50); /* delay 50 ms */

    LCD_Clear(LCD_BLACK);
}

/*******************************************************************************
 * Function Name  : TP_ReadXY
 * Description    : Obtain X and Y touch coordinates
 * Input          : None
 * Output         : None
 * Return         : Pointer to "Point" structure
 * Attention      : None
 *******************************************************************************/
Point TP_ReadXY()
{
    // declare a coordinate plane for the touchscreen
    Point coordinates;

    /* Read X coord. */
    coordinates.x = TP_ReadX();

    /* Read Y coord. */
    coordinates.y = TP_ReadY();

    /* Return point  */
    return coordinates;
}

/*******************************************************************************
 * Function Name  : TP_ReadX
 * Description    : Obtain X touch coordinate
 * Input          : None
 * Output         : None
 * Return         : X Coordinate
 * Attention      : None
 *******************************************************************************/
uint16_t TP_ReadX()
{
    // ADC_X_MIN = 384
    // ADC_X_MAX = 4032

    // read channel-x adc_value and return x-coordinate
    // IMPLEMENT: FLOATING POINT UNIT!!!
    // IMPLEMENT: CALIBRATIONS FOR INDIVIDUAL LCD TOUCHPANELS

    // test (REMOVE)
    adc_val_x = TP_ReadADC(CHX);
    return (0.08772 * (adc_val_x - 384));
}

/*******************************************************************************
 * Function Name  : TP_ReadY
 * Description    : Obtain Y touch coordinate
 * Input          : None
 * Output         : None
 * Return         : Y Coordinate
 * Attention      : None
 *******************************************************************************/
uint16_t TP_ReadY()
{
    // ADC_Y_MIN = 384
    // ADC_Y_MAX = 4000
    // read channel-y adc_value and return y-coordinate
    // IMPLEMENT: FLOATING POINT UNIT!!!
    // IMPLEMENT: CALIBRATIONS FOR INDIVIDUAL LCD TOUCHPANELS
    // test (REMOVE)
    adc_val_y = TP_ReadADC(CHY);
    return (0.064 * (adc_val_y - 384));
}

/*******************************************************************************
 * Function Name  : TP_CheckForSectionPress
 * Description    : Check whether or not a touch made to the TouchPanel was made
 *                  to any rectangle within a specified section (array of
 *                  rectangles). Function returns either index of rectangle
 *                  in given section that was touched if one was found
 *                  or function returns -1 if no such rectangle was found.
 * Input          : Point point, Rectangle* rectangles, uint16_t array_size
 * Output         : None
 * Return         : int index
 * Attention      : None
 *******************************************************************************/
int TP_CheckForSectionPress(Point point, const Rectangle* rectangles, uint16_t array_size)
{
    // traverse given section to see if specified touch made to TouchPanel
    // interacted with any rectangle within section
    for (uint16_t i = 0; i < array_size; i++)
    {
        // if touch was made to specific rectangle within section
        if ( (point.x >= rectangles[i].xMin) &&
             (point.x <= rectangles[i].xMax) &&
             (point.y >= rectangles[i].yMin) &&
             (point.y <= rectangles[i].yMax) )
        {
            return i;       // return index of rectangle within secion
        }
    }

    // if reached, no rectangle within section was touched
    return -1;
}

/************************************  Public Functions  *******************************************/


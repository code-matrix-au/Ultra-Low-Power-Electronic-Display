/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-02-19
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"

void GPIO_Config(void)
{

    gpio_set_drive_capability(EPD_BUSY_PIN, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(EPD_RST_PIN, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(EPD_DC_PIN, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(EPD_SCK_PIN, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(EPD_MOSI_PIN, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(EPD_CS_PIN, GPIO_DRIVE_CAP_3);


    gpio_set_direction(EPD_BUSY_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(EPD_RST_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_DC_PIN, GPIO_MODE_OUTPUT);

    gpio_set_direction(EPD_SCK_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_MOSI_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_CS_PIN, GPIO_MODE_OUTPUT);

    gpio_set_level(EPD_CS_PIN, 1);
    gpio_set_level(EPD_SCK_PIN, 0);
}
/******************************************************************************
function:	Module Initialize, the BCM2835 library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
    // gpio
    GPIO_Config();

    // serial printf

    // Serial.begin(115200);

    // spi
    // SPI.setDataMode(SPI_MODE0);
    // SPI.setBitOrder(MSBFIRST);
    // SPI.setClockDivider(SPI_CLOCK_DIV4);
    // SPI.begin();

    return 0;
}

/******************************************************************************
function:
            SPI read and write
******************************************************************************/
void DEV_SPI_WriteByte(UBYTE data)
{
    // SPI.beginTransaction(spi_settings);
    gpio_set_level(EPD_CS_PIN, GPIO_PIN_RESET);

    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0)
            gpio_set_level(EPD_MOSI_PIN, GPIO_PIN_RESET);
        else
            gpio_set_level(EPD_MOSI_PIN, GPIO_PIN_SET);

        data <<= 1;
        gpio_set_level(EPD_SCK_PIN, GPIO_PIN_SET);
        gpio_set_level(EPD_SCK_PIN, GPIO_PIN_RESET);
    }

    // SPI.transfer(data);
    gpio_set_level(EPD_CS_PIN, GPIO_PIN_SET);
    // SPI.endTransaction();
}

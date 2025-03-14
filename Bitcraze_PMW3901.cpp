/* PMW3901 Arduino driver
 * Copyright (c) 2017 Bitcraze AB
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Bitcraze_PMW3901.h"

//#include <SPI.h>
#include <bcm2835.h>

#define CHIP_ID         0x49  // 01001001
#define CHIP_ID_INVERSE 0xB6  // 10110110

Bitcraze_PMW3901 obj(0);

void testPy(){
  printf("testPy: Hello from lib9\n");
  obj.begin();
}

void readFlow(){
  printf("readFlow()\n");
  obj.readMotionCountGlob();
}

bool Bitcraze_PMW3901_begin()
{
  printf("i am in fuction Bitcraze_PMW3901_begin()\n");
  bool tp = obj.begin();
  return tp;
}

void Bitcraze_PMW3901_readMotionCount()
{
  obj.readMotionCountGlob();
}

int16_t Bitcraze_PMW3901_getX()
{
  return obj.getX();
}

int16_t Bitcraze_PMW3901_getY()
{
  return obj.getY();
}

Bitcraze_PMW3901::Bitcraze_PMW3901(uint8_t cspin)
  : _cs(cspin)
{ }

boolean Bitcraze_PMW3901::begin(void) {
  printf("i am in Bitcraze_PMW3901::begin(void)\n");
  // Setup SPI port
  //SPI.begin();
  //pinMode(_cs, OUTPUT);
  //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));

  // Make sure the SPI bus is reset
  //digitalWrite(_cs, HIGH);
  //delay(1);
  //digitalWrite(_cs, LOW);
  //delay(1);
  //digitalWrite(_cs, HIGH);
  //delay(1);

  //SPI.endTransaction();
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return false;
    }

    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failed. Are you running as root??\n");
      return false;
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);   // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default


  // Power on reset
  registerWrite(0x3A, 0x5A);

  delay(5);
  // Test the SPI communication, checking chipId and inverse chipId
  uint8_t chipId = registerRead(0x00);
  uint8_t dIpihc = registerRead(0x5F);

  if (chipId != CHIP_ID || dIpihc != CHIP_ID_INVERSE) {
    return false;
  }

  // Reading the motion registers one time
  registerRead(0x02);
  registerRead(0x03);
  registerRead(0x04);
  registerRead(0x05);
  registerRead(0x06);
  delay(1);

  initRegisters();

  return true;
}

// Functional access
void Bitcraze_PMW3901::readMotionCountGlob(void)
{
  int16_t tmpX = 0;
  int16_t tmpY = 0;
  registerRead(0x02);
  tmpX = ((int16_t)registerRead(0x04) << 8) | registerRead(0x03);
  tmpY = ((int16_t)registerRead(0x06) << 8) | registerRead(0x05);
   deltaX_g = tmpX;
   deltaY_g = tmpY;
}

void Bitcraze_PMW3901::readMotionCount(int16_t *deltaX, int16_t *deltaY)
{
  int16_t tmpX = 0;
  int16_t tmpY = 0;
  registerRead(0x02);
  tmpX = ((int16_t)registerRead(0x04) << 8) | registerRead(0x03);
  tmpY = ((int16_t)registerRead(0x06) << 8) | registerRead(0x05);
  if(deltaX)
      {*deltaX = tmpX;}
  if(deltaY)
      {*deltaY = tmpY;}
   deltaX_g = tmpX;
   deltaY_g = tmpY;
}

int16_t Bitcraze_PMW3901::getX()
{
  return deltaX_g;
}
int16_t Bitcraze_PMW3901::getY()
{
  return deltaY_g;
}

void Bitcraze_PMW3901::enableFrameBuffer()
{

  registerWrite(0x7F, 0x07);  //Magic frame readout registers
  registerWrite(0x41, 0x1D);
  registerWrite(0x4C, 0x00);
  registerWrite(0x7F, 0x08);
  registerWrite(0x6A, 0x38);
  registerWrite(0x7F, 0x00);
  registerWrite(0x55, 0x04);
  registerWrite(0x40, 0x80);
  registerWrite(0x4D, 0x11);

  registerWrite(0x70, 0x00);   //More magic? 
  registerWrite(0x58, 0xFF);

  int temp, check;

  do { // keep reading
    temp = registerRead(0x58); // the status register
    check = temp>>6; // rightshift 6 bits so only top two stay 
  } while(check == 0x03); // while bits aren't set denoting ready state
  delayMicroseconds(50);
}

void Bitcraze_PMW3901::readFrameBuffer(char *FBuffer)
{
  int count = 0;
  uint8_t a; //temp value for reading register
  uint8_t b; //temp value for second register
  uint8_t hold; //holding value for checking bits
  uint8_t mask = 0x0c; //mask to take bits 2 and 3 from b
  uint8_t pixel = 0; //temp holding value for pixel

  for (int ii = 0; ii < 1225; ii++) { //for 1 frame of 1225 pixels (35*35)
    do { 
      //if data is either invalid status
      //check status bits 6 and 7
      //if 01 move upper 6 bits into temp value
      //if 00 or 11, reread
      //else lower 2 bits into temp value
      a = registerRead(0x58); //read register
      hold = a >> 6; //right shift to leave top two bits for ease of check.
    } while((hold == 0x03) || (hold == 0x00));
    
    if (hold == 0x01) { //if data is upper 6 bits
      b = registerRead(0x58); //read next set to get lower 2 bits
      pixel = a; //set pixel to a
      pixel = pixel << 2; //push left to 7:2
      pixel += (b & mask); //set lower 2 from b to 1:0
      FBuffer[count++] = pixel; //put temp value in fbuffer array
      //delayMicroseconds(100);
    }
  }
  registerWrite(0x70, 0x00);   //More magic? 
  registerWrite(0x58, 0xFF);

  int temp, check; 

  do { //keep reading and testing
    temp = registerRead(0x58); //read status register
    check = temp>>6; //rightshift 6 bits so only top two stay 
  } while(check == 0x03); //while bits aren't set denoting ready state
}

// Low level register access
void Bitcraze_PMW3901::registerWrite(uint8_t reg, uint8_t value) {
  //reg |= 0x80u;

  //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));

  //digitalWrite(_cs, LOW);

  //delayMicroseconds(50);
  //SPI.transfer(reg);
  //SPI.transfer(value);
  //delayMicroseconds(50);

  //digitalWrite(_cs, HIGH);

  //SPI.endTransaction();

  //delayMicroseconds(200);
  char rg_vl[2] = {reg, value};
  bcm2835_spi_writenb(rg_vl, 2);

}

uint8_t Bitcraze_PMW3901::registerRead(uint8_t reg) {
  //reg &= ~0x80u;

  //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));

  //digitalWrite(_cs, LOW);

  //delayMicroseconds(50);
 // SPI.transfer(reg);
  //delayMicroseconds(50);
  //uint8_t value = SPI.transfer(0);
  //delayMicroseconds(100);

  //digitalWrite(_cs, HIGH);

  //delayMicroseconds(200);

  //SPI.endTransaction();
  //return value;

  char buft[2] = {reg,0x00};
  char bufr[2] = {0x00};
  bcm2835_spi_transfernb(buft,bufr,2);

  return bufr[1];
}

// Performance optimisation registers
void Bitcraze_PMW3901::initRegisters()
{
  registerWrite(0x7F, 0x00);
  registerWrite(0x61, 0xAD);
  registerWrite(0x7F, 0x03);
  registerWrite(0x40, 0x00);
  registerWrite(0x7F, 0x05);
  registerWrite(0x41, 0xB3);
  registerWrite(0x43, 0xF1);
  registerWrite(0x45, 0x14);
  registerWrite(0x5B, 0x32);
  registerWrite(0x5F, 0x34);
  registerWrite(0x7B, 0x08);
  registerWrite(0x7F, 0x06);
  registerWrite(0x44, 0x1B);
  registerWrite(0x40, 0xBF);
  registerWrite(0x4E, 0x3F);
  registerWrite(0x7F, 0x08);
  registerWrite(0x65, 0x20);
  registerWrite(0x6A, 0x18);
  registerWrite(0x7F, 0x09);
  registerWrite(0x4F, 0xAF);
  registerWrite(0x5F, 0x40);
  registerWrite(0x48, 0x80);
  registerWrite(0x49, 0x80);
  registerWrite(0x57, 0x77);
  registerWrite(0x60, 0x78);
  registerWrite(0x61, 0x78);
  registerWrite(0x62, 0x08);
  registerWrite(0x63, 0x50);
  registerWrite(0x7F, 0x0A);
  registerWrite(0x45, 0x60);
  registerWrite(0x7F, 0x00);
  registerWrite(0x4D, 0x11);
  registerWrite(0x55, 0x80);
  registerWrite(0x74, 0x1F);
  registerWrite(0x75, 0x1F);
  registerWrite(0x4A, 0x78);
  registerWrite(0x4B, 0x78);
  registerWrite(0x44, 0x08);
  registerWrite(0x45, 0x50);
  registerWrite(0x64, 0xFF);
  registerWrite(0x65, 0x1F);
  registerWrite(0x7F, 0x14);
  registerWrite(0x65, 0x60);
  registerWrite(0x66, 0x08);
  registerWrite(0x63, 0x78);
  registerWrite(0x7F, 0x15);
  registerWrite(0x48, 0x58);
  registerWrite(0x7F, 0x07);
  registerWrite(0x41, 0x0D);
  registerWrite(0x43, 0x14);
  registerWrite(0x4B, 0x0E);
  registerWrite(0x45, 0x0F);
  registerWrite(0x44, 0x42);
  registerWrite(0x4C, 0x80);
  registerWrite(0x7F, 0x10);
  registerWrite(0x5B, 0x02);
  registerWrite(0x7F, 0x07);
  registerWrite(0x40, 0x41);
  registerWrite(0x70, 0x00);

  delay(100);
  registerWrite(0x32, 0x44);
  registerWrite(0x7F, 0x07);
  registerWrite(0x40, 0x40);
  registerWrite(0x7F, 0x06);
  registerWrite(0x62, 0xf0);
  registerWrite(0x63, 0x00);
  registerWrite(0x7F, 0x0D);
  registerWrite(0x48, 0xC0);
  registerWrite(0x6F, 0xd5);
  registerWrite(0x7F, 0x00);
  registerWrite(0x5B, 0xa0);
  registerWrite(0x4E, 0xA8);
  registerWrite(0x5A, 0x50);
  registerWrite(0x40, 0x80);
}

void Bitcraze_PMW3901::setLed(bool ledOn)
{
  delay(200);
  registerWrite(0x7f, 0x14);
  registerWrite(0x6f, ledOn ? 0x1c : 0x00);
  registerWrite(0x7f, 0x00);
}


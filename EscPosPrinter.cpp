/*------------------------------------------------------------------------
  An Arduino library for the USB Thermal Printer using Epson ESC POS commands

  These printers use USB to communicate.

  This library is based on the Adafruit Thermal Printer Library.

  Adafruit invests time and resources providing this open source code.
  Please support USB and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries, with
  contributions from the open source community.  Originally based on
  Thermal library from bildr.org
  MIT license, all text above must be included in any redistribution.

Reference: https://reference.epson-biz.com/modules/ref_escpos/index.php?content_id=72
------------------------------------------------------------------------*/

#include "EscPosPrinter.h"

// ASCII codes used by some of the printer config commands:
#define ASCII_TAB "\t" // Horizontal tab
#define ASCII_LF '\n'  // Line feed
#define ASCII_FF '\f'  // Form feed
#define ASCII_CR '\r'  // Carriage return
#define ASCII_EOT 4    // End of Transmission
#define ASCII_DLE 16   // Data Link Escape
#define ASCII_DC2 18   // Device control 2
#define ASCII_ESC 27   // Escape
#define ASCII_FS 28    // Field separator
#define ASCII_GS 29    // Group separator
#define ASCII_ETX 0x03 // End Of Text
#define ASCII_NULL 0x0 // NULL

EscPosPrinter::EscPosPrinter(HardwareSerial *comPort, int8_t pinRX, int8_t pinTX)
{
  this->comPort = comPort;
  this->pinRX = pinRX;
  this->pinTX = pinTX;
}

void EscPosPrinter::init()
{
  comPort->begin(115200, SERIAL_8N1, pinRX, pinTX);
  comPort->setTimeout(5);
}

// The next four helper methods are used when issuing configuration
// commands, printing bitmaps or barcodes, etc.  Not when printing text.

void EscPosPrinter::writeBytes(uint8_t a)
{
  comPort->write(a);
}

void EscPosPrinter::writeBytes(uint8_t a, uint8_t b)
{
  uint8_t cmd[2] = {a, b};
  comPort->write(cmd, sizeof(cmd));
}

void EscPosPrinter::writeBytes(uint8_t a, uint8_t b, uint8_t c)
{
  uint8_t cmd[3] = {a, b, c};
  comPort->write(cmd, sizeof(cmd));
}

void EscPosPrinter::writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  uint8_t cmd[4] = {a, b, c, d};
  comPort->write(cmd, sizeof(cmd));
}

// The underlying method for all high-level printing (e.g. println()).
// The inherited Print class handles the rest!
size_t EscPosPrinter::write(uint8_t c)
{
  if (c != 0x13)
  { // Strip carriage returns
    comPort->write(c);
  }
  return 1;
}

void EscPosPrinter::test()
{
  println(F("Hello World!"));
  feed(2);
}

void EscPosPrinter::testPage()
{
  char commandTest[] = {ASCII_GS, '(', 'A', 2, 0, 0, 3};
  comPort->write(commandTest, sizeof(commandTest));
}

/*************-Barcode-********************
Definition of sendStart

-Width multiplier-
0x1D = Group separator GS
0x77 = 'w' = Width Select 
0x03 = Width
-Height in dots-
0x1D  = Group separator GS
0x68  = 'h' = Height Select 
0x78 = Height
-HRI, (Human Readable Interpretation) FONT-
0x1D = Group separator GS
0x66 = 'f' = Font select
0x00 = Default
-HRI PRINTING POSITION-
0x1D  = Group separator GS
0x48  = 'H' = Position select
0x01 1=above 2=below
-Barcode generator-
0x1D  = Group separator GS
0x6B  = 'k'
0x05 ITF type
*********************************************
More about this subject:
https://escpos.readthedocs.io/en/latest/imaging.html#barcode-generator-1-1d-6b-m-d1-dk-00-rel
*********************************************/

void EscPosPrinter::setupBarcode()
{
  //char sendStart[15] = {0x1D, 0x77, 0x04, 0x1D, 0x68, 0x78, 0x1D, 0x66, 0x01, 0x1D, 0x48, 0x00, 0x1D, 0x6B, 0x05};
  char sendStart[9] = {0x1D, 0x77, 0x04, 0x1D, 0x68, 0x78, 0x1D, 0x6B, 0x05};

  for (size_t i = 0; i < sizeof(sendStart); i++)
  {
    comPort->write(sendStart[i]);
  }
}

void EscPosPrinter::inverseOn()
{
  writeBytes(ASCII_GS, 'B', 1);
}

void EscPosPrinter::setArea()
{
  writeBytes(0x1D, 0x57, 0x2c, 0x01);
  writeBytes(0x1B, 0x24, 0x64, 0x0);
}

void EscPosPrinter::inverseOff()
{
  writeBytes(ASCII_GS, 'B', 0);
}

void EscPosPrinter::upsideDownOn()
{
  writeBytes(ASCII_ESC, '{', 1);
}

void EscPosPrinter::upsideDownOff()
{
  writeBytes(ASCII_ESC, '{', 0);
}

void EscPosPrinter::emphasizedOn()
{
  writeBytes(ASCII_ESC, 'E');
}

void EscPosPrinter::emphasizedOff()
{
  writeBytes(ASCII_ESC, 'F');
}

void EscPosPrinter::justify(char value)
{
  uint8_t pos = 0;

  switch (toupper(value))
  {
  case 'L':
    pos = 0;
    break;
  case 'C':
    pos = 1;
    break;
  case 'R':
    pos = 2;
    break;
  }

  writeBytes(ASCII_ESC, 'a', pos);
}

// Feeds by the specified number of lines
void EscPosPrinter::feed(uint8_t x)
{
  writeBytes(ASCII_ESC, 'd', x);
}

// Feeds by the specified number of individual pixel rows
void EscPosPrinter::feedRows(uint8_t rows)
{
  writeBytes(ASCII_ESC, 'J', rows);
}

void EscPosPrinter::flush()
{
  writeBytes(ASCII_FF);
}

void EscPosPrinter::autoCut()
{
  writeBytes(ASCII_ESC, 0x76, 0);
}

void EscPosPrinter::rotateStart()
{
  writeBytes(0x13, 0x44);
}

void EscPosPrinter::rotateStop()
{
  writeBytes(0x0B);
}

void EscPosPrinter::sendETX()
{
  writeBytes(ASCII_ETX);
}

void EscPosPrinter::setSize(char value)
{
  uint8_t size;

  switch (toupper(value))
  {
  default: // Small: standard width and height
    size = 0x00;
    break;
  case 'M': // Medium: double height
    size = 0x01;
    break;
  case 'L': // Large: double width and height
    size = 0x11;
    break;
  case 'X': // XL: 3 width 3 height
    size = 0x22;
  }

  writeBytes(ASCII_GS, '!', size);
}

void EscPosPrinter::setSize(uint8_t height, uint8_t width)
{
  uint8_t size = ((width & 0x7) << 3) | (height & 0x7);

  writeBytes(ASCII_GS, '!', size);
}

// Underlines of different weights can be produced:
// 0 - no underline
// 1 - normal underline
// 2 - thick underline
void EscPosPrinter::underlineOn(uint8_t weight)
{
  if (weight > 2)
    weight = 2;
  writeBytes(ASCII_ESC, '-', weight);
}

void EscPosPrinter::underlineOff()
{
  writeBytes(ASCII_ESC, '-', 0);
}

void EscPosPrinter::setRelPos()
{
  //writeBytes(ASCII_ESC, 0x5C, 0x64, 0x00);
  writeBytes(ASCII_ESC, 0x5C, 0x02, 0x20);
}

// Take the printer offline. Print commands sent after this will be
// ignored until 'online' is called.
void EscPosPrinter::offline()
{
  writeBytes(ASCII_ESC, '=', 0);
}

// Take the printer back online. Subsequent print commands will be obeyed.
void EscPosPrinter::online()
{
  writeBytes(ASCII_ESC, '=', 1);
}

// Check the status of the paper using the printer's self reporting
// ability.  Returns true for paper, false for no paper.
// Might not work on all printers!
bool EscPosPrinter::hasPaper()
{
  //  writeBytes(ASCII_DLE, ASCII_EOT, 4);
  writeBytes(ASCII_GS, 'r', 1);
  //  writeBytes(ASCII_ESC, 'v');

  int status = 0;
  for (size_t i = 0; i < 10; i++)
  {
    if (comPort->available())
    {
      status = comPort->read();
      break;
    }
  }

  return !(status & 0b00001100);
}

int EscPosPrinter::getStatus(int stateIn, byte *statusArray) // Possible to ask about more faults but this should be enough, 0x01,0x02,0x05 and not 0x03,0x04
{
  int stateOut = stateIn;
  switch (statusState)
  {
  case 0:
  {
    clearRxBuffer();
    statusArray[0] = 0;
    statusArray[1] = 0;
    statusArray[2] = 0;
    writeBytes(0x10, 0x04, 0x01);
    statusTs = millis();
    statusState++;
  }
  break;

  case 1:
  {
    statusState = waitCase(statusTs, 10, statusState);
  }
  break;

  case 2:
  {
    if (comPort->available())
    {
      byte statusByte = comPort->read();
      printlnV(statusByte, HEX);
      statusArray[0] = statusByte;
      if (statusByte == 0x16) // OK
      {
        statusState = 10; // Go to chute sensor.
      }
      else
      {
        writeBytes(0x10, 0x04, 0x02);
        statusTs = millis();
        statusState++;
      }
    }
    else if (millis() - statusTs > 200)
    {
      debugD("Com error status printer");
      stateOut = -99; // No answer
      statusState = 0;
    }
  }
  break;

  case 3:
  {
    statusState = waitCase(statusTs, 10, statusState);
  }
  break;

  case 4:
  {
    if (comPort->available())
    {
      byte statusByte = comPort->read();
      debugD("ErrorCode:");
      printlnD(statusByte, HEX);
      statusArray[1] = statusByte;
      stateOut = stateIn + 1;
      statusState = 0;
    }
    else if (millis() - statusTs > 200)
    {
      debugD("Com error status printer");
      stateOut = -99; // No answer
      statusState = 0;
    }
  }
  break;

  case 10:
  {
    writeBytes(0x10, 0x04, 0x05);
    statusTs = millis();
    statusState++;
  }
  break;

  case 11:
  {
    statusState = waitCase(statusTs, 10, statusState);
  }
  break;

  case 12:
  {
    if (comPort->available())
    {
      byte statusByte = comPort->read();
      statusArray[2] = statusByte;
      printlnV(statusByte, HEX);
      statusState = 0;
      stateOut = stateIn + 1;
    }
    else if (millis() - statusTs > 200)
    {
      debugD("Com error status printer chute");
      stateOut = -99; // No answer
      statusState = 0;
    }
  }
  break;
  }

  return (stateOut);
}

void EscPosPrinter::clearRxBuffer()
{
  int bytesAtPort = comPort->available();
  if (bytesAtPort > 0)
  {
    for (size_t i = 0; i < bytesAtPort; i++)
    {
      comPort->read();
    }
  }
}
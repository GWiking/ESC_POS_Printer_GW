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
  ------------------------------------------------------------------------*/

#ifndef EscPosPrinter_H
#define EscPosPrinter_H

#include "Arduino.h"
#include "SerialDebug.h"
#include "tools.h"

class EscPosPrinter : public Print
{
public:
    // IMPORTANT: constructor syntax has changed from prior versions
    // of this library.  Please see bills in the example code!

    EscPosPrinter(){}; // Default constructor, do not use
    EscPosPrinter(HardwareSerial *comPort, int8_t pinRX, int8_t pinTX);
    size_t
    write(uint8_t c);
    void
    feed(uint8_t x = 1),
        feedRows(uint8_t),
        flush(),
        inverseOff(),
        inverseOn(),
        justify(char value),
        offline(),
        online(),
        setupBarcode(),
        setSize(char value),
        setSize(uint8_t height, uint8_t width),
        test(),
        testPage(),
        underlineOff(),
        underlineOn(uint8_t weight = 1),
        upsideDownOff(),
        upsideDownOn(),
        emphasizedOn(),
        emphasizedOff(),
        autoCut(),
        rotateStart(),
        rotateStop(),
        sendETX(),
        setArea(),
        setRelPos(),
        init();

    bool hasPaper();
    int getStatus(int stateIn, byte *statusArray, size_t statusArrayLength);
    int paperLowStatus(int stateIn, bool &paperStatus);
    // int checkPaperInChute(int stateIn);
    int8_t pinRX;
    int8_t pinTX;

private:
    
    HardwareSerial *comPort;
    void
    writeBytes(uint8_t a),
        writeBytes(uint8_t a, uint8_t b),
        writeBytes(uint8_t a, uint8_t b, uint8_t c),
        writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    void clearRxBuffer();
    int statusState = 0;
    uint32_t statusTs;
    int paperLowState = 0;
};

#endif // EscPosPrinter_H

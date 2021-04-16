#ifndef RTCDS1307_H
#define RTCDS1307_H

#include "Wire.h"

#define RTCDS1307_TIME 0x00
#define RTCDS1307_MODE 0x02
#define RTCDS1307_DATE 0x03
#define RTCDS1307_CONTROL 0x07
#define RTCDS1307_RAM 0x08

#define RTCDS1307_FREQ1H 0
#define RTCDS1307_FREQ4K 1
#define RTCDS1307_FREQ8K 2
#define RTCDS1307_FREQ32K 3

#define RTCDS1307_MODE12 0
#define RTCDS1307_MODE24 1

class RTCDS1307 {
  private:
    uint8_t _address;
    uint8_t *_buffer = (uint8_t *) malloc(5);

    uint8_t bcd(uint8_t val);
    uint8_t decimal(uint8_t val);
    bool read(uint8_t address, uint8_t length);
    bool write(uint8_t address, uint8_t length);

  public:
    RTCDS1307(uint8_t address);
    bool begin();
    bool getDate(uint8_t &Y, uint8_t &M, uint8_t &D, uint8_t &WD);
    bool getMode();
    bool getTime(uint8_t &h, uint8_t &m, uint8_t &s);
    bool getTime(uint8_t &h, uint8_t &m, uint8_t &s, bool &period);
    bool isLeapYear(uint16_t Y);
    bool read(uint8_t address, uint8_t *&buffer, uint8_t length);
    bool setControl(bool output, bool square, uint8_t frequency);
    bool setDate(uint8_t Y, uint8_t M, uint8_t D);
    bool setMode(bool state);
    bool setTime(uint8_t h, uint8_t m, uint8_t s, bool mode = 0, bool period = 0);
    uint8_t wday(uint16_t Y, uint8_t M, uint8_t D);
    bool write(uint8_t address, uint8_t *buffer, uint8_t length);
};

#endif

/*
 * licenced with Creative Commons: CC0 1.0 Universal (CC0 1.0)
 */

#ifndef __PICOSPI_H__
#define __PICOSPI_H__

#ifndef ARDUINO_ARCH_RP2040
#  error This version of the PicoSPI library only works for the Raspberry Pi Pico board
#endif

#include <hardware/spi.h>
#include <Arduino.h>

class PicoSPI
{
 public:
  PicoSPI (int _unit)
  {
    unit = _unit ;
    //base = (spi_hw_t *) (unit == 0 ? 0x4003C000 : 0x40040000) ;
    configured = false ;
  }

  bool configure (byte _sclk_pin, byte _mosi_pin, byte _miso_pin, byte _cs_pin, int _frequency, byte _mode, bool _auto_transactions) ;

  void beginTransaction (void) ;
  byte transfer (byte in) ;
  void endTransaction (void) ;

 private:
  bool check_configuration (void) ;
  void actually_configure (void) ;
  spi_hw_t * base ;
  int unit ;
  bool configured ;
  int frequency ;
  bool auto_transactions ;
  byte sclk_pin, mosi_pin, miso_pin, cs_pin, mode ;
};


// the two SPI units
extern PicoSPI PicoSPI0 ;
extern PicoSPI PicoSPI1 ;

#endif

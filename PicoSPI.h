#ifndef __PICOSPI_H__
#define __PICOSPI_H__

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

#  ifndef SPI_MODE0
#    define SPI_MODE0 0x00
#    define SPI_MODE1 0x04
#    define SPI_MODE2 0x08
#    define SPI_MODE3 0x0C
#  endif

#endif

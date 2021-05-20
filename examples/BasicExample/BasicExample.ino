/*
 * licenced with Creative Commons: CC0 1.0 Universal (CC0 1.0)
 */

#include <PicoSPI.h>


// PicoSPI library provides two objects for the RP2040 chip's two SPI units,
// PicoSPI0 and PicoSPI1.

// >>> It supports master mode only <<<

// The units are completely independent but each is limited to a particular subset of
// the Pico's pins, as shown on the various pinout diagrams.  You would typically choose
// a contiguous set of 4 pins for a particular SPI unit, although this is not required.

// The configuration arguments are 4 pins, namely SCLK, MOSI, MISO and CS, in that order,
// and all must be supplied.  Also a frequency, an SPI mode, and a flag "auto_transations".
// If auto_transations is true, every byte will automatically drive CS low for its transmission
// If auto_transations is false, you have to call beginTransaction() and endTransaction()
// explicitly to drive CS, but you can group multiple bytes within a "transaction".

// The PicoSPI library forces you to explicitly configure an SPI unit before you can
// use it - this is good as the existing different Arduino ports of the Pico are different
// about the default pins used!  Explicit is always good for pin assignments(!)

// The clock frequency used is constrained automatically in the range 2500 - 25000000 (2.5kHz to 25MHz)
// as this is what the hardware can usefully support.

// Again, note it supports master mode only....

void setup() 
{
  // configure SPI0 for
  // SCLK = pin 2    (alternative pins are 6, 18, 22)
  // MOSI = pin 3    (alternative pins are 7, 19)
  // MISO = pin 4    (alternative pins are 0, 16, 20)
  // CS   = pin 5    (alternative pins are 1, 17, 21)
  // and for 1MHz clock, in mode 0, without automatic transactions
  if (! PicoSPI0.configure (2, 3, 4, 5, 1000000, 0, false))
  {
    while (true) {}
  }
  // note that if you pass an incompatible pin (check the pinout diagrams), or if the mode
  // isn't valid, the configure method return false and the SPI won't work - check the result
  // from configure().

  // configure SPI1 for
  // SCLK = pin 10    (alternative pins are 14, 26)
  // MOSI = pin 11    (alternative pins are 7, 27)
  // MISO = pin 8    (alternative pins are 12, 28)
  // CS   = pin 9    (alternative pins are 13, 25 - but note 25 is only the LED on the PCB)
  // and for 1MHz clock, in mode 0, without automatic transactions
  if (! PicoSPI1.configure (10, 11, 8, 9, 1000000, 0, false))
  {
    while (true) {}
  }
}

void loop()
{
  static byte b = 0 ;
  
  PicoSPI0.beginTransaction() ;
  PicoSPI0.transfer (b) ;  // increment the byte after sending it twice in one transaction
  PicoSPI0.transfer (b++) ;
  PicoSPI0.endTransaction() ;
  // note this example ignores the returned byte(s)

  // this example only configures SPI1 and doesn't use.

  delayMicroseconds (100) ; // to make the output easier to see on a scope
}

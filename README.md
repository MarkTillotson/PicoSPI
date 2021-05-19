# PicoSPI
Lightweight SPI library for RasPi Pico board using the RP2040 microcontroller

The PicoSPI library provides two objects for the RP2040 chip's two SPI units,
namely PicoSPI0 and PicoSPI1.

Note this library works identically on the official Arduino Pico port, as well
as Earle F. Philhower's Arduino Pico port.  And it supports both SPI0 and SPI1.

It was written in response to the SPI support in these two implementations being
different.

** NOTE: it supports SPI master mode only **

The SPI units are completely independent but each is limited to a particular subset of
the Pico's pins, as shown on the various pinout diagrams for the Pico.

You would typically choose a contiguous set of 4 pins for a particular SPI unit,
although this is not required by this library.

To use the library

```
#include <PicoSPI.h>

void setup ()
{
  ...
  bool success = PicoSPI0.configure (sclk, mosi, miso, cs, frequency, mode, auto_transations) ;
  if (!success)
    Serial.println ("PicoSPI configuration was invalid") ;
}

void loop ()
{
  ...
  PicoSPI0.beginTransaction () ;
  recv_val = PicoSPI0.transfer (transmit_val) ;
  ...
  PicoSPI0.endTransaction () ;
  ...
}
```

The configuration arguments are 4 pins, namely SCLK, MOSI, MISO and CS, in that order,
and all must be supplied.  Also a frequency, an SPI mode, and a flag "auto_transations".
If auto_transations is true, every byte will automatically drive CS low for its transmission
If auto_transations is false, you have to call beginTransaction() and endTransaction()
explicitly to drive CS, but you can group multiple bytes within a "transaction".

The PicoSPI library forces you to explicitly configure an SPI unit before you can
use it - this is good as the existing different Arduino ports of the Pico are different
about the default pins used!  Explicit is always good for pin assignments(!)

The clock frequency used is constrained automatically in the range 2500 - 25000000 (2.5kHz to 25MHz)
as this is what the hardware can usefully support.

example provided:  examples/BasicExample/BasicExample.ino - shows how to configure SPI0 and SPI1,
exercises SPI0.

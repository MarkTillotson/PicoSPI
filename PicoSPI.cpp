/*
 * licenced with Creative Commons: CC0 1.0 Universal (CC0 1.0)
 */

#include <hardware/structs/iobank0.h>

#include "PicoSPI.h"

////// utilities

static void select_higher_drive_for_pin (int gpio)
{
  hw_write_masked (&padsbank0_hw->io[gpio],
		   (3 << PADS_BANK0_GPIO0_DRIVE_LSB),  // code 3 is 12mA drive
		   PADS_BANK0_GPIO0_DRIVE_BITS) ;
  hw_write_masked (&padsbank0_hw->io[gpio],
		   (1 << PADS_BANK0_GPIO0_SLEWFAST_LSB), // 1 means fast slew
		   PADS_BANK0_GPIO0_SLEWFAST_BITS) ;
}


////// configuration

bool PicoSPI::configure (byte _sclk_pin, byte _mosi_pin, byte _miso_pin, byte _cs_pin,
			 int _frequency, byte _mode, bool _auto_transactions)
{
  // setup instance
  sclk_pin = _sclk_pin ;
  mosi_pin = _mosi_pin ;
  miso_pin = _miso_pin ;
  cs_pin   = _cs_pin ;
  mode = _mode ;
  frequency = _frequency ;
  auto_transactions = _auto_transactions ;

  // validate
  configured = check_configuration() ;
  if (!configured)
    return false ;
  else
  {
    actually_configure () ;
    return true ;
  }
}

void PicoSPI::actually_configure (void)
{
  // calculate clock divisors - this code is rough & ready at the moment.
  int mclk = 133000000 ;
  int divide_by = mclk / frequency + 1 ;
  int divisor = divide_by ;
  int extra_divisor = (divisor+253) / 254 ;
  if (extra_divisor > 1)
    divisor /= extra_divisor ;
  divisor &= ~1 ;  // force even

  //Serial.print (divisor) ; Serial.print (" ") ; Serial.println (extra_divisor) ;

  // switch pins to SPI role
  gpio_set_function (sclk_pin, GPIO_FUNC_SPI) ;
  gpio_set_function (mosi_pin, GPIO_FUNC_SPI) ;
  gpio_set_function (miso_pin, GPIO_FUNC_SPI) ;
  if (auto_transactions)
    gpio_set_function (cs_pin, GPIO_FUNC_SPI) ;
  else
  { // if we manage CS pin explicitly, make it an output
    gpio_init_mask (1 << cs_pin) ;
    gpio_set_dir (cs_pin, true) ;
    gpio_put (cs_pin, true) ;
  }

  // get the base address for SPI unit
  // base->cr1 = 0x0 ;
  byte polarity = (mode >> 1) & 1 ;
  byte phase    = mode & 1 ;
  byte bits     = 8 ;  // only byte transfers currently handled - the hardware can do 4 to 16 bits

  // control reg 0 fields
  hw_write_masked (&base->cr0, (extra_divisor-1) << SPI_SSPCR0_SCR_LSB, SPI_SSPCR0_SCR_BITS) ;
  hw_write_masked (&base->cr0, phase             << SPI_SSPCR0_SPH_LSB, SPI_SSPCR0_SPH_BITS) ;
  hw_write_masked (&base->cr0, polarity          << SPI_SSPCR0_SPO_LSB, SPI_SSPCR0_SPO_BITS) ;
  hw_write_masked (&base->cr0, (bits-1)          << SPI_SSPCR0_DSS_LSB, SPI_SSPCR0_DSS_BITS) ;
  // clock prescale reg
  hw_write_masked (&base->cpsr, divisor          << SPI_SSPCPSR_CPSDVSR_LSB, SPI_SSPCPSR_CPSDVSR_BITS) ;
  // control reg 1 field - enable it
  hw_write_masked (&base->cr1,  1                << SPI_SSPCR1_SSE_LSB, SPI_SSPCR1_SSE_BITS) ;


  if (frequency >= 8000000) // select high drive and fast slew for SCLK and MOSI if 8MHz or more.
  {
    select_higher_drive_for_pin (sclk_pin) ;
    select_higher_drive_for_pin (mosi_pin) ;
  }
}


////// operations

void PicoSPI::beginTransaction (void)
{
  if (configured && !auto_transactions)
    gpio_put (cs_pin, false) ;
}

byte PicoSPI::transfer (byte in)
{
  if (!configured)
    return 0x00 ;
  base->dr = in ;   // write value to TX FIFO
  while (base->sr & SPI_SSPSR_BSY_BITS) {}  // busy-wait
  return (byte) (base->dr) ;  // read RX FIFO
}

void PicoSPI::endTransaction (void)
{
  if (configured && !auto_transactions)
    gpio_put (cs_pin, true) ;
}


////// configuration checks

#define VALID_PICO_PINS 0x1E7FFFFF   // mask for all GPIO pins brought out on the Pico board (including the LED!)
#define SPI0_PINS      (0x00FF00FF & VALID_PICO_PINS)  // partitions of pins into SPI0 and SPI1, masked by above
#define SPI1_PINS      (0xFF00FF00 & VALID_PICO_PINS)

#define SPI0_MISOS     (0x11111111 & SPI0_PINS)  // valid pins for each role and SPI unit
#define SPI0_CSS       (0x22222222 & SPI0_PINS)
#define SPI0_SCLKS     (0x44444444 & SPI0_PINS)
#define SPI0_MOSIS     (0x88888888 & SPI0_PINS)

#define SPI1_MISOS     (0x11111111 & SPI1_PINS)
#define SPI1_CSS       (0x22222222 & SPI1_PINS)
#define SPI1_SCLKS     (0x44444444 & SPI1_PINS)
#define SPI1_MOSIS     (0x88888888 & SPI1_PINS)

bool PicoSPI::check_configuration (void)
{
  switch (unit)
  {
  case 0:
    base = (spi_hw_t *) 0x4003C000 ;
    if (((1 << sclk_pin) & SPI0_SCLKS) == 0) return false ; // might be nice here to indicate
    if (((1 << mosi_pin) & SPI0_MOSIS) == 0) return false ; // what the precise problem was
    if (((1 << miso_pin) & SPI0_MISOS) == 0) return false ;
    if (((1 << cs_pin)   & SPI0_CSS)   == 0) return false ;
    break ;

  case 1:
    base = (spi_hw_t *) 0x40040000 ;
    if (((1 << sclk_pin) & SPI1_SCLKS) == 0) return false ;
    if (((1 << mosi_pin) & SPI1_MOSIS) == 0) return false ;
    if (((1 << miso_pin) & SPI1_MISOS) == 0) return false ;
    if (((1 << cs_pin)   & SPI1_CSS)   == 0) return false ;
    break ;

  default: return false ;  // unit wasn't 0 or 1...
  }

  // limit frequencies to values the hardware can handle reasonably
  // 2.5kHz is about the slowest for the divider, above 25MHz and the waveforms are getting poor.
  if (frequency < 2500)
    frequency = 2500 ;
  if (frequency > 25000000)
    frequency = 25000000 ;

  if (mode > 3) return false ;
  return true ;  // all checks passed
}

PicoSPI PicoSPI0(0) ;
PicoSPI PicoSPI1(1) ;

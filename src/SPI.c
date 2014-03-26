/*
 * SPI.c
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:
 *
 */
#include "SPI.h"

void SPI_Delay_us (uint16_t usec)
{
   uint32_t i=0;
   uint32_t top;
   #if  SPI_CLOCK == 8000000
   top = usec * 2;
   #elif SPI_CLOCK == 16000000
   top = usec * 4;
   #elif SPI_CLOCK == 24000000
   top = usec * 6;
   #else
      #error "Specify either A defined clock value or write some code ;)"
   #endif
   while ( ++i < top );
}


int SPI_ConnectDI (SPI_t *spi, SPI_PinOut_t pfun) {
   spi->DI = pfun;
}

int SPI_ConnectDO (SPI_t *spi, SPI_PinIn_t pfun) {
   spi->DO = pfun;
}

int SPI_ConnectCLK (SPI_t *spi, SPI_PinOut_t pfun) {
   spi->CLK = pfun;
}

int SPI_ConnectCS (SPI_t *spi, SPI_PinOut_t pfun) {
   spi->CS = pfun;
}


/*-----------------------------------------------------------------------*/
/* Transmit bytes to the MMC (bitbanging)                                */
/*-----------------------------------------------------------------------*/

void SPI_Tx (SPI_t *spi, const unsigned char* buff, unsigned int bc)
{
   unsigned char d;
   unsigned char mask=0x80, i;

   while (bc--)
   {
      d = *buff++;    // Get a byte to be sent
      for (i=0; i<8 ; ++i)
      {
         /*
          * Send 8 bit MSB first
          */
         if (d & (mask>>i))
            spi->DI(1);
         else
            spi->DI(0);
         spi->CLK(1);   // Clock out pin
         SPI_Delay_us(1);
         spi->CLK(0);
         SPI_Delay_us(1);
      }
      SPI_Delay_us(10);
   };
}



/*-----------------------------------------------------------------------*/
/* Receive bytes from the MMC (bitbanging)                               */
/*-----------------------------------------------------------------------*/
void SPI_Rx (SPI_t *spi, unsigned char* buff, unsigned int bc)
{
   unsigned char r;
   unsigned char i;

   spi->DI(1); // Send 0xFF as we read.
   while (bc--)
   {
      for (i=r=0 ; i<8 ; ++i)
      {
         r <<= 1;
         if (spi->DO())
            r++;
         spi->CLK(1);   // Clock out pin
         SPI_Delay_us(1);
         spi->CLK(0);
         SPI_Delay_us(1);
      }
      *buff++ = r;
      SPI_Delay_us(10);
   }
}

void SPI_CS (SPI_t *spi, unsigned char on)
{
   if (on)  spi->CS (1);
   else     spi->CS (0);
}




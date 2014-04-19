/*
 * SPI.h
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
#ifndef  __SPI_h__
#define  __SPI_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm32f10x_systick.h>
#include <std/stime.h>

#define SPI_CLOCK    (CLOCK)

typedef void (*SPI_PinOut_t) (int8_t);
typedef uint8_t (*SPI_PinIn_t) (void);


typedef struct
{
   SPI_PinOut_t   DI;
   SPI_PinIn_t    DO;
   SPI_PinOut_t   CLK;
   SPI_PinOut_t   CS;
}SPI_t;

int SPI_ConnectDI (SPI_t *spi, SPI_PinOut_t pfun);
int SPI_ConnectDO (SPI_t *spi, SPI_PinIn_t pfun);
int SPI_ConnectCLK (SPI_t *spi, SPI_PinOut_t pfun);
int SPI_ConnectCS (SPI_t *spi, SPI_PinOut_t pfun);

void SPI_DeInit (void);
void SPI_Init (void);

void SPI_Delay_us (uint16_t usec);
void SPI_Tx (SPI_t *spi, const unsigned char* buff, unsigned int bc);
void SPI_Rx (SPI_t *spi, unsigned char* buff, unsigned int bc);
void SPI_CS (SPI_t *spi, unsigned char on);

#ifdef __cplusplus
 }
#endif

#endif   //#ifndef  __SPI_h__

/*
 * \file ss_display.h
 * \brief
 *    This is a Seven Segment display module
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Houtouridis Christos. The intellectual
 * and technical concepts contained herein are proprietary to
 * Houtouridis Christos and are protected by copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Houtouridis Christos.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       11/2013
 * Version:    0.1
 *
 */

#ifndef __ss_display_h__
#define __ss_display_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>


/* ============ User Defines ============== */
#define  DEL                  (0x7F)
#define  SEVEN_SEG_DIGITS     (3)
#define  SEVEN_SEG_FB_SIZE    (3)


typedef void (*SSD_Pin_t) (int8_t);

/*!
 * Seven Segment Display Pin assignements.
 * Each one can be called xx.SSA(1); or xx.SSA(0); in order to set
 * or clear the corresponding pin.
 *
 * \note These pointers MUST to be assigned from main application.
 */
typedef volatile struct
{
   SSD_Pin_t  SSA;     /*!< Pointer for SSA pin */
   SSD_Pin_t  SSB;     /*!< Pointer for SSB pin */
   SSD_Pin_t  SSC;     /*!< Pointer for SSC pin */
   SSD_Pin_t  SSD;     /*!< Pointer for SSD pin */
   SSD_Pin_t  SSE;     /*!< Pointer for SSE pin */
   SSD_Pin_t  SSF;     /*!< Pointer for SSF pin */
   SSD_Pin_t  SSG;     /*!< Pointer for SSG pin */
   SSD_Pin_t  SSDP;    /*!< Pointer for SSDP pin */
   SSD_Pin_t  SSx[SEVEN_SEG_DIGITS];
}SSD_IO_t;

/*!
 * Seven Segment Display Public Data struct
 */
typedef struct
{
   SSD_IO_t IO;
   uint8_t  fb[SEVEN_SEG_FB_SIZE];   //Frame Buffer
   uint8_t  cur;        //cursor used for the buffer writing
   uint8_t  disp;       //Shows which digit of the display is ON
   uint8_t  blink;      //Blink flag
   clock_t  blink_time; // Blink time
}SSD_t;
extern SSD_t SSD;

typedef void (*SSD_Pin_t) (int8_t);



/* ================ API ================ */
void SSD_Service (void);
int  SSD_Connect (volatile SSD_Pin_t *sio, SSD_Pin_t pfun);
int  SSD_putchar (int ch);
void SSD_BlinkEnable (void);
void SSD_BlinkDisable (void);


#ifdef __cplusplus
}
#endif

#endif //#ifndef __ss_display_h__

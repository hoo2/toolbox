/*
 * \file tbx_iotypes.h
 * \brief
 *    An In Out type definition for the toolbox.
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
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
 */
#ifndef __tbx_iotypes_h__
#define __tbx_iotypes_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <tbx_ioctl.h>


typedef uint8_t  byte_t;         /*!< 8 bits wide */
typedef uint16_t word_t;         /*!< 16 bits wide */
typedef uint32_t dword_t;        /*!< 32 bits wide */

typedef uint32_t bytecount_t;    /*!< general counter */
typedef uint32_t address_t;      /*!< general index/address type */

typedef int32_t  iterator_t;     /*!< general iterator type */

/*!
 * This is a toolbox wide generic driver status type.
 * \note
 *    DRV_NOINIT = 0, so after memset to zero called by XXXX_deinit() the
 *    module/device will automatically set to NOINIT state.
 */
typedef enum {
   DRV_NODEV=-1,     /*!< No device/module */
   DRV_NOINIT=0,     /*!< Module/Device exist but no initialized */
   DRV_READY,        /*!< Module/Device initialized succesfully */
   DRV_BUSY,         /*!< Module/Device busy */
   DRV_TIMEOUT,      /*!< Module/Device timeout */
   DRV_AWAIT,
   //DRV_COMPLETE,     /*!< Module/device operation complete status */
   DRV_ERROR         /*!< Module/Device error */
}drv_status_en;


/*!
 * Pin function pointers
 * \note
 *    These function pointers do not correspond to pin levels.
 *    They correspond to the enable/disable functionality of that pin.
 */

typedef enum {
   drv_pin_disable = 0,
   drv_pin_input,
   drv_pin_output
}drv_pin_dir_en;

typedef uint8_t (*drv_pinin_ft)  (void);
typedef    void (*drv_pinout_ft) (uint8_t);
typedef uint8_t (*drv_pinio_ft)  (uint8_t);
typedef    void (*drv_pindir_ft) (drv_pin_dir_en);

/*!
 * Analog input function pointer
 */
typedef  float (*drv_ain_f_ft) (void);
typedef    int (*drv_ain_i_ft) (void);

/*!
 * Digital I/O function pointers
 * \note
 *    These function pointers do not correspond to circuit/port/pin levels.
 *    They correspond to the enable/disable functionality.
 */
typedef  uint8_t (*drv_din_ft)   (void);
typedef     void (*drv_dout_ft)  (uint8_t);
typedef     void (*drv_out_i_ft) (int);
typedef     void (*drv_out_f_ft) (float);

/*!
 * I2C I/O Function pointer
 */
typedef   void (*drv_i2c_start_ft) (void *);
typedef   void (*drv_i2c_stop_ft) (void *);
typedef byte_t (*drv_i2c_rx_ft) (void *, uint8_t, int);
typedef    int (*drv_i2c_tx_ft) (void *, byte_t, int);
typedef drv_status_en (*drv_i2c_ioctl_ft) (void *, ioctl_cmd_t, ioctl_buf_t);


/*
 * Complex types
 */
typedef double _Complex    complex_d_t;
typedef float _Complex     complex_f_t;

#ifdef __GNUC__
typedef int _Complex       complex_i_t;
#endif


#define  tbx_real(_z)       ( ((double*)&(_z)) [0] )
#define  tbx_imag(_z)       ( ((double*)&(_z)) [1] )
#define  tbx_realf(_z)      ( ((float*)&(_z)) [0] )
#define  tbx_imagf(_z)      ( ((float*)&(_z)) [1] )
#ifdef __GNUC__
#define  tbx_reali(_z)      ( ((int*)&(_z)) [0] )
#define  tbx_imagi(_z)      ( ((int*)&(_z)) [1] )
#endif

/*
 * Cartesian 2D coordinate types
 */
typedef struct {
   double   x;
   double   y;
}cart2_d_t;

typedef struct {
   float    x;
   float    y;
}cart2_f_t;

typedef struct {
   int32_t  x;
   int32_t  y;
}cart2_i32_t;

typedef struct {
   uint32_t    x;
   uint32_t    y;
}cart2_ui32_t;

/*
 * Cartesian 3D coordinate types
 */
typedef struct {
   double   x;
   double   y;
   double   z;
}cart3_d_t;

typedef struct {
   float    x;
   float    y;
   float    z;
}cart3_f_t;

typedef struct {
   int32_t  x;
   int32_t  y;
   int32_t  z;
}cart3_i32_t;

typedef struct {
   uint32_t    x;
   uint32_t    y;
   uint32_t    z;
}cart3_ui32_t;


/*
 * Polar coordinate types
 */
typedef struct {
   double   abs;     // Amplitude
   double   arg;     // Argument - angle
}polar_d_t;

typedef struct {
   float    abs;     // Amplitude
   float    arg;     // Argument - angle
}polar_f_t;

typedef struct {
   int32_t  abs;     // Amplitude
   int32_t  arg;     // Argument - angle
}polar_i32_t;

typedef struct {
   uint32_t abs;     // Amplitude
   uint32_t arg;     // Argument - angle
}polar_ui32_t;



#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
  #define __STATIC_INLINE  static __inline

#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
  //#define __STATIC_INLINE  static inline

#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline

#elif defined ( __TMS470__ )
  #define __ASM            __asm                                      /*!< asm keyword for TI CCS Compiler       */
  #define __STATIC_INLINE  static inline

#elif defined ( __TASKING__ )
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
  #define __STATIC_INLINE  static inline

#elif defined ( __CSMC__ )
  #define __packed
  #define __ASM            _asm                                      /*!< asm keyword for COSMIC Compiler      */
  #define __INLINE         inline                                    /*use -pc99 on compile line !< inline keyword for COSMIC Compiler   */
  #define __STATIC_INLINE  static inline

#endif

#ifdef __cplusplus
}
#endif

#endif //#ifndef __tbx_iotypes_h__

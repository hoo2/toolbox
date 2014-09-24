/*!
 * \file s25fs_spi.h
 * \brief
 *    A target independent spansion s25fs spi flash memory
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
#ifndef __s25fs_spi_h__
#define __s25fs_spi_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_iotypes.h>
#include <stddef.h>
#include <drv/spi_bb.h>
#include <sys/jiffies.h>

/*
 * =================== User Defines =====================
 */

/*!
 * Flash models by Spanion
 */
// #define FL_D
// #define FL_A
// #define FL_P_SINGLE_BIT_IO
// #define FL_P_MULTI_BIT_IO
// #define FL_R
// #define FL_T
// #define FL_127S
// #define FL_S
#define FS_S
// #define FL_K
// #define FL_2K
// #define FL_1K

#define S25FS_TIMEOUT      (1000)      // 1000 msec


/*
 * =================== General Defines =====================
 */

/*!
 * SPI Flash Commands info
 */

#define S25FS_SPI_WRSR_CMD                (0x01)
#define S25FS_SPI_WRR_CMD                 (0x01)
#define S25FS_SPI_PP_CMD                  (0x02)
#define S25FS_SPI_READ_CMD                (0x03)
#define S25FS_SPI_WRDI_CMD                (0x04)
  #define S25FS_SPI_RDSR_CMD                (0x05)   // used
  #define S25FS_SPI_WREN_CMD                (0x06)
#if (defined FL_1K || defined FL_K)
#define S25FS_SPI_RDSR2_CMD               (0x35)  // Read Status Register-2
#else
  #define S25FS_SPI_RDSR2_CMD               (0x07)  // Read Status Register-2
#endif
#define S25FS_SPI_FAST_READ_CMD           (0x0B)
#define S25FS_SPI_FAST_READ_4B_CMD        (0x0C)
#define S25FS_SPI_FAST_READ_DDR_CMD       (0x0D)
#define S25FS_SPI_FAST_READ_DDR_4B_CMD    (0x0E)
  #define S25FS_SPI_PP_4B_CMD               (0x12)
  #define S25FS_SPI_READ_4B_CMD             (0x13)   //used
#define S25FS_SPI_ABRD_CMD                (0x14)  // AutoBoot Register Read
#define S25FS_SPI_ABWR_CMD                (0x15)  // AutoBoot Register Write
#define S25FS_SPI_BRRD_CMD                (0x16)  // Bank Register Read
#define S25FS_SPI_BRWR_CMD                (0x17)  // Bank Register Write
#define S25FS_SPI_ECCRD_CMD               (0x18)  // ECC Read
#define S25FS_SPI_P4E_CMD                 (0x20)
#define S25FS_SPI_P4E4_CMD                (0x21)  // Parameter 4K-sector Erase (4Byte Addr)
#define S25FS_SPI_RASP_CMD                (0x2B)
#define S25FS_SPI_WASP_CMD                (0x2F)
#if (defined FS_S)
#define S25FS_SPI_CLSR_CMD                (0x82)
#else
#define S25FS_SPI_CLSR_CMD                (0x30)
#endif
#define S25FS_SPI_QPP_CMD                 (0x32)
#define S25FS_SPI_RDSR3_CMD               (0x33)  // Read Status Register-3
#define S25FS_SPI_QPP_4B_CMD              (0x34)
#define S25FS_SPI_RCR_CMD                 (0x35)
#define S25FS_SPI_QPP2_CMD                (0x38)  // Quad Page Program (3Byte Addr)
#define S25FS_SPI_DUALIO_RD_CMD           (0x3B)
#define S25FS_SPI_DUALIO_RD_4B_CMD        (0x3C)
#define S25FS_SPI_P8E_CMD                 (0x40)
#define S25FS_SPI_DLPRD_CMD               (0x41)  // Read Data Learning Pattern
#define S25FS_SPI_OTPP_CMD                (0x42)
#define S25FS_SPI_PROGRAM_SECURITY_CMD    (0x42)  // Program Security Register
#define S25FS_SPI_PNVDLR_CMD              (0x43)  // Program NV Data Learning Register
#define S25FS_SPI_ERASE_SECURITY_CMD      (0x44)  // Erase Security Register
#define S25FS_SPI_READ_SECURITY_CMD       (0x48)  // Read Security Register
#define S25FS_SPI_WVDLR_CMD               (0x4A)  // Write Volatile Data Learning Register
#define S25FS_SPI_OTPR_CMD                (0x4B)
#define S25FS_SPI_READ_UNIQUE_ID_CMD      (0x4B)  // Read Unique ID Number
#define S25FS_SPI_P8E_4B_CMD              (0x4C)
#define S25FS_SPI_WRITE_VOLATILE_CMD      (0x50)  // Write Enable for Volatile Status Register
#define S25FS_SPI_BE32KB_CMD              (0x52)  // Block Erase 32KB
#define S25FS_SPI_READ_SFDP_CMD           (0x5A)  // Read Serial Flash Discoverable Parameter Register
#define S25FS_SPI_BE1_CMD                 (0x60)  // Bulk Erase
  #define S25FS_SPI_RDAR_CMD                (0x65)  // Read Any Register
#define S25FS_SPI_RSTEN_CMD               (0x66)  // Software Reset Enable
#define S25FS_SPI_QUADIO_RD_CMD           (0x6B)
#define S25FS_SPI_QUADIO_RD_4B_CMD        (0x6C)
#define S25FS_SPI_WRAR_CMD                (0x71)  // Write Any Register
#define S25FS_SPI_ERS_SSP_CMD             (0x75)  // Erase / Program Suspend
#define S25FS_SPI_SETBURSTWRAP_CMD        (0x77)  // Set Burst with Wrap
#define S25FS_SPI_ERS_RES_CMD             (0x7A)  // Erase / Program Resume
#define S25FS_SPI_CLSR2_CMD               (0x82)  //Clear Status Register 1 (alternate instruction) - Erase/Prog. Fail Reset
#define S25FS_SPI_PGSP_CMD                (0x85)  // Program Suspend
#define S25FS_SPI_PGRS_CMD                (0x8A)  // Program Resume
#define S25FS_SPI_READID_90_CMD           (0x90)
#define S25FS_SPI_READID_DUAL_CMD         (0x92)  // Read Device ID by Dual
#define S25FS_SPI_READID_QUAD_CMD         (0x94)  // Read Device ID by Quad
#define S25FS_SPI_RESET_CMD               (0x99)  // Software Reset
#define S25FS_SPI_RDID_9F_CMD             (0x9F)
#define S25FS_SPI_READ_ID_9F_CMD          (0x9F)
#define S25FS_SPI_MPM_CMD                 (0xA3)
#define S25FS_SPI_PLBWR_CMD               (0xA6)  // PPB Lock Bit Write
#define S25FS_SPI_PLBRD_CMD               (0xA7)  // PPB Lock Bit Read
#define S25FS_SPI_READ_ID_AB_CMD          (0xAB)
#define S25FS_SPI_RDID_AB_CMD             (0xAB)
#define S25FS_SPI_RES_CMD                 (0xAB)
#define S25FS_SPI_RDQID_CMD               (0xAF)  //Read Quad ID
#define S25FS_SPI_EPS_CMD                 (0xB0)  //Erase / Program Suspend (alternate instruction)
#ifdef FS_S
#define S25FS_SPI_EPR_CMD                 (0x30)  //Erase / Program resume (alternate instruction)
#endif
#define S25FS_SPI_4BAM_CMD                (0xB7)  //Enter 4-byte Address Mode
#define S25FS_SPI_SP_CMD                  (0xB9)
#define S25FS_SPI_DP_CMD                  (0xB9)
#define S25FS_SPI_DUALIO_HPRD_CMD         (0xBB)
#define S25FS_SPI_DUALIO_HPRD_4B_CMD      (0xBC)
#define S25FS_SPI_DDR_DUALIO_HPRD_CMD     (0xBD)
#define S25FS_SPI_DDR_DUALIO_HPRD_4B_CMD  (0xBE)
#define S25FS_SPI_SBL_CMD                 (0xC0)  // Set Burst Length
#define S25FS_SPI_BE_CMD                  (0xC7)  // BULK ERASE
#define S25FS_SPI_EES_CMD                 (0xD0)  //Evaluate Erase Status
#define S25FS_SPI_BLOCK_ERASE_CMD         (0xD8)  // BLOCK ERASE
#if (defined FL_1K || defined FL_2K)
#define S25FS_SPI_SE_CMD                  S25FS_SPI_P4E_CMD
#else
#define S25FS_SPI_SE_CMD                  (0xD8)
#endif

#define S25FS_SPI_SE_4B_CMD               (0xDC)
#define S25FS_SPI_DYB_RD_CMD              (0xE0)  // DYB Read  -32bit address
#define S25FS_SPI_DYB_PG_CMD              (0xE1)  // DYB Write -32bit address
#define S25FS_SPI_PPB_RD_CMD              (0xE2)  // PPB Read  -32bit address
#define S25FS_SPI_PPB_PG_CMD              (0xE3)  // PPB Write -32bit address
#define S25FS_SPI_OCTALWORDREADQUAD_CMD   (0xE3)  // Octal Word Read Quad
#define S25FS_SPI_PPB_ERS_CMD             (0xE4)  // PPB Erase
#define S25FS_SPI_PASSRD_CMD              (0xE7)  // Password Read
#define S25FS_SPI_RPWD_CMD                (0xE7)
#define S25FS_SPI_WORDREADQUAD_CMD        (0xE7)  // Word Read Quad
#define S25FS_SPI_PASSP_CMD               (0xE8)  // Password Program
#define S25FS_SPI_WPWD_CMD                (0xE8)
#define S25FS_SPI_PASSU_CMD               (0xE9)  // Password Unlock
#define S25FS_SPI_QUADIO_HPRD_CMD         (0xEB)
#define S25FS_SPI_QUADIO_HPRD_4B_CMD      (0xEC)
#define S25FS_SPI_DDR_QUADIO_HPRD_CMD     (0xED)
#define S25FS_SPI_DDR_QUADIO_HPRD_4B_CMD  (0xEE)
#define S25FS_SPI_SOFTWARE_RESET          (0xF0)
#define S25FS_SPI_DYBRD_CMD               (0xFA)  //DYB Read   -24bit or 32bit address
#define S25FS_SPI_DYBWR_CMD               (0xFB)  //DYB Write  -24bit or 32bit address
#define S25FS_SPI_PPBRD_CMD               (0xFC)  //PPB Read   -24bit or 32bit address
#define S25FS_SPI_PPBP_CMD                (0xFD)   //PPB Write -24bit or 32bit address
#define S25FS_SPI_RMB_CMD                 (0xFF)
#define S25FS_SPI_READMODE_RESET_CMD      (0xFF)  // Continuous Read Mode Reset

#define S25FS_SPI_WRB_PP_CMD              (0x00) // Reserved: Write buffer autodetect programming
#define S25FS_SPI_RCVR_CMD                (0x00) // Reserved: Initiate recovery mode (manually refreshing ECC)
#define S25FS_SPI_RCSP_CMD                (0x00) // Reserved: Recovery suspend
#define S25FS_SPI_RCRS_CMD                (0x00) // Reserved: Recovery resume

/*!
 * data mask
 */
#define B0_MASK                  (0x01)
#define B1_MASK                  (0x02)
#define B2_MASK                  (0x04)
#define B3_MASK                  (0x08)
#define B4_MASK                  (0x10)
#define B5_MASK                  (0x20)
#define B6_MASK                  (0x40)
#define B7_MASK                  (0x80)

#define LLD_DEV_READ_MASK        (0xFF)
#define BLOCK_PROTECT_BITS_MASK  (0x1C)
#define LLD_BYTES_PER_OP         (0x01)

/*!
 * RDAR read any register address definition
 */
// Non-Volatile Status and Configuration Registers
#define    SR1NV         0x00000000
#define    CR1NV         0x00000002
#define    CR2NV         0x00000003
#define    CR3NV         0x00000004
#define    CR4NV         0x00000005
// Non-Volatile Data Learning Register
#define    NVDLR         0x00000010
#define    PASS7_0       0x00000020
//Non-Volatile Password Register
#define    PASS15_8      0x00000021
#define    PASS23_160    0x00000022
#define    PASS31_24     0x00000023
#define    PASS39_32     0x00000024
#define    PASS47_40     0x00000025
#define    PASS55_48     0x00000026
#define    PASS63_56     0x00000027
#define    ASPR7_0       0x00000030
#define    ASPR15_8      0x00000031
#define    SR1V          0x00800000
//Volatile Status and Configuration Registers
#define    SR2V          0x00800001
#define    CR1V          0x00800002
#define    CR2V          0x00800003
#define    CR3V          0x00800004
#define    CR4V          0x00800005
// Volatile Data Learning Register
#define    VDLR          0x00800010
//Volatile PPB Lock Register
#define    PPBL          0x00800040

/*!
 * SPI.read and SPI.write functions needed defines
 */
#define ADDRESS_NOT_USED 0xFFFFFFFF
#define BUFFER_NOT_USED  (byte_t*)0

#ifndef ENABLE
#define ENABLE    (1)
#endif
#ifndef DISABLE
#define DISABLE   (0)
#endif

/*
 * =================== Data types =====================
 */

/*!
 * LLD System Specific Typedefs
 */
typedef uint32_t     s25fs_idx_t ;        /*!< Used for system level addressing */
typedef byte_t       s25fs_data_t;        /*!< Used for system data addressing */

/*!
 * SLLD Internal Data Types
 */
typedef uint32_t     bytecount_t;            /*!< used for multi-byte operations */


/*!
 * Flash Software protect status
 */
typedef enum {
    S25FS_UNPROTECTED = 0,
    S25FS_PROTECTED
}s25fs_sp_status_en;

/*!
 * Flash embedded operation status
 */
typedef enum {
   S25FS_STATUS_UNKNOWN = 0,
   S25FS_NOT_BUSY,
   S25FS_PROGRAM_ERROR,
   S25FS_ERASE_ERROR,
   S25FS_SUSPEND,
   S25FS_BUSY
}s25fs_devstatus_en;


typedef drv_status_en (*spi_ioctl_t) (void *, ioctl_cmd_t, ioctl_buf_t *);
typedef drv_status_en (*spi_read_t)  (void *, byte_t *, int);
typedef drv_status_en (*spi_write_t) (void *, byte_t *, int);

typedef enum {
   S25FS_4B_ADDR_BAR = 0,
   S25FS_4B_ADDR_CMDS
}s25fs_add_mode_en;

typedef uint8_t   s25fs_cmd_t;

typedef volatile struct {
   drv_pinin_ft   wp;            /*!< Write protect pin */
   drv_pinout_ft  cs;            /*!< Chip Select pin */
   void*          spi;           /*!< void SPI type structure */
   spi_ioctl_t    spi_ioctl;     /*!< SPI ioctl function */
   spi_read_t     spi_read;      /*!< SPI read/write function */
   spi_write_t    spi_write;     /*!< SPI read/write function */
}s25fs_io_t;

typedef volatile struct {
   s25fs_io_t           s25fs_io;
   int                  write_page_sz;
   int                  erase_page_sz;
   drv_status_en        status;     /*!< Flash status */
}s25fs_t;


/*
 *  ============= PUBLIC S25FS API =============
 */

/*
 * Link and Glue functions
 */
void s25fs_link_wp (s25fs_t *drv, drv_pinin_ft fun);
void s25fs_link_cs (s25fs_t *drv, drv_pinout_ft fun);
void s25fs_link_spi_ioctl (s25fs_t *drv, spi_ioctl_t fun);
void s25fs_link_spi_read (s25fs_t *drv, spi_read_t fun);
void s25fs_link_spi_write (s25fs_t *drv, spi_write_t fun);
void s25fs_link_spi (s25fs_t *drv, void* spi);

/*
 * Set functions
 */
void s25fs_set_address_mode (s25fs_t *drv, s25fs_add_mode_en mode);

/*
 * User Functions
 */
void s25fs_deinit (s25fs_t *drv);                  /*!< for compatibility */
drv_status_en s25fs_init (s25fs_t *drv);           /*!< for compatibility */

drv_status_en  s25fs_read_buffer (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, size_t count);
drv_status_en s25fs_write_buffer (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, size_t count);
drv_status_en        s25fs_ioctl (s25fs_t *drv, ioctl_cmd_t ctrl, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __s25fs_spi_h__

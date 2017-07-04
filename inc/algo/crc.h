/*!
 * \file crc.h
 * \brief
 *    A target independent CRC generator implementations
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2016 Choutouridis Christos (http://www.houtouridis.net)
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
#ifndef __crc_h__
#define __crc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <toolbox_defs.h>

/*
 * Polynomials for CRC8
 */
#define CRC8_DVB_S2        (0xD5)
#define CRC8_DVB_S2_rev    (0xAB)
#define CRC8_AUTOSAR       (0x2F)   /*!< automotive integration, OpenSafety */
#define CRC8_AUTOSAR_rev   (0xF4)   /*!< automotive integration, OpenSafety for reverse bit order */
#define CRC8_Bluetooth     (0xA7)   /*!< Bluetooth wireless connectivity */
#define CRC8_Bluetooth_rev (0xE5)   /*!< Bluetooth wireless connectivity for reverse bit order */
#define CRC8_CCITT         (0x07)   /*!< I.432.1; ATM HEC, ISDN HEC and cell delineation */
#define CRC8_CCITT_rev     (0xE0)   /*!< I.432.1; ATM HEC, ISDN HEC and cell delineation for reverse bit order */
#define CRC8_Maxim         (0x31)   /*!< 1-Wire bus */
#define CRC8_Maxim_rev     (0x8C)   /*!< 1-Wire bus for reverse bit order */
#define CRC8_DARC          (0x31)   /*!< Data Radio Channel */
#define CRC8_DARC_rev      (0x31)   /*!< Data Radio Channel for reverse bit order */
#define CRC8_GSM_B         (0x49)   /*!< mobile networks */
#define CRC8_GSM_B_rev     (0x92)   /*!< mobile networks for reverse bit order */
#define CRC8_SAE_J1850     (0x1D)   /*!< AES3 */
#define CRC8_SAE_J1850_rev (0x1D)   /*!< AES3 for reverse bit order */
#define CRC8_WCDMA         (0x9B)   /*!< mobile networks */
#define CRC8_WCDMA_rev     (0xD9)   /*!< mobile networks for reverse bit order */

/*
 * Polynomials for CRC16
 */
#define CRC16_Chakravarty     (0x2F15)    /*!< Optimal for payloads <= 64 bits */
#define CRC16_Chakravarty_rev (0xA8F4)    /*!< Optimal for payloads <= 64 bits for reverse bit order */
#define CRC16_ARINC           (0xA02B)    /*!< ACARS applications */
#define CRC16_ARINC_rev       (0xD405)    /*!< ACARS applications for reverse bit order*/
#define CRC16_CCITT           (0x1021)    /*!< X.25, V.41, HDLC FCS, XMODEM, Bluetooth, PACTOR, SD, DigRF, many others */
#define CRC16_CCITT_rev       (0x8408)    /*!< X.25, V.41, HDLC FCS, XMODEM, Bluetooth, PACTOR, SD, DigRF, many others, for reverse bit order */
#define CRC16_CDMA2000        (0xC867)    /*!< mobile networks */
#define CRC16_CDMA2000_rev    (0xE613)    /*!< mobile networks, for reverse bit order */
#define CRC16_DECT            (0x0589)    /*!< Cordeless Telephones */
#define CRC16_DECT_rev        (0x91A0)    /*!< Cordeless Telephones, for reverse bit order */
#define CRC16_T10_DIF         (0x8BB7)    /*!< SCSI DIF */
#define CRC16_T10_DIF_rev     (0xEDD1)    /*!< SCSI DIF, for reverse bit order */
#define CRC16_DNP             (0x3D65)    /*!< DNP, IEC 870, M-BUS, wM-BUS, ... */
#define CRC16_DNP_rev         (0xA6BC)    /*!< DNP, IEC 870, M-BUS, wM-BUS, for reverse bit order */
#define CRC16_IBM             (0x8005)    /*!< Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI */
#define CRC16_IBM_rev         (0xA001)    /*!< Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI, for reverse bit order */
#define CRC16_OpenSafety_A    (0x5935)    /*!< safety fieldbus */
#define CRC16_OpenSafety_A_rev (0xAC9A)   /*!< safety fieldbus, for reverse bit order */
#define CRC16_OpenSafety_B    (0x755B)    /*!< safety fieldbus */
#define CRC16_OpenSafety_B_rev (0xDAAE)   /*!< safety fieldbus, for reverse bit order */
#define CRC16_Profibus        (0x1DCF)    /*!< fieldbus networks */
#define CRC16_Profibus_rev    (0xF3B8)    /*!< fieldbus networks, for reverse bit order */

#define CRC16_ANSI            (CRC16_IBM)

/*!
 * Enumerator for Bit Order of the CRC operation
 */
typedef enum {
   CRC_MSB=0,     //!< BigEndian:      Most significant bit to Least significant bit
   CRC_LSB        //!< LittleEndian:   Least significant bit to Most significant bit
}CRC_BitOrder_en;


uint8_t CRC8_byte (uint8_t poly, CRC_BitOrder_en bo, uint8_t crc, byte_t b);
uint8_t CRC8_buffer (uint8_t poly, CRC_BitOrder_en bo, uint8_t crc, const byte_t *data, bytecount_t size);

uint16_t CRC16_byte (uint16_t poly, CRC_BitOrder_en bo, uint16_t crc, byte_t b);
uint16_t CRC16_buffer (uint16_t poly, CRC_BitOrder_en bo, uint16_t crc, const byte_t *data, bytecount_t size);




#ifdef __cplusplus
}
#endif

#endif /*#ifndef __crc_h__ */

/*!
 * \file crc.c
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
#include <algo/crc.h>

/*!
 * \brief
 *    Append CRC8 to an existing CRC value
 * \param   poly  The 8bit wide polynomial to use
 *    \arg     CRC8_DVB_S2        (0xD5)
 *    \arg     CRC8_DVB_S2_rev    (0xAB)
 *    \arg     CRC8_AUTOSAR       (0x2F)    automotive integration, OpenSafety
 *    \arg     CRC8_AUTOSAR_rev   (0xF4)    automotive integration, OpenSafety for reverse bit order
 *    \arg     CRC8_Bluetooth     (0xA7)    Bluetooth wireless connectivity
 *    \arg     CRC8_Bluetooth_rev (0xE5)    Bluetooth wireless connectivity for reverse bit order
 *    \arg     CRC8_CCITT         (0x07)    I.432.1; ATM HEC, ISDN HEC and cell delineation
 *    \arg     CRC8_CCITT_rev     (0xE0)    I.432.1; ATM HEC, ISDN HEC and cell delineation for reverse bit order
 *    \arg     CRC8_Maxim         (0x31)    1-Wire bus
 *    \arg     CRC8_Maxim_rev     (0x8C)    1-Wire bus for reverse bit order
 *    \arg     CRC8_DARC          (0x31)    Data Radio Channel
 *    \arg     CRC8_DARC_rev      (0x31)    Data Radio Channel for reverse bit order
 *    \arg     CRC8_GSM_B         (0x49)    mobile networks
 *    \arg     CRC8_GSM_B_rev     (0x92)    mobile networks for reverse bit order
 *    \arg     CRC8_SAE_J1850     (0x1D)    AES3
 *    \arg     CRC8_SAE_J1850_rev (0x1D)    AES3 for reverse bit order
 *    \arg     CRC8_WCDMA         (0x9B)    mobile networks
 *    \arg     CRC8_WCDMA_rev     (0xD9)    mobile networks for reverse bit order
 *    OR: Any other 8bit wide polynomial
 * \param   bo    The CRC bit order of the operation
 *    \arg     CRC_BigEndian     The "usual" bit order of the operation
 *    \arg     CRC_LittleEndian  The invert LSB->MSB order of the operation, used in 1-wire of example
 * \param   crc   The current CRC value in witch to append the calculated the new CRC
 * \param   b     The byte to check
 * \return  The new CRC value
 */
uint8_t CRC8_byte (uint8_t poly, CRC_BitOrder_en bo, uint8_t crc, byte_t b)
{
   uint8_t i;

   switch (bo) {
      default:
      case CRC_MSB:
         for (i=0; i<8; ++i) {
            if ((crc & 0x80) ^ (b & 0x80))
               crc = (crc << 1) ^ poly;
            else
               crc <<= 1;
            b <<= 1;
         }
         break;
      case CRC_LSB:
         for (i=0; i<8; ++i) {
            if ((crc & 0x01) ^ (b & 0x01))
               crc = (crc >> 1) ^ poly;
            else
               crc >>= 1;
            b >>= 1;
         }
         break;
   }
   return crc;
}

/*!
 * \brief
 *    Calculate the CRC8 code of a buffer
 * \param   poly  The polynomial to use
 *    \arg     CRC8_DVB_S2        (0xD5)
 *    \arg     CRC8_DVB_S2_rev    (0xAB)
 *    \arg     CRC8_AUTOSAR       (0x2F)    automotive integration, OpenSafety
 *    \arg     CRC8_AUTOSAR_rev   (0xF4)    automotive integration, OpenSafety for reverse bit order
 *    \arg     CRC8_Bluetooth     (0xA7)    Bluetooth wireless connectivity
 *    \arg     CRC8_Bluetooth_rev (0xE5)    Bluetooth wireless connectivity for reverse bit order
 *    \arg     CRC8_CCITT         (0x07)    I.432.1; ATM HEC, ISDN HEC and cell delineation
 *    \arg     CRC8_CCITT_rev     (0xE0)    I.432.1; ATM HEC, ISDN HEC and cell delineation for reverse bit order
 *    \arg     CRC8_Maxim         (0x31)    1-Wire bus
 *    \arg     CRC8_Maxim_rev     (0x8C)    1-Wire bus for reverse bit order
 *    \arg     CRC8_DARC          (0x31)    Data Radio Channel
 *    \arg     CRC8_DARC_rev      (0x31)    Data Radio Channel for reverse bit order
 *    \arg     CRC8_GSM_B         (0x49)    mobile networks
 *    \arg     CRC8_GSM_B_rev     (0x92)    mobile networks for reverse bit order
 *    \arg     CRC8_SAE_J1850     (0x1D)    AES3
 *    \arg     CRC8_SAE_J1850_rev (0x1D)    AES3 for reverse bit order
 *    \arg     CRC8_WCDMA         (0x9B)    mobile networks
 *    \arg     CRC8_WCDMA_rev     (0xD9)    mobile networks for reverse bit order
 *    OR: Any other 8bit wide polynomial
 * \param   bo    The CRC bit order of the operation
 *    \arg     CRC_BigEndian     The "usual" bit order of the operation
 *    \arg     CRC_LittleEndian  The invert LSB->MSB order of the operation
 * \param   crc      The current CRC value in witch to append the calculated the new CRC
 * \param   data     Pointer to data buffer
 * \param   size     The size of the data buffer
 * \return  The CRC8 value
 */
uint8_t CRC8_buffer (uint8_t poly, CRC_BitOrder_en bo, uint8_t crc, const byte_t *data, bytecount_t size)
{
   uint16_t i;

   // Data check
   if(data == 0)  return crc;

   for (i=0 ; i<size ; ++i)
      crc = CRC8_byte (poly, bo, crc, data [i]);
   return crc;
}


/*!
 * \brief
 *    Append CRC16 to an existing CRC value
 * \param   poly  The 16bit wide polynomial to use
 *    \arg  CRC16_Chakravarty     (0x2F15)     Optimal for payloads <= 64 bits
 *    \arg  CRC16_Chakravarty_rev (0xA8F4)     Optimal for payloads <= 64 bits for reverse bit order
 *    \arg  CRC16_ARINC           (0xA02B)     ACARS applications
 *    \arg  CRC16_ARINC_rev       (0xD405)     ACARS applications for reverse bit orde
 *    \arg  CRC16_CCITT           (0x1021)     X.25, V.41, HDLC FCS, XMODEM, Bluetooth, PACTOR, SD, DigRF, many others
 *    \arg  CRC16_CCITT_rev       (0x8408)     X.25, V.41, HDLC FCS, XMODEM, Bluetooth, PACTOR, SD, DigRF, many others, for reverse bit order
 *    \arg  CRC16_CDMA2000        (0xC867)     mobile networks
 *    \arg  CRC16_CDMA2000_rev    (0xE613)     mobile networks, for reverse bit order
 *    \arg  CRC16_DECT            (0x0589)     Cordeless Telephones
 *    \arg  CRC16_DECT_rev        (0x91A0)     Cordeless Telephones, for reverse bit order
 *    \arg  CRC16_T10_DIF         (0x8BB7)     SCSI DIF
 *    \arg  CRC16_T10_DIF_rev     (0xEDD1)     SCSI DIF, for reverse bit order
 *    \arg  CRC16_DNP             (0x3D65)     DNP, IEC 870, M-BUS, wM-BUS, ...
 *    \arg  CRC16_DNP_rev         (0xA6BC)     DNP, IEC 870, M-BUS, wM-BUS, for reverse bit order
 *    \arg  CRC16_IBM             (0x8005)     Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI
 *    \arg  CRC16_IBM_rev         (0xA001)     Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI, for reverse bit order
 *    \arg  CRC16_OpenSafety_A    (0x5935)     safety fieldbus
 *    \arg  CRC16_OpenSafety_A_rev (0xAC9A)    safety fieldbus, for reverse bit order
 *    \arg  CRC16_OpenSafety_B    (0x755B)     safety fieldbus
 *    \arg  CRC16_OpenSafety_B_rev (0xDAAE)    safety fieldbus, for reverse bit order
 *    \arg  CRC16_Profibus        (0x1DCF)     fieldbus networks
 *    \arg  CRC16_Profibus_rev    (0xF3B8)     fieldbus networks, for reverse bit order
 *    \arg  Any other 16bit wide polynomial
 * \param   bo    The CRC bit order of the operation
 *    \arg     CRC_BigEndian     The "usual" bit order of the operation
 *    \arg     CRC_LittleEndian  The invert LSB->MSB order of the operation, used in 1-wire of example
 * \param   crc   The current CRC value in witch to append the calculated the new CRC
 * \param   b     The byte to check
 * \return  The new CRC value
 */
uint16_t CRC16_byte (uint16_t poly, CRC_BitOrder_en bo, uint16_t crc, byte_t b)
{
   uint8_t i;

   switch (bo) {
      default:
      case CRC_MSB:
         for (i=0; i<8; ++i) {
            if (((crc & 0x8000) >> 8) ^ (b & 0x80))
               crc = (crc << 1) ^ poly;
            else
               crc <<= 1;
            b <<= 1;
         }
         break;
      case CRC_LSB:
         for (i=0; i<8; ++i) {
            if ((crc & 0x0001) ^ (b & 0x01))
               crc = (crc >> 1) ^ poly;
            else
               crc >>= 1;
            b >>= 1;
         }
         break;
   }
   return crc;
}

/*!
 * \brief
 *    Calculate the CRC16 code of a buffer
 * \param   poly  The polynomial to use
 *    \arg  CRC16_Chakravarty     (0x2F15)     Optimal for payloads <= 64 bits
 *    \arg  CRC16_Chakravarty_rev (0xA8F4)     Optimal for payloads <= 64 bits for reverse bit order
 *    \arg  CRC16_ARINC           (0xA02B)     ACARS applications
 *    \arg  CRC16_ARINC_rev       (0xD405)     ACARS applications for reverse bit orde
 *    \arg  CRC16_CCITT           (0x1021)     X.25, V.41, HDLC FCS, XMODEM, Bluetooth, PACTOR, SD, DigRF, many others
 *    \arg  CRC16_CCITT_rev       (0x8408)     X.25, V.41, HDLC FCS, XMODEM, Bluetooth, PACTOR, SD, DigRF, many others, for reverse bit order
 *    \arg  CRC16_CDMA2000        (0xC867)     mobile networks
 *    \arg  CRC16_CDMA2000_rev    (0xE613)     mobile networks, for reverse bit order
 *    \arg  CRC16_DECT            (0x0589)     Cordeless Telephones
 *    \arg  CRC16_DECT_rev        (0x91A0)     Cordeless Telephones, for reverse bit order
 *    \arg  CRC16_T10_DIF         (0x8BB7)     SCSI DIF
 *    \arg  CRC16_T10_DIF_rev     (0xEDD1)     SCSI DIF, for reverse bit order
 *    \arg  CRC16_DNP             (0x3D65)     DNP, IEC 870, M-BUS, wM-BUS, ...
 *    \arg  CRC16_DNP_rev         (0xA6BC)     DNP, IEC 870, M-BUS, wM-BUS, for reverse bit order
 *    \arg  CRC16_IBM             (0x8005)     Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI
 *    \arg  CRC16_IBM_rev         (0xA001)     Bisync, Modbus, USB, ANSI X3.28, SIA DC-07, many others; also known as CRC-16 and CRC-16-ANSI, for reverse bit order
 *    \arg  CRC16_ANSI            (0x8005)
 *    \arg  CRC16_OpenSafety_A    (0x5935)     safety fieldbus
 *    \arg  CRC16_OpenSafety_A_rev (0xAC9A)    safety fieldbus, for reverse bit order
 *    \arg  CRC16_OpenSafety_B    (0x755B)     safety fieldbus
 *    \arg  CRC16_OpenSafety_B_rev (0xDAAE)    safety fieldbus, for reverse bit order
 *    \arg  CRC16_Profibus        (0x1DCF)     fieldbus networks
 *    \arg  CRC16_Profibus_rev    (0xF3B8)     fieldbus networks, for reverse bit order
 *    \arg  Any other 16bit wide polynomial
 * \param   bo    The CRC bit order of the operation
 *    \arg  CRC_BigEndian     The "usual" bit order of the operation
 *    \arg  CRC_LittleEndian  The invert LSB->MSB order of the operation, used in 1-wire of example
 * \param   crc      The current CRC value in witch to append the calculated the new CRC
 * \param   data     Pointer to data buffer
 * \param   size     The size of the data buffer
 * \return  The CRC16 value
 */
uint16_t CRC16_buffer (uint16_t poly, CRC_BitOrder_en bo, uint16_t crc, const byte_t *data, bytecount_t size)
{
   uint16_t i;

   // Data check
   if(data == 0)  return crc;

   for (i=0 ; i<size ; ++i)
      crc = CRC16_byte (poly, bo, crc, data [i]);
   return crc;
}



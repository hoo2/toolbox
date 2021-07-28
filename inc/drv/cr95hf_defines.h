/*!
 * \file cr95hf_defines.h
 * \brief
 *    Configuration number define header. This header is NOT publicly included by toolbox.h
 *    Its included "locally" only by cr95hf_xxx.c files.
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2021 Christos Choutouridis (http://www.houtouridis.net)
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
#ifndef __cr95hf_defines_h__
#define __cr95hf_defines_h__

#ifdef __cplusplus
extern "C" {
#endif

/* CR95HF command definition */
#define IDN                               0x01
#define PROTOCOL_SELECT                   0x02
#define POLL_FIELD                        0x03
#define SEND_RECEIVE                      0x04
#define LISTEN                            0x05
#define SEND                              0x06
#define IDLE                              0x07
#define READ_REGISTER                     0x08
#define WRITE_REGISTER                    0x09
#define BAUD_RATE                         0x0A
#define SUB_FREQ_RES                      0x0B
#define AC_FILTER                         0x0D
#define TEST_MODE                         0x0E
#define SLEEP_MODE                        0x0F
#define ECHO                              0x55
#define ECHORESPONSE                      0x55

#define SUCCESFUL_SENDS                   0x80
#define SUCCESFUL_COMMAND                 0x00


/*
 * Protocol Select parameters
 *
 * format: [PROTOCOL_CODE_xxx, PS_xxx_PARAM0, PS_xxx_PARAM1, PS_xxx_PARAM2]
 * PS_PARAM1,2 are optional
 */
// Parameters[0] ~ code 1st byte parameter for Protocol Select command
#define PROTOCOL_CODE_TAG_FIELDOFF        0x00
#define PROTOCOL_CODE_TAG_ISO15693        0x01
#define PROTOCOL_CODE_TAG_ISO14443A       0x02
#define PROTOCOL_CODE_TAG_ISO14443B       0x03
#define PROTOCOL_CODE_TAG_FELICA          0x04
#define PROTOCOL_CODE_CARD_ISO14443A      0x12
#define PROTOCOL_CODE_CARD_ISO14443B      0x13
#define PROTOCOL_CODE_CARD_FELICA         0x14

// ISO15693
// Parameters[0] ~ 2nd byte parameters for ISO15693
// bits 5:4
#define PS_ISO15693_PARAM0_RATE_26K       0x00
#define PS_ISO15693_PARAM0_RATE_52K       0x10
#define PS_ISO15693_PARAM0_RATE_6K        0x20
#define PS_ISO15693_PARAM0_RATE_RFU       0x30
// bit 3
#define PS_ISO15693_PARAM0_DELAY312U      0x00
#define PS_ISO15693_PARAM0_WAIT_SOF       0x08
// bit 2
#define PS_ISO15693_PARAM0_MODULATION_100 0x00
#define PS_ISO15693_PARAM0_MODULATION_10  0x04
// bit 1
#define PS_ISO15693_PARAM0_SINGLE_SUBCAR  0x00
#define PS_ISO15693_PARAM0_DUAL_SUBCAR    0x02
// bit 0
#define PS_ISO15693_PARAM0_APPEND_CRC     0x01


//ISO14443A (Forum 1, Forum 2, Forum 4A)
// Parameters[0] ~ 2nd byte parameters for ISO14443A
// bits 7:6
#define PS_ISO14443A_PARAM0_TXRATE_106K   0x00
#define PS_ISO14443A_PARAM0_TXRATE_212K   0x40
#define PS_ISO14443A_PARAM0_TXRATE_424K   0x80
#define PS_ISO14443A_PARAM0_TXRATE_RFU    0xC0
// bits 5:4
#define PS_ISO14443A_PARAM0_RXRATE_106K   0x00
#define PS_ISO14443A_PARAM0_RXRATE_212K   0x10
#define PS_ISO14443A_PARAM0_RXRATE_424K   0x20
#define PS_ISO14443A_PARAM0_RXRATE_RFU    0x30

// Parameters[1] optional ~ 3rd parameters for ISO14443A
#define PS_ISO14443A_PARAM1_PP            0x00
// Parameters[2] optional ~ 4th parameters for ISO14443A
#define PS_ISO14443A_PARAM2_MM            0x00

//ISO14443B (Forum 4B)
// Parameters[0] ~ 2nd byte parameters for ISO14443B
// bits 7:6
#define PS_ISO14443B_PARAM0_TXRATE_106K   0x00
#define PS_ISO14443B_PARAM0_TXRATE_212K   0x40
#define PS_ISO14443B_PARAM0_TXRATE_424K   0x80
#define PS_ISO14443B_PARAM0_TXRATE_RFU    0xC0
// bits 5:4
#define PS_ISO14443B_PARAM0_RXRATE_106K   0x00
#define PS_ISO14443B_PARAM0_RXRATE_212K   0x10
#define PS_ISO14443B_PARAM0_RXRATE_424K   0x20
#define PS_ISO14443B_PARAM0_RXRATE_RFU    0x30
// bit 0
#define PS_ISO14443B_PARAM0_APPEND_CRC    0x01

// Parameters[1] optional ~ 3rd byte parameters for ISO14443B
#define PS_ISO14443B_PARAM1_PP            0x00
// Parameters[2] optional ~ 4th byte parameters for ISO14443B
#define PS_ISO14443B_PARAM2_MM            0x00


//ISO18092 (Forum 3)
// Parameters[0] ~ 2nd byte parameters for ISO18092
// bits 7:6
#define PS_ISO18092_PARAM0_TXRATE_RFU0   0x00
#define PS_ISO18092_PARAM0_TXRATE_212K   0x40
#define PS_ISO18092_PARAM0_TXRATE_424K   0x80
#define PS_ISO18092_PARAM0_TXRATE_RFU1   0xC0
// bits 5:4
#define PS_ISO18092_PARAM0_RXRATE_RFU0   0x00
#define PS_ISO18092_PARAM0_RXRATE_212K   0x10
#define PS_ISO18092_PARAM0_RXRATE_424K   0x20
#define PS_ISO18092_PARAM0_RXRATE_RFU1   0x30
// bit 0
#define PS_ISO18092_PARAM0_APPEND_CRC    0x01

// Parameters[1] optional ~ 3rd byte parameters for ISO14443B
#define PS_ISO18092_PARAM1_PP            0x00
// Parameters[2] optional ~ 4th byte parameters for ISO14443B
#define PS_ISO18092_PARAM2_MM            0x00








#ifdef __cplusplus
}
#endif

#endif   /* #ifndef __cr95hf_defines_h__ */

/*
 * \file _vsxscanf.h
 * \brief
 *    A small footprint scanf with floating point support
 *
 * this file is part of toolbox (std part)
 *
 * Copyright (C) 2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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

#ifndef __vsxscanf_h__
#define __vsxscanf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <std/_base_io.h>

/*!
 * Callback read mode type.
 */
typedef enum
{
   _GETC_HEAD=0,     /*!< Return the head character and stay at head */
   _GETC_READ,       /*!< Return the head character and go to next one */
   _GETC_NEXT        /*!< Return the next character and stay to next one */
}_io_getc_read_en;

/*!
 * _number_copy number type
 */
typedef enum {
   _INT=0,           /*!< The number to copy from strwam is integer */
   _HEX,             /*!< The number to copy from stream is hex */
   _FLOAT            /*!< The number to copy from stream is real */
}_number_copy_type_en;

/*!
 * callback function pointer to use in:
 *
 *  1) user's __getchar ()
 *  2) source string for sscanf family
 *  3) source file for fscanf family
 */
typedef int (*_getc_in_t) (const char *, char **psrc, _io_getc_read_en);

int _getc_usr (const char *src, char **psrc, _io_getc_read_en mode);  /*!< back end for user's device stdin */
int _getc_src (const char *src, char **psrc, _io_getc_read_en mode);  /*!< back end for sscanf family */
int _getc_fil (const char *src, char **psrc, _io_getc_read_en mode);  /*!< back end for file fscanf family */

/*
 * ============================ Public Functions ============================
 */

int vsxscanf (_getc_in_t _in, const char *src, const char *frm, __VALIST ap);

/*!
 * Tailor this in order to connect scanf functionality
 * to your hardware (stdin).
 */
extern int __getchar (void);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __vsxprintf_h__

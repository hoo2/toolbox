/*
 * \file _vsxprintf.h
 * \brief
 *    A small footprint printf with floating point support
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
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:
 *
 *
 * \todo
 * 1. Implement the long long int type
 * 2. Implement the long double type
 * 3. Report the bug and fix the code
 *
 */

#ifndef __vsxprintf_h__
#define __vsxprintf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <std/_base_io.h>
#include <toolbox_defs.h>

/*!
 * callback function pointer to use in:
 *
 *  1) user's __putchar ()
 *  2) destination string for sprintf family
 *  3) file destination (fprintf family)
 */
typedef int (*_putc_out_t) (char *, const char );

int _putc_usr (char *dst, const char c);  /*!< back end for user's device stdout */
int _putc_dst (char *dst, const char c);  /*!< back end for sprintf family */
int _putc_fil (char *dst, const char c);  /*!< back end for file printf family */

/*
 * ============================ Public Functions ============================
 */
void _io_unused (void);

int vsxprintf(_putc_out_t _putc_out, char *dst, char *pfrm, __VALIST ap);

/*!
 * Tailor this in order to connect printf functionality
 * to your hardware (stdout).
 */
extern int __putchar(char c);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __vsxprintf_h__

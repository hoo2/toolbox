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
 * callback function pointer to use in:
 *
 *  1) user's __getchar ()
 *  2) destination string for sscanf family
 *  3) file destination (fscanf family)
 */
typedef int (*_getc_out_t) (char *, const char );

int _getc_usr (char *dst, const char c);  /*!< back end for user's device stdin */
int _getc_dst (char *dst, const char c);  /*!< back end for sscanf family */
int _getc_fil (char *dst, const char c);  /*!< back end for file fscanf family */

/*
 * ============================ Public Functions ============================
 */

int vsxscanf (_getc_out_t _getc_out, char *dst, const char *pfrm, va_list ap);

/*!
 * Tailor this in order to connect scanf functionality
 * to your hardware (stdin).
 */
extern int __getchar (void);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __vsxprintf_h__

/*
 * \file   w1602a_buydisplay_cyril.h
 * \brief  Buy Display W1602A character set defines (for Greeks over Cyrilic)
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
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

#ifndef __w1602a_buydisplay_cyril_h__
#define __w1602a_buydisplay_cyril_h__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Custom character maps
 */
//#define _make_c_D(_ch_map) \
//   uint8_t _ch_map[8] = {  \
//      0b00100,             \
//      0b01010,             \
//      0b01010,             \
//      0b10001,             \
//      0b10001,             \
//      0b10001,             \
//      0b11111,             \
//      0b00000              \
//   }

#define _make_c_TH(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b01110,             \
      0b10001,             \
      0b10001,             \
      0b11111,             \
      0b10001,             \
      0b10001,             \
      0b01110,             \
      0b00000              \
   }

#define _make_c_L(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b00100,             \
      0b01010,             \
      0b01010,             \
      0b10001,             \
      0b10001,             \
      0b10001,             \
      0b10001,             \
      0b00000              \
   }

#define _make_c_J(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b11111,             \
      0b00000,             \
      0b00000,             \
      0b01110,             \
      0b00000,             \
      0b00000,             \
      0b11111,             \
      0b00000              \
   }

#define _make_c_S(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b11111,             \
      0b01000,             \
      0b00100,             \
      0b00010,             \
      0b00100,             \
      0b01000,             \
      0b11111,             \
      0b00000              \
   }

#define _make_c_Y(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b10101,             \
      0b10101,             \
      0b10101,             \
      0b01110,             \
      0b00100,             \
      0b00100,             \
      0b01110,             \
      0b00000              \
   }

#define _make_c_W(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b01110,             \
      0b10001,             \
      0b10001,             \
      0b10001,             \
      0b01110,             \
      0b00000,             \
      0b11111,             \
      0b00000              \
   }

#define _make_c_pow3(_ch_map) \
   uint8_t _ch_map[8] = {  \
      0b11110,             \
      0b00010,             \
      0b11100,             \
      0b00010,             \
      0b11100,             \
      0b00000,             \
      0b00000,             \
      0b00000              \
   }

//#define  _c_D_       "\x00"
#define  _c_TH_      "\x01"
#define  _c_L_       "\x02"
#define  _c_J_       "\x03"
#define  _c_S_       "\x04"
#define  _c_Y_       "\x05"
#define  _c_W_       "\x06"
#define  _c_pow3_    "\x07"

/*
 * A "generic programming" function body part to load the custom characters
 */
#ifdef   ALCD_HAS_CUSTOM_CHARS
 #undef   ALCD_HAS_CUSTOM_CHARS
#endif
#define  ALCD_HAS_CUSTOM_CHARS     1

#define  _alcd_createCharacters_body(_alcd_)    \
   do {                                         \
      _make_c_TH(_c_TH_map);                    \
      _make_c_L(_c_L_map);                      \
      _make_c_J(_c_J_map);                      \
      _make_c_S(_c_S_map);                      \
      _make_c_Y(_c_Y_map);                      \
      _make_c_W(_c_W_map);                      \
      _make_c_pow3(_c_pow3_map);                \
      alcd_createChar(&_alcd, 1, _c_TH_map);    \
      alcd_createChar(&_alcd, 2, _c_L_map);     \
      alcd_createChar(&_alcd, 3, _c_J_map);     \
      alcd_createChar(&_alcd, 4, _c_S_map);     \
      alcd_createChar(&_alcd, 5, _c_Y_map);     \
      alcd_createChar(&_alcd, 6, _c_W_map);     \
      alcd_createChar(&_alcd, 7, _c_pow3_map);  \
   } while(0)

/*
 * Display's Character translation
 */
#define  _Deg_     " "        // Degrese
#define  _pow2_    " "        // ^2
#define  _pow3_    _c_pow3_   // ^3
#define  _A_       "A"
#define  _B_       "B"
#define  _G_       "\xA1"      // Gamma
#define  _D_       "D"         // Delta
#define  _E_       "E"
#define  _Z_       "Z"
#define  _H_       "H"
#define  _TH_      _c_TH_      // Theta
#define  _I_       "I"
#define  _K_       "K"
#define  _L_       _c_L_       // Lambda
#define  _M_       "M"
#define  _N_       "N"
#define  _J_       _c_J_       // Ksi
#define  _O_       "O"
#define  _P_       "\xA8"      // Pi
#define  _R_       "P"
#define  _S_       _c_S_       // Sigma
#define  _T_       "T"
#define  _U_       "Y"
#define  _F_       "\xAA"      // Phi
#define  _X_       "X"
#define  _Y_       _c_Y_       // Psi
#define  _W_       _c_W_       // Omega

#define  _a_       "A"        // alpha
#define  _b_       "B"        // beta
#define  _g_       _G_        // gamma
#define  _d_       _D_      // delta
#define  _e_       _E_      // epsilon
#define  _z_       _Z_      // zeta
#define  _h_       _H_      // heta
#define  _th_      _TH_      // theta
#define  _i_       _I_      // iota
#define  _k_       _K_      // kapa
#define  _l_       _L_      // lambda
#define  _m_       _M_      // mi
#define  _n_       _N_      // ni
#define  _j_       _J_      // ksi
#define  _o_       _O_         // omikron
#define  _p_       _P_      // pi
#define  _r_       _R_      // ro
#define  _s_       _S_      // sigma
#define  _t_       _T_      // taph
#define  _u_       _U_      // upsilon
#define  _f_       _F_      // phi
#define  _x_       _X_      // hi
#define  _y_       _Y_      // psi
#define  _w_       _W_      // omega
#define  _ss_      _S_      // sigma at end


#ifdef __cplusplus
}
#endif

#endif   // #ifndef __w1602a_buydisplay_cyril_h__

/*!
 * \file tuid.c
 * \brief
 *    A small tui library.
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2010-2014 Houtouridis Christos (http://www.houtouridis.net)
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
 *
 */
#include <ui/tuid.h>

/*
 * Link and Glue functions
 */
inline void tuid_link_framebuffer (tuid_t *tuid, uint8_t *fb) {
   tuid->frame_buffer.fb = fb;
}

/*
 * Set functions
 */
inline void tuid_set_fb_lines (tuid_t *tuid, int l) {
   tuid->frame_buffer.l = l;
}
void tuid_set_fb_columns (tuid_t *tuid, int c) {
   tuid->frame_buffer.c = c;
}

void    tuid_set_key_up (tuid_t *tuid, int k) {
   tuid->keys.UP = k;
}
void  tuid_set_key_down (tuid_t *tuid, int k) {
   tuid->keys.DOWN = k;
}
void  tuid_set_key_left (tuid_t *tuid, int k) {
   tuid->keys.LEFT = k;
}
void tuid_set_key_right (tuid_t *tuid, int k) {
   tuid->keys.RIGHT = k;
}
void tuid_set_key_enter (tuid_t *tuid, int k) {
   tuid->keys.ENTER = k;
}
void tuid_set_key_enter_l (tuid_t *tuid, int k) {
   tuid->keys.ENTER_L = k;
}
void   tuid_set_key_esc (tuid_t *tuid, int k)  {
   tuid->keys.ESC = k;
}

/*!
 * \file tui.c
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
#include <ui/tui.h>

/*
 * Link and Glue functions
 */
inline void tui_link_framebuffer (tui_t *tui, uint8_t *fb) {
   tui->frame_buffer.fb = fb;
}
inline void tui_link_get_key (tui_t *tui, tui_get_key_t fp) {
   tui->get_key = fp;
}

/*
 * Set functions
 */
inline void tui_set_fb_lines (tui_t *tui, int l) {
   tui->frame_buffer.l = l;
}
void tui_set_fb_columns (tui_t *tui, int c) {
   tui->frame_buffer.c = c;
}

void    tui_set_key_up (tui_t *tui, int k) {
   tui->keys.UP = k;
}
void  tui_set_key_down (tui_t *tui, int k) {
   tui->keys.DOWN = k;
}
void  tui_set_key_left (tui_t *tui, int k) {
   tui->keys.LEFT = k;
}
void tui_set_key_right (tui_t *tui, int k) {
   tui->keys.RIGHT = k;
}
void tui_set_key_enter (tui_t *tui, int k) {
   tui->keys.ENTER = k;
}
void tui_set_key_enter_l (tui_t *tui, int k) {
   tui->keys.ENTER_L = k;
}
void   tui_set_key_esc (tui_t *tui, int k)  {
   tui->keys.ESC = k;
}

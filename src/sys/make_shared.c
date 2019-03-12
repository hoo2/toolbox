/*
 * \file make_shared.c
 * \brief
 *    This file provides shared resources utility for init and de-init calling
 *
 * Copyright (C) 2013-2019 Choutouridis Christos <houtouridis.ch@gmail.com>
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
 * Author:     Choutouridis Christos <houtouridis.ch@gmail.com>
 * Date:       09/2013
 * Version:
 *
 */
#include <sys/make_shared.h>

void make_shared (shared_resource_t* self, mksh_aloc_ft aloc, mksh_dealoc_ft dealoc) {
   self->aloc = aloc;
   self->dealoc = dealoc;
   self->count =0;
}

int acquire (shared_resource_t* self, void* args) {
   if (self->count++ == 0) {
      return self->aloc (args);
   }
   return 0;
}

int release (shared_resource_t* self) {
   if (self->count-- == 1) {
      return self->dealoc();
   }
   if (self->count <0)
      self->count =0;
   return 0;
}

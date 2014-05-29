/*!
 * \file ui.c
 * \brief
 *    A small footprint ui library
 *
 * Copyright (C) 2011 Houtouridis Christos <houtouridis.ch@gmail.com>
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
 * Date:       07/2011
 * Version:
 *
 */

/*!
 * \brief
 *    Creates a Time value box between a min-max domain
 *
 * \param   cap      The Value box caption
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 *
 * \return  The selected time value.
 *
 * This function can create a Value box between a min-max domain.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the time value by step
 * DOWN     --    Decrease the time value by step
 * RIGHT    --    Selected and return the current time value.
 * LEFT     --    Exit with the previous time value
 * ESC      --       "        "        "
 */
time_t ui_timebox (text_t cap, time_t up, time_t down, time_t step, time_t cur)
{
   time_t value = cur;
   int in=0;
   char value_str[12];
   struct tm *s;

   //User Interface Loop
   ui_print_ctrl ('\f');
   ui_print_caption (cap);
   ui_print_ctrl ('\n');
   for ( ; ; )
   {
      // Time Box is painting the screen
      s = localtime(&value);
      if (s->tm_mday)
         sprintf (value_str, "\r= %d:%02d:%02d:%02d", s->tm_mday, s->tm_hour, s->tm_min, s->tm_sec);
      else if (s->tm_hour)
         sprintf (value_str, "\r= %02d:%02d:%02d", s->tm_hour, s->tm_min, s->tm_sec);
      else
         sprintf (value_str, "\r= %02d:%02d", s->tm_min, s->tm_sec);
      ui_print_box (value_str);

      in = ui_getkey (1);
      switch (in)
      {
         case UP:       value += step; break;
         case DOWN:     value -= step; break;
         //Actions
         case ESC:
         case LEFT:
            return cur;
         case RIGHT:
         case ENTER:
            return value;
         default:
            break;
      }
      //Cycle the values
      if (value > up)   value = down;
      if (value < down) value = up;
   }
}

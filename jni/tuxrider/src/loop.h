/* 
 * Tux Racer 
 * Copyright (C) 1999-2001 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _LOOP_H_
#define _LOOP_H_

typedef void (*mode_init_func_ptr_t)(void);
typedef void (*mode_loop_func_ptr_t)( scalar_t time_step );
typedef void (*mode_term_func_ptr_t)(void);

scalar_t get_clock_time();
void register_loop_funcs( game_mode_t mode, 
			  mode_init_func_ptr_t init_func,
			  mode_loop_func_ptr_t loop_func,
			  mode_term_func_ptr_t term_func );
void set_game_mode( game_mode_t mode );
void main_loop();
bool_t is_mode_change_pending();

#endif /* _LOOP_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

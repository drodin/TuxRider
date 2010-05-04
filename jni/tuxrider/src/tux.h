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

#ifndef _TUX_H_
#define _TUX_H_

void adjust_tux_joints( scalar_t turnFact, bool_t isBraking, 
			scalar_t paddling_factor, scalar_t speed,
			vector_t net_force, scalar_t jump_factor );
void  load_tux();
void  draw_tux();
char* get_tux_root_node();
char* get_tux_left_shoulder_joint();
char* get_tux_right_shoulder_joint();
char* get_tux_left_hip_joint();
char* get_tux_right_hip_joint();
char* get_tux_left_knee_joint();
char* get_tux_right_knee_joint();
char* get_tux_left_ankle_joint();
char* get_tux_right_ankle_joint();
char* get_tux_tail_joint();
char* get_tux_neck();
char* get_tux_head();

#endif /* _TUX_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

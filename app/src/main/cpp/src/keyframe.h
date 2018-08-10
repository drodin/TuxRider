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

#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_

#include "tuxracer.h"

void get_key_frame_data( key_frame_t **fp, int *n );
void init_key_frame();
void reset_key_frame();
void update_key_frame( player_data_t *plyr, scalar_t dt );
void mirror_keyframe();
void register_key_frame_callbacks( Tcl_Interp *ip );

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

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

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "tuxracer.h"

#define NUM_COURSE_LIGHTS 8

light_t* get_course_lights();

void reset_lights();

void setup_course_lighting();

void register_course_light_callbacks( Tcl_Interp *ip );

#endif /* _LIGHTS_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

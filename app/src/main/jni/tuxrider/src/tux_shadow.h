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

#ifndef _TUX_SHADOW_H_
#define _TUX_SHADOW_H_

#include "tuxracer.h"

void draw_tux_shadow();
void traverse_dag_for_shadow( scene_node_t *node, matrixgl_t model_matrix );
void draw_shadow_sphere( matrixgl_t model_matrix );
void draw_shadow_vertex( scalar_t x, scalar_t y, scalar_t z, 
			 matrixgl_t model_matrix );

#endif /* _TUX_SHADOW_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

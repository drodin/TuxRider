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

#ifndef _VIEWMODE_H_
#define _VIEWMODE_H_

void set_view_mode( player_data_t *plyr, view_mode_t mode );
view_mode_t get_view_mode( player_data_t *plyr );
void traverse_dag_for_view_point( scene_node_t *node, matrixgl_t trans );
point_t get_tux_view_pt();
void set_tux_eye( tux_eye_t which_eye, point_t pt );
void update_view( player_data_t *plyr, scalar_t dt );
void setup_view_matrix( player_data_t *plyr );

#endif /* _VIEWMODE_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

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

#ifndef _PART_SYS_H_
#define _PART_SYS_H_

void create_new_particles( point_t loc, vector_t vel, int num );
void update_particles( scalar_t time_step );
void clear_particles();
void reset_particles();
void draw_particles( player_data_t *plyr );
void register_particle_callbacks( Tcl_Interp *ip );

#endif /* _PART_SYS_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

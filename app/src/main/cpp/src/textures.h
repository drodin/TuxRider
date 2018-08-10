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

#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#define TEX_SCALE 6

typedef struct {
    GLuint texture_id;
    int    repeatable;
    int    ref_count;
} texture_node_t;

void init_textures();

bool_t load_and_bind_texture( char *binding, char *filename );

bool_t load_texture( char *texname, char *filename, int repeatable );
bool_t get_texture( char *texname, texture_node_t **tex );
bool_t del_texture( char *texname );

bool_t bind_texture( char *binding, char *texname );
bool_t get_texture_binding( char *binding, GLuint *texid );
bool_t unbind_texture( char *binding );

void get_current_texture_dimensions( int *width, int *height );

bool_t flush_textures(void);

void register_texture_callbacks(Tcl_Interp *ip);

#endif /* _TEXTURES_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

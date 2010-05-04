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

#ifndef _TCL_UTIL_H_
#define _TCL_UTIL_H_

#include "tuxracer.h"

int       get_tcl_tuple ( Tcl_Interp *ip, char *inList, scalar_t *p, int n );
int       get_tcl_int_tuple( Tcl_Interp *ip, char *inList, int *p, int n );
point2d_t make_point2d_from_array ( scalar_t *p );
point_t   make_point_from_array ( scalar_t *p );
vector_t  make_vector_from_array ( scalar_t *v );
colour_t  make_colour_from_array ( scalar_t *c );
colour_t  make_colour( scalar_t r, scalar_t g, scalar_t b, scalar_t a );
void      setup_tcl_std_channels();

/* Useful macro for processing Tcl callbacks */
#define NEXT_ARG argc -=1; argv += 1

/* Checks for existence of argument */
#define CHECK_ARG( name_str, err_string, bail_label ) \
if ( *argv == NULL ) { \
    (err_string) = "No argument supplied for " name_str; \
    goto bail_label; \
}

#endif /* _TCL_UTIL_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

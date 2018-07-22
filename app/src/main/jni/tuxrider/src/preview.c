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


#include "tuxracer.h"
#include "course_load.h"
#include "view.h"

#define DTHETA 0.2
#define MAXTHETA  10
#define MINTHETA -10

static scalar_t theta;
static scalar_t sign;
static point_t eye_pt;

void init_preview()
{
    theta = 0;
    sign = 1;
} 

point_t get_preview_eye_pt() 
{
    return eye_pt;
}

void update_preview( player_data_t *plyr )
{
    scalar_t courseWidth, courseLength;
    point_t  coursePt;
    scalar_t courseAngle;

    get_course_dimensions( &courseWidth, &courseLength );
    courseAngle = get_course_angle();

    eye_pt = make_point( sin( ANGLES_TO_RADIANS( theta ) ) * courseLength + 
			courseWidth / 2.,
			-courseLength * 
			 tan( ANGLES_TO_RADIANS( courseAngle ) ) / 2.,
			-cos( ANGLES_TO_RADIANS( theta ) ) * courseLength );

    coursePt = make_point( courseWidth / 2., 
			   -courseLength * 
			   tan( ANGLES_TO_RADIANS( courseAngle ) ) / 2.,
			   -courseLength / 2.);

    plyr->view.pos = eye_pt;
    plyr->view.dir = subtract_points( coursePt, eye_pt );
    plyr->view.up = make_vector( 0, 1, 0 );

    setup_view_matrix( plyr );

    theta += sign*DTHETA;
    if (theta > MAXTHETA ) {
        theta = MAXTHETA;
        sign = -1;
    } else if ( theta < MINTHETA ) {
        theta = MINTHETA;
        sign = +1;
    } 
} 

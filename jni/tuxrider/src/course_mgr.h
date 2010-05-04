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

#ifndef COURSE_MGR_H
#define COURSE_MGR_H 1

#include "tuxracer.h"
#include "list.h"

typedef struct event_data_ event_data_t; /* Opaque */
typedef struct cup_data_ cup_data_t; /* Opaque */

typedef struct {
    char *course;
    char *name;
    char *description;
    scalar_t par_time;
    race_conditions_t conditions;
} open_course_data_t;
 
void init_course_manager();
list_t get_open_courses_list( );
list_t get_events_list(  );
list_elem_t get_event_by_name( char *event_name );
char* get_event_name( event_data_t *event );
char*  get_event_icon_texture_binding( event_data_t *event );
list_t get_event_cup_list( event_data_t *event );
list_elem_t get_event_cup_by_name( event_data_t *event, char *cup_name );
char* get_cup_name( cup_data_t *cup );
char* get_cup_icon_texture_binding( cup_data_t *cup );
list_t get_cup_race_list( cup_data_t *cup );
void register_course_manager_callbacks( Tcl_Interp *ip );
list_elem_t get_last_complete_cup_for_event( event_data_t *event_data );
bool_t is_cup_complete( event_data_t *event_data, list_elem_t cup );
bool_t is_cup_first_incomplete_cup( event_data_t *event_data,
				    list_elem_t cup );
int compare_race_positions( cup_data_t *cup_data,
			    list_elem_t race1, list_elem_t race2 );

#endif /* COURSE_MGR_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */

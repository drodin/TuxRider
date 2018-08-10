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
#include "course_mgr.h"
#include "list.h"
#include "tcl_util.h"
#include "textures.h"
#include "save.h"
#include "multiplayer.h"

static char err_buff[BUFF_LEN];

struct event_data_ {
    char *name;          		/* event name */
    list_t cup_list;			/* list of cups */
};

struct cup_data_ {
    char *name;				/* name of cup */
    list_t race_list;			/* list of races */
};

static bool_t initialized = False;	/* has module been initialized? */
static list_t event_list = NULL;	/* list of events */
static list_t open_course_list = NULL;	/* list of open courses */

static char *race_condition_names[RACE_CONDITIONS_NUM_CONDITIONS] =
{
"sunny",
"cloudy",
"night",
"evening"
};


/*---------------------------------------------------------------------------*/
/*! 
 Initializes the course manager module
 \return  None
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
void init_course_manager() 
{
    check_assertion( initialized == False,
                    "Attempt to initialize course manager twice" );
    
    initialized = True;
    
    event_list = create_list();
    open_course_list = create_list();
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the list of open courses
 \return  List of open courses
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
list_t get_open_courses_list( )
{
    return open_course_list;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the list of events
 \return  List of events
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
list_t get_events_list(  )
{
    return event_list;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the event data for the event with specified name
 \author  jfpatry
 \date    Created:  2000-09-23
 \date    Modified: 2000-09-23
 */
list_elem_t get_event_by_name( char *event_name )
{
    list_elem_t cur_event = NULL;
    event_data_t *data;
    
    cur_event = get_list_head( event_list );
    
    while (1) {
        if ( cur_event == NULL ) {
            return NULL;
        }
        
        data = (event_data_t*) get_list_elem_data( cur_event );
        
        if ( strcmp( data->name, event_name ) == 0 ) {
            return cur_event;
        }
        
        cur_event = get_next_list_elem( event_list, cur_event );
    }
    
    return NULL;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the name of an event
 \return  Name of event
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
char* get_event_name( event_data_t *event )
{
    return event->name;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the texture binding name for the event icon
 \pre     
 \arg \c  foo
 \arg \c  foo
 \arg \c  foo
 
 \return  texture binding name for event icon
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
char*  get_event_icon_texture_binding( event_data_t *event )
{
    /* Same as name, for now */
    return event->name;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the cup list for the event
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
list_t get_event_cup_list( event_data_t *event )
{
    return event->cup_list;
}



/*---------------------------------------------------------------------------*/
/*! 
 Returns the cup data for the specified cup name
 \author  jfpatry
 \date    Created:  2000-09-23
 \date    Modified: 2000-09-23
 */
list_elem_t get_event_cup_by_name( event_data_t *event, char *name )
{
    list_elem_t cur_cup;
    cup_data_t *data;
    
    cur_cup = get_list_head( event->cup_list );
    
    while (1) {
        if ( cur_cup == NULL ) {
            return NULL;
        }
        
        data = (cup_data_t*)get_list_elem_data( cur_cup );
        
        if ( strcmp( data->name, name ) == 0 ) {
            return cur_cup;
        }
        
        cur_cup = get_next_list_elem( event->cup_list, cur_cup );
    }
    
    return NULL;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the name of the specified cup
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
char* get_cup_name( cup_data_t *cup )
{
    return cup->name;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the texture binding name for the cup icon
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
char* get_cup_icon_texture_binding( cup_data_t *cup )
{
    /* Same as cup name for now */
    return cup->name;
    
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the list of races for the specified cup
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
list_t get_cup_race_list( cup_data_t *cup )
{
    return cup->race_list;
}



/*---------------------------------------------------------------------------*/
/*! 
 Returns the last complete cup for the specified event
 \author  jfpatry
 \date    Created:  2000-09-24
 \date    Modified: 2000-09-24
 */
list_elem_t get_last_complete_cup_for_event( event_data_t *event_data )
{
    char *last_cup;
    list_elem_t cup;
    
    if ( get_last_completed_cup( g_game.player[local_player()].name,
                                get_event_name( event_data ),
                                g_game.difficulty,
                                &last_cup ) )
    {
        cup = get_event_cup_by_name( event_data, last_cup );
        
        if ( cup == NULL ) {
            print_warning( IMPORTANT_WARNING,
                          "Couldn't find saved cup `%s'", last_cup );
        }
        return cup;
    } else {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns true iff the cup is complete
 \author  jfpatry
 \date    Created:  2000-09-24
 \date    Modified: 2000-09-24
 */
bool_t is_cup_complete( event_data_t *event_data, list_elem_t cup )
{
    list_elem_t cur_elem;
    list_elem_t last_complete_cup = 
	get_last_complete_cup_for_event( event_data );
    
    if ( last_complete_cup == NULL ) {
        return False;
    }
    
    if ( cup == last_complete_cup ) {
        return True;
    }
    
    cur_elem = get_prev_list_elem( event_data->cup_list, cup );
    
    while (1) {
        if ( cur_elem == NULL ) {
            return True;
        }
        
        if ( cur_elem == last_complete_cup ) {
            return False;
        }
        
        cur_elem = get_prev_list_elem( event_data->cup_list, cur_elem );
    }
}



/*---------------------------------------------------------------------------*/
/*! 
 Returns true iff the specified cup is the first incomplete cup
 \author  jfpatry
 \date    Created:  2000-09-24
 \date    Modified: 2000-09-24
 */
bool_t is_cup_first_incomplete_cup( event_data_t *event_data,
                                   list_elem_t cup )
{
    list_elem_t elem = get_last_complete_cup_for_event( event_data );
    
    if ( elem == NULL ) {
        if ( cup == get_list_head( event_data->cup_list ) ) {
            return True;
        } else {
            return False;
        }
    }
    
    elem = get_next_list_elem( event_data->cup_list, elem );
    
    if ( elem == NULL ) {
        /* All cups complete */
        return False;
    }
    
    if ( cup == elem ) {
        return True;
    } else {
        return False;
    }
}



/*---------------------------------------------------------------------------*/
/*! 
 Compares the positions of two races in a cup
 \return  pos(race2)-pos(race1)
 \author  jfpatry
 \date    Created:  2000-09-24
 \date    Modified: 2000-09-24
 */
int compare_race_positions( cup_data_t *cup_data,
                           list_elem_t race1, list_elem_t race2 )
{
    int incr = 1;
    list_elem_t cur_elem = NULL;
    bool_t found1 = False;
    bool_t found2 = False;
    int diff;
    
    check_assertion( race1 != NULL, "race is null" );
    check_assertion( race2 != NULL, "race is null" );
    check_assertion( cup_data != NULL, "null data" );
    check_assertion( cup_data->race_list != NULL, "null list" );
    
    diff = 0;
    cur_elem = get_list_head( cup_data->race_list );
    while( 1 ) {
        
        if ( cur_elem == NULL ) {
            check_assertion( 0, "race1 or race2 aren't races in the cup" );
        }
        
        if ( cur_elem == race1 ) {
            if ( found2 ) {
                return diff;
            }
            
            found1 = True;
            
            incr = 1;
        }
        
        if ( cur_elem == race2 ) {
            if ( found1 ) {
                return diff;
            }
            
            found2 = True;
            
            incr = -1;
        }
        
        cur_elem = get_next_list_elem( cup_data->race_list, cur_elem );
        
        if ( found1 || found2 ) {
            diff += incr;
        }
    }
    
    code_not_reached();
}



/*---------------------------------------------------------------------------*/
/*! 
 Creates an open_course_data_t object from a Tcl string.
 \author  jfpatry
 \date    Created:  2000-09-21
 \date    Modified: 2000-09-21
 */
open_course_data_t* create_open_course_data( Tcl_Interp *ip, char *string, 
                                            char **err_msg )
{
    char **argv = NULL;
    char **orig_argv = NULL;
    int argc = 0;
    
    char *course = NULL;
    char *name = NULL;
    char *description = NULL;
    race_conditions_t conditions = RACE_CONDITIONS_SUNNY;
    scalar_t par_time = 120;
    
    open_course_data_t *open_course_data = NULL;
    
    if ( Tcl_SplitList( ip, string, &argc, &argv ) == TCL_ERROR ) {
        *err_msg = "open course data is not a list";
        goto bail_open_course_data;
    }
    
    orig_argv = argv;
    
    while ( *argv != NULL ) {
        if ( strcmp( *argv, "-course" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -course in open course data";
                goto bail_open_course_data;
            }
            
            course = string_copy( *argv );
        } else if ( strcmp( *argv, "-name" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -name in open course data";
                goto bail_open_course_data;
            }
            
            name = string_copy( *argv );
        } else if ( strcmp( *argv, "-description" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -description in open course data";
                goto bail_open_course_data;
            }
            
            description = string_copy( *argv );
        } else if ( strcmp( *argv, "-par_time" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                par_time = 120.0;
                print_warning( PEDANTIC_WARNING,
                              "No data supplied for -par_time in open course "
                              "data.  Using %g seconds.", par_time );
            } else if ( Tcl_GetDouble( ip, *argv, &par_time ) != TCL_OK ) {
                *err_msg = "Invalid value for -par_time in open course data";
                goto bail_open_course_data;
            }
        } else if ( strcmp( *argv, "-conditions" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -conditions in open course data";
                goto bail_open_course_data;
            }
            
            int i;
            for ( i=0; i<RACE_CONDITIONS_NUM_CONDITIONS; i++ ) {
                if ( string_cmp_no_case( race_condition_names[i],
                                        *argv ) == 0 )
                {
                    break;
                }
            }
            
            if ( i == RACE_CONDITIONS_NUM_CONDITIONS ) {
                *err_msg = "Invalid value for -conditions in race data";
                goto bail_open_course_data;
            }
            
            conditions = (race_conditions_t)i;
            
        }
        else {
            sprintf( err_buff, "unrecognized option `%s' in open course data",
                    *argv );
            *err_msg = err_buff;
            goto bail_open_course_data;
        }
        
        NEXT_ARG;
    }
    
    /* Check mandatory arguments */
    if ( course == NULL ) {
        *err_msg = "No course specified in open course data";
        goto bail_open_course_data;
    }
    
    if ( name == NULL ) {
        *err_msg = "No name specified in open course data";
        goto bail_open_course_data;
    }
    
    
    /* Create new open_course_data_t object */
    open_course_data = (open_course_data_t*) malloc( sizeof(open_course_data_t) );
    check_assertion( open_course_data != NULL, "out of memory" );
    
    open_course_data->course = course;
    open_course_data->name = name;
    open_course_data->description = description;
    open_course_data->par_time = par_time;
    open_course_data->conditions = conditions;
    
    Tcl_Free( (char*) orig_argv );
    
    return open_course_data;
    
bail_open_course_data:
    
    if ( orig_argv ) {
        Tcl_Free( (char*) orig_argv );
    }
    
    if ( course ) {
        free( course );
    }
    
    if ( name ) {
        free( name );
    }
    
    if ( description ) {
        free( description );
    }
    
    if ( open_course_data ) {
        free( open_course_data );
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*/
/*! 
 tux_open_courses Tcl callback
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
static int open_courses_cb( ClientData cd, Tcl_Interp *ip,
                           int argc, char **argv )
{
    char *err_msg;
    char **list = NULL;
    int num_courses;
    list_elem_t last_elem = NULL;
    int i;
    
    check_assertion( initialized,
                    "course_mgr module not initialized" );
    
    if ( argc != 2 ) {
        err_msg = "Wrong number of arguments";
        goto bail_open_courses;
    }
    
    if ( Tcl_SplitList( ip, argv[1], &num_courses, &list ) == TCL_ERROR ) {
        err_msg = "Argument is not a list";
        goto bail_open_courses;
    }
    
    /* Add items to end of list */
    last_elem = get_list_tail( open_course_list );
    
    for ( i=0; i<num_courses; i++ ) {
        open_course_data_t *data;
        data = create_open_course_data( ip, list[i], &err_msg );
        
        if ( data == NULL ) {
            goto bail_open_courses;
        }
        
        last_elem = insert_list_elem( 
                                     open_course_list, 
                                     last_elem,
                                     (list_elem_data_t) data );
    }
    
    Tcl_Free( (char*) list );
    list = NULL;
    
    return TCL_OK;
    
bail_open_courses:
    
    /* We'll leave the data that was successfully added in the list. */
    
    Tcl_AppendResult(
                     ip,
                     "Error in call to tux_open_courses: ", 
                     err_msg,
                     "\n",
                     "Usage: tux_open_courses { list of open courses }",
                     (NULL) );
    return TCL_ERROR;
}


/*---------------------------------------------------------------------------*/
/*! 
 Creates a race_data_t object from a Tcl string.
 \return  New race_data_t object if successful, or NULL if error
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
static
race_data_t* create_race_data ( Tcl_Interp *ip, char *string, char **err_msg )
{
    char **argv = NULL;
    char **orig_argv = NULL;
    int argc = 0;
    
    char *course = NULL;
    char *name = NULL;
    char *description = NULL;
    
    int      herring_req[DIFFICULTY_NUM_LEVELS];
    bool_t   herring_req_init = False;
    scalar_t time_req[DIFFICULTY_NUM_LEVELS];
    bool_t   time_req_init = False;
    int score_req[DIFFICULTY_NUM_LEVELS];
    bool_t   score_req_init = False;
    
    bool_t   mirrored = False;
    race_conditions_t conditions = RACE_CONDITIONS_SUNNY;
    bool_t   windy = False;
    bool_t   snowing = False;
    
    race_data_t *race_data = NULL;
    
    if ( Tcl_SplitList( ip, string, &argc, &argv ) == TCL_ERROR ) {
        *err_msg = "race data is not a list";
        goto bail_race_data;
    }
    
    orig_argv = argv;
    
    while ( *argv != NULL ) {
        if ( strcmp( *argv, "-course" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -course in race data";
                goto bail_race_data;
            }
            
            course = string_copy( *argv );
        } else if ( strcmp( *argv, "-name" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -name in race data";
                goto bail_race_data;
            }
            
            name = string_copy( *argv );
        } else if ( strcmp( *argv, "-description" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -description in race data";
                goto bail_race_data;
            }
            
            description = string_copy( *argv );
        } else if ( strcmp( *argv, "-herring" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -herring in race data";
                goto bail_race_data;
            }
            
            if ( get_tcl_int_tuple( 
                                   ip, *argv, herring_req, 
                                   sizeof(herring_req)/sizeof(herring_req[0]) ) == TCL_ERROR )
            {
                *err_msg = "Value for -herring is not a list or has "
                "the wrong number of elements";
                goto bail_race_data;
            }
            
            herring_req_init = True;
        } else if ( strcmp( *argv, "-time" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -time in race data" ;
                goto bail_race_data;
            }
            
            if ( get_tcl_tuple( ip, *argv, time_req, 
                               sizeof(time_req)/sizeof(time_req[0]) ) 
                == TCL_ERROR ) 
            {
                *err_msg = "Value for -time is not a list or hsa the "
                "wrong number of elements";
                goto bail_race_data;
            }
            
            time_req_init = True;
        } else if ( strcmp( *argv, "-score" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -score in race data";
                goto bail_race_data;
            }
            
            if ( get_tcl_int_tuple( ip, *argv, score_req,
                               sizeof(score_req)/sizeof(score_req[0]) )
                == TCL_ERROR ) 
            {
                *err_msg = "Value for -score is not a list or has the "
                "wrong number of elements";
                goto bail_race_data;
            }
            
            score_req_init = True;
        } else if ( strcmp( *argv, "-mirrored" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -mirrored in race data";
                goto bail_race_data;
            }
            
            if ( string_cmp_no_case( *argv, "yes" ) == 0 ) {
                mirrored = True;
            } else {
                mirrored = False;
            }
        } else if ( strcmp( *argv, "-conditions" ) == 0 ) {
            int i;
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -conditions in race data";
                goto bail_race_data;
            }
            
            for ( i=0; i<RACE_CONDITIONS_NUM_CONDITIONS; i++ ) {
                if ( string_cmp_no_case( race_condition_names[i],
                                        *argv ) == 0 )
                {
                    break;
                }
            }
            
            if ( i == RACE_CONDITIONS_NUM_CONDITIONS ) {
                *err_msg = "Invalid value for -conditions in race data";
                goto bail_race_data;
            }
            
            conditions = (race_conditions_t)i;
        } else if ( strcmp( *argv, "-windy" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -windy in race data";
                goto bail_race_data;
            }
            
            if ( string_cmp_no_case( *argv, "yes" ) == 0 ) {
                windy = True;
            } else {
                windy = False;
            }
        } else if ( strcmp( *argv, "-snowing" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -snowing in race data";
                goto bail_race_data;
            }
            
            if ( string_cmp_no_case( *argv, "yes" ) == 0 ) {
                snowing = True;
            } else {
                snowing = False;
            }
        } else {
            sprintf( err_buff, "unrecognized option `%s' in race data",
                    *argv );
            *err_msg = err_buff;
            goto bail_race_data;
        }
        
        NEXT_ARG;
    }
    
    /* Check mandatory arguments */
    if ( course == NULL ) {
        *err_msg = "No course specified in race data";
        goto bail_race_data;
    }
    
    if ( !herring_req_init ||
        !time_req_init ||
        !score_req_init ) 
    {
        *err_msg = "Must specify requirement for herring, time, and score.";
        goto bail_race_data;
    }
    
    /* Create new race_data_t object */
    
    race_data = (race_data_t*) malloc( sizeof(race_data_t) );
    check_assertion( race_data != NULL, "out of memory" );
    
    race_data->course = course;
    race_data->name = name;
    race_data->description = description;
    
    memcpy( race_data->herring_req, herring_req, sizeof(herring_req) );
    memcpy( race_data->time_req, time_req, sizeof(time_req) );
    memcpy( race_data->score_req, score_req, sizeof(score_req) );
    
    race_data->mirrored = mirrored;
    race_data->conditions = conditions;
    race_data->windy = windy;
    race_data->snowing = snowing;
    
    Tcl_Free( (char*) orig_argv );
    
    return race_data;
    
bail_race_data:
    
    if ( orig_argv ) {
        Tcl_Free( (char*) orig_argv );
    }
    
    if ( course ) {
        free( course );
    }
    
    if ( name ) {
        free( name );
    }
    
    if ( description ) {
        free( description );
    }
    
    if ( race_data ) {
        free( race_data );
    }
    
    return NULL;
}


/*---------------------------------------------------------------------------*/
/*! 
 Creates a cup_data_t object from a Tcl string.
 \return  New cup_data_t object if successful, or NULL if error
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
static
cup_data_t* create_cup_data( Tcl_Interp *ip, char *string, char **err_msg )
{
    char **argv = NULL;
    char **orig_argv = NULL;
    int argc = 0;
    
    char *name = NULL;
    char *icon = NULL;
    list_t race_list = NULL;
    list_elem_t last_race = NULL;
    char **races = NULL;
    int num_races = 0;
    int i;
    
    cup_data_t *cup_data = NULL;
    
    if ( Tcl_SplitList( ip, string, &argc, &argv ) == TCL_ERROR ) {
        *err_msg = "cup data is not a list";
        goto bail_cup_data;
    }
    
    orig_argv = argv;
    
    while ( *argv != NULL ) {
        if ( strcmp( *argv, "-name" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -name in cup data";
                goto bail_cup_data;
            }
            
            name = string_copy( *argv );
        } else if ( strcmp( *argv, "-icon" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -icon in cup data";
                goto bail_cup_data;
            }
            
            icon = string_copy( *argv );
        } else if ( strcmp( *argv, "-races" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg= "No data supplied for -races in cup data";
                goto bail_cup_data;
            }
            
            race_list = create_list();
            last_race = NULL;
            
            if ( Tcl_SplitList( ip, *argv, &num_races, &races ) == TCL_ERROR ) {
                *err_msg = "Race data is not a list in event data";
                goto bail_cup_data;
            }
            
            for ( i=0; i<num_races; i++) {
                race_data_t *race_data;
                race_data = create_race_data( ip, races[i], err_msg );
                if ( race_data == NULL ) {
                    goto bail_cup_data;
                }
                
                last_race = insert_list_elem( race_list, last_race,
                                             (list_elem_data_t) race_data );
            }
            
            Tcl_Free( (char*) races );
            races = NULL;
        } else {
            sprintf( err_buff, "Unrecognized argument `%s'", *argv );
            *err_msg = err_buff;
            goto bail_cup_data;
        }
        
        NEXT_ARG;
    }
    
    /* Make sure mandatory fields have been specified */
    if ( name == NULL ) {
        *err_msg = "Must specify a name in cup data";
        goto bail_cup_data;
    }
    
    if ( icon == NULL ) {
        *err_msg = "Must specify an icon texture in cup data";
        goto bail_cup_data;
    }
    
    if ( race_list == NULL ) {
        *err_msg = "Must specify a race list in cup data";
        goto bail_cup_data;
    }
    
    /* Create a new cup data object */
    cup_data = (cup_data_t*) malloc( sizeof( cup_data_t ) );
    check_assertion( cup_data != NULL, "out of memory" );
    
    cup_data->name = name;
    cup_data->race_list = race_list;
    
    bind_texture( name, icon );
    
    Tcl_Free( (char*) orig_argv );
    argv = NULL;
    
	free( icon );
    
    return cup_data;
    
bail_cup_data:
    
    if ( orig_argv ) {
        Tcl_Free( (char*) orig_argv );
    }
    
    if ( name ) {
        free( name );
    }
    
    if ( icon ) {
        free( icon );
    }
    
    if ( races ) {
        Tcl_Free( (char*) races );
    }
    
    /* Clean out race list */
    if ( race_list ) {
        last_race = get_list_tail( race_list );
        while ( last_race != NULL ) {
            race_data_t *data;
            data = (race_data_t*) delete_list_elem( race_list, last_race );
            free( data );
            last_race = get_list_tail( race_list );
        }
        
        del_list( race_list );
    }
    
    if ( cup_data ) {
        free( cup_data );
    }
    
    return NULL;
}

/*---------------------------------------------------------------------------*/
/*! 
 Creates an event_data_t object from a Tcl string.
 \return  New event_data_t object if successful, or NULL on error
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
static
event_data_t* create_event_data( Tcl_Interp *ip, char *string, char **err_msg )
{
    char **orig_argv = NULL;
    char **argv = NULL;
    int argc = 0;
    
    char *name = NULL;
    char *icon = NULL;
    list_t cup_list = NULL;
    list_elem_t last_cup = NULL;
    char **cups = NULL;
    int num_cups = 0;
    int i;
    
    event_data_t *event_data = NULL;
    
    if ( Tcl_SplitList( ip, string, &argc, &argv ) == TCL_ERROR ) {
        *err_msg = "event data is not a list";
        goto bail_event_data;
    }
    
    orig_argv = argv;
    
    while ( *argv != NULL ) {
        if ( strcmp( *argv, "-name" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -name in event data";
                goto bail_event_data;
            }
            
            name = string_copy( *argv );
        } else if ( strcmp( *argv, "-icon" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -icon in event data";
                goto bail_event_data;
            }
            
            icon = string_copy( *argv );
        } else if ( strcmp( *argv, "-cups" ) == 0 ) {
            NEXT_ARG;
            
            if ( *argv == NULL ) {
                *err_msg = "No data supplied for -cups in event data";
                goto bail_event_data;
            }
            
            cup_list = create_list();
            last_cup = NULL;
            
            if ( Tcl_SplitList( ip, *argv, &num_cups, &cups ) == TCL_ERROR ) {
                *err_msg = "Cup data is not a list in event data";
                goto bail_event_data;
            }
            
            for ( i=0; i<num_cups; i++ ) {
                cup_data_t *cup_data;
                cup_data = create_cup_data( ip, cups[i], err_msg );
                if ( cup_data == NULL ) {
                    goto bail_event_data;
                }
                
                last_cup = insert_list_elem( cup_list, last_cup,
                                            (list_elem_data_t) cup_data );
            }
            
            Tcl_Free( (char*) cups );
            cups = NULL;
        } else {
            sprintf( err_buff, "Unrecognized argument `%s'", *argv );
            *err_msg = err_buff;
            goto bail_event_data;
        }
        
        NEXT_ARG;
    }
    
    /* Make sure mandatory fields have been specified */
    if ( name == NULL ) {
        *err_msg = "Must specify a name in event data";
        goto bail_event_data;
    }
    
    if ( icon == NULL ) {
        *err_msg = "Must specify an icon texture in event data";
        goto bail_event_data;
    }
    
    if ( cup_list == NULL ) {
        *err_msg = "Must specify a cup list in event data";
        goto bail_event_data;
    }
    
    /* Create new event data object */
    event_data = (event_data_t*) malloc( sizeof( event_data_t ) );
    check_assertion( event_data != NULL, "out of memory" );
    
    event_data->name = name;
    event_data->cup_list = cup_list;
    
    bind_texture( name, icon );
    free(icon);
    
    Tcl_Free( (char*) orig_argv );
    argv = NULL;
    
    return event_data;
    
bail_event_data:
    
    if ( orig_argv ) {
        Tcl_Free( (char*) orig_argv );
    }
    
    if ( name ) {
        free( name );
    }
    
    if ( icon ) {
        free( name );
    }
    
    if ( cups ) {
        Tcl_Free( (char*) cups );
    }
    
    /* Clean out cup list */
    if ( cup_list ) {
        last_cup = get_list_tail( cup_list );
        while ( last_cup != NULL ) {
            cup_data_t *data;
            data = (cup_data_t*) delete_list_elem( cup_list, 
                                                  last_cup );
            free( data );
            last_cup = get_list_tail( cup_list );
        }
        
        del_list( cup_list );
    }
    
    if ( event_data ) {
        free( event_data );
    }
    
    return NULL;
}


/*---------------------------------------------------------------------------*/
/*! 
 tux_events Tcl callback
 Here's a sample call to tux_events:
 
 tux_events {
 { 
 -name "Herring Run" -icon noicon -cups {
 { 
 -name "Cup 1" -icon noicon -races {
 {
 -course path_of_daggers \
 -description "nice long description" \
 -herring { 15 20 25 30 } \
 -time { 40.0 35.0 30.0 25.0 } \
 -score { 0 0 0 0 } \
 -mirrored yes -conditions cloudy \
 -windy no -snowing no
 }
 {
 -course ingos_speedway \
 -description "nice long description" \
 -herring { 15 20 25 30 } \
 -time { 40.0 35.0 30.0 25.0 } \
 -score { 0 0 0 0 } \
 -mirrored yes -conditions cloudy \
 -windy no -snowing no
 }
 }
 -name "Cup 2" -icon noicon -races {
 {
 -course penguins_cant_fly \
 -description "nice long description" \
 -herring { 15 20 25 30 } \
 -time { 40.0 35.0 30.0 25.0 } \
 -score { 0 0 0 0 } \
 -mirrored yes -conditions cloudy \
 -windy no -snowing no
 }
 {
 -course ingos_speedway \
 -description "nice long description" \
 -herring { 15 20 25 30 } \
 -time { 40.0 35.0 30.0 25.0 } \
 -score { 0 0 0 0 } \
 -mirrored yes -conditions cloudy \
 -windy no -snowing no
 }
 }
 }
 }
 }
 }
 
 \return  Tcl error code
 \author  jfpatry
 \date    Created:  2000-09-19
 \date    Modified: 2000-09-19
 */
static int events_cb( ClientData cd, Tcl_Interp *ip,
                     int argc, char **argv )
{
    char *err_msg;
    char **list = NULL;
    int num_events;
    list_elem_t last_event = NULL;
    int i;
    
    /* Make sure module has been initialized */
    check_assertion( initialized,
                    "course_mgr module not initialized" );
    
    if ( argc != 2 ) {
        err_msg = "Incorrect number of arguments";
        goto bail_events;
    }
    
    if ( Tcl_SplitList( ip, argv[1], &num_events, &list ) == TCL_ERROR ) {
        err_msg = "Argument is not a list";
        goto bail_events;
    }
    
    /* We currently only allow tux_events to be called once */
    last_event = get_list_tail( event_list );
    
    if ( last_event != NULL ) {
        err_msg = "tux_events has already been called; it can only be called "
	    "once.";
        goto bail_events;
    }
    
    for (i=0; i<num_events; i++) {
        event_data_t *data = create_event_data( ip, list[i], &err_msg );
        
        if ( data == NULL ) {
            goto bail_events;
        }
        
        last_event = insert_list_elem( event_list, last_event, 
                                      (list_elem_data_t) data );
    }
    
    Tcl_Free( (char*) list );
    list = NULL;
    
    return TCL_OK;
    
bail_events:
    if ( list != NULL ) {
        Tcl_Free( (char*) list );
    }
    
    /* Clean out event list */
    if ( event_list != NULL ) {
        last_event = get_list_tail( event_list );
        while ( last_event != NULL ) {
            event_data_t *data;
            data = (event_data_t*) delete_list_elem( event_list, 
                                                    last_event );
            free( data );
            last_event = get_list_tail( event_list );
        }
    }
    
    Tcl_AppendResult(
                     ip,
                     "Error in call to tux_events: ", 
                     err_msg,
                     "\n",
                     "Usage: tux_events { list of event data }",
                     (NULL) );
    return TCL_ERROR;
}


/*---------------------------------------------------------------------------*/
/*! 
 Returns the current race conditions (sunny, cloudy, etc.)
 \author  jfpatry
 \date    Created:  2000-09-25
 \date    Modified: 2000-09-25
 */
static int get_race_conditions_cb( ClientData cd, Tcl_Interp *ip,
                                  int argc, char **argv )
{
    char *err_msg;
    Tcl_Obj *result;
    
    if ( argc != 1 ) {
        err_msg = "Incorrect number of arguments";
        goto bail_race_conditions;
    }
    
    result = Tcl_NewStringObj( 
                              race_condition_names[ g_game.race.conditions ],
                              strlen( race_condition_names[ g_game.race.conditions ] ) );;
    
    Tcl_SetObjResult( ip, result );
    
    return TCL_OK;
    
bail_race_conditions:
    
    Tcl_AppendResult(
                     ip,
                     "Error in call to tux_get_race_conditions: ", 
                     err_msg,
                     "\n",
                     "Usage: tux_get_race_conditions",
                     (NULL) );
    return TCL_ERROR;
}

void register_course_manager_callbacks( Tcl_Interp *ip )
{
    Tcl_CreateCommand (ip, "tux_open_courses", open_courses_cb, 0,0);
    Tcl_CreateCommand (ip, "tux_events", events_cb, 0,0);
    Tcl_CreateCommand (ip, "tux_get_race_conditions", 
                       get_race_conditions_cb, 0,0);
}

/* EOF */

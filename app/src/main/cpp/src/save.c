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
#include "save.h"
#include "hash.h"
#include "game_config.h"


#include "course_mgr.h"
#include "multiplayer.h"
#include "game_logic_util.h"

#define STANDARD_RECORD_SIZE 512

#define SAVE_MAGIC_V1 "fish"

/* Saved game data structures */
typedef enum {
    INVALID_DATA = -1,
    EVENT_INFO = 0,
    RACE_RESULTS,
    NUM_EVENT_DATA_TYPES
} save_data_type_t;

typedef struct {
    char event[MAX_EVENT_NAME_LENGTH];
    char cup[MAX_CUP_NAME_LENGTH];
    difficulty_level_t difficulty;
} single_event_t;

typedef struct {
    char event[MAX_EVENT_NAME_LENGTH];
    char cup[MAX_EVENT_NAME_LENGTH];
    char race[MAX_EVENT_NAME_LENGTH];
    difficulty_level_t difficulty;
    double time;
    int herring;
    int score;
} race_results_t;

typedef union {
    single_event_t event;
    race_results_t results;
} save_data_t;

typedef struct {
    save_data_type_t data_type;
    save_data_t data;
    char future_use[STANDARD_RECORD_SIZE - sizeof(save_data_t) - 
		   sizeof(save_data_type_t)];
} save_info_t;

    /*
      #if (STANDARD_RECORD_SIZE - sizeof(single_event_t) < 0)
      ERROR: event_info_t overruns STANDARD_RECORD_SIZE
      #endif
    */

/* High score data structures */
typedef enum {
    SCORE_INFO,
    NUM_SCORE_DATA_TYPES
} score_data_t;

typedef struct {
    score_data_t data_type;
    single_event_t event;
    char player_name[MAX_PLAYER_NAME_LENGTH];
    int score;
    char future_use[STANDARD_RECORD_SIZE - sizeof(single_event_t) -
	sizeof(int) - MAX_PLAYER_NAME_LENGTH];
} score_info_t;

    /*
      #if (STANDARD_RECORD_SIZE - sizeof(single_event_t) - \
      sizeof(int) - MAX_NAME_LENGTH < 0)
      ERROR: score_info_t overruns STANDARD_RECORD_SIZE
      #endif
    */

/*
 * progress_save_table is indexed on player and references 
 * hash tables that are indexed on event and reference save_info_t[d]
 *    player->event->save[difficulty]
 */
static hash_table_t progress_save_table;

/* 
 * player->event->cup->race->results
 */
static hash_table_t results_save_table[DIFFICULTY_NUM_LEVELS];

/*
 * score_table is indexed on event and references pointers to
 * hash tables that are indexed on cup and reference score_info_t*
 *    event->cup->score
 */
static hash_table_t score_table; 

#define HIGH_SCORE_FILE "scores"

int get_high_score_file_name( char *buff, int len )
{
    if (get_config_dir_name( buff, len ) != 0) {
	return 1;
    }
    if ( strlen( buff ) + strlen( HIGH_SCORE_FILE ) +2 > len ) {
	return 1;
    }

#if defined( WIN32 ) 
    strcat( buff, "\\" );
#else
    strcat( buff, "/" );
#endif /* defined( WIN32 ) */

    strcat( buff, HIGH_SCORE_FILE);
    return 0;
}

int get_save_game_file_name( char *buff, char *player, int len )
{
    if (get_config_dir_name( buff, len ) != 0) {
	return 1;
    }
    if ( strlen( buff ) + strlen( player ) + 6 > len ) {
	return 1;
    }

    strcat( buff, DIR_SEPARATOR );

    strcat( buff, player);
    strcat( buff, ".sav");
    return 0;
}

void init_high_scores( void ) 
{
    FILE *score_stream;
    char score_file[BUFF_LEN];
    score_info_t this_score;

    score_table = create_hash_table();
    if ( get_high_score_file_name( score_file, sizeof(score_file) ) == 0 ) {
	score_stream = fopen( score_file, "r" );
	if (score_stream != NULL) {
	    while (fread( &this_score, sizeof(this_score), 1, score_stream)) {
		set_high_score( this_score.event.event, this_score.event.cup,
				this_score.player_name, this_score.score );
	    }
	    if ( fclose( score_stream ) != 0 ) {
		perror( "fclose" );
	    }
	}
    }
}

bool_t get_high_score( char* event, char* cup, char** player, int *score )
{
    hash_table_t *cup_table;
    score_info_t *this_score;

    if ( get_hash_entry( score_table, event, (hash_entry_t*)&cup_table ) ) {
	if ( get_hash_entry( *cup_table, cup, (hash_entry_t*)&this_score ) ) {
	    *player = this_score->player_name;
	    *score = this_score->score;
	    return True;
	}
    }
    return False;
}


bool_t set_high_score( char* event, char* cup, char* player, int score )
{
    hash_table_t *cup_table;
    score_info_t *this_score;

    if ( !get_hash_entry( score_table, event, (hash_entry_t*)&cup_table ) ) {
	cup_table = (hash_table_t*)malloc(sizeof(hash_table_t));
	*cup_table = create_hash_table();
	add_hash_entry( score_table, event, cup_table );
    }

    if ( !get_hash_entry( *cup_table, cup, (hash_entry_t*)&this_score ) ) {
	this_score = (score_info_t*)malloc(sizeof(score_info_t));
	strcpy(this_score->event.event, event);
	strcpy(this_score->event.cup, cup);
	this_score->event.difficulty = (difficulty_level_t)0;
	this_score->data_type = (score_data_t)0;
	add_hash_entry( *cup_table, cup, this_score );
    }

    this_score->score = score;
    strcpy( this_score->player_name, player );
    return True;
}

void write_high_scores( void )
{
    hash_table_t *cup_table;
    hash_search_t score_scan_ptr;
    hash_search_t cup_scan_ptr;
    char *event_name;
    char *cup_name;
    score_info_t *this_score;
    FILE *score_stream;
    char score_file[BUFF_LEN];

    if ( get_high_score_file_name( score_file, sizeof(score_file) ) == 0 ) {
	score_stream = fopen( score_file, "w" );
	begin_hash_scan( score_table, &score_scan_ptr );
	while ( next_hash_entry( score_scan_ptr, &event_name, (hash_entry_t*)&cup_table ) ) {
	    begin_hash_scan( *cup_table, &cup_scan_ptr );
	    while ( next_hash_entry( cup_scan_ptr, &cup_name, (hash_entry_t*)&this_score ) ) {
		fwrite( this_score, sizeof(this_score), 1, score_stream );
	    }
	    end_hash_scan( cup_scan_ptr );
	}
	end_hash_scan( score_scan_ptr );
	if ( fclose( score_stream ) != 0 ) {
	    perror( "fclose" );
	}
    }
}

bool_t get_sav_index( char* str, int *index )
{
    int sav_index;
    sav_index = strlen(str) - 1; 
    if (str[sav_index] != 'v') {
	return False;
    }
    sav_index--;
    if (str[sav_index] != 'a') {
	return False;
    }
    sav_index--;
    if (str[sav_index] != 's') {
	return False;
    }
    sav_index--;
    if (str[sav_index] != '.') {
	return False;
    }
    *index = sav_index;
    return True;
}

void init_saved_games( void ) 
{
    char dir_name[BUFF_LEN];
    FILE* save_stream;
    save_info_t this_save;
    char player_name[BUFF_LEN];
    int sav_index;
    char file_name[BUFF_LEN];
    int i;
    char magic[4];
    list_t dir_file_list = NULL;
    list_elem_t cur_dir_file = NULL;
    char *cur_dir_filename = NULL;


    progress_save_table = create_hash_table();

    for (i=0; i<DIFFICULTY_NUM_LEVELS; i++) {
	results_save_table[i] = create_hash_table();
    }
 
    if (get_config_dir_name( dir_name, sizeof(dir_name) ) != 0) {
	return;
    }

    dir_file_list = get_dir_file_list(dir_name);
    if ( dir_file_list == NULL ) {
	/* Config dir doesn't exist.  Don't print warning since this is a
	   normal condition the first time the program is run. */
	return;
    }

    for ( cur_dir_file = get_list_head( dir_file_list );
	  cur_dir_file != NULL;
	  cur_dir_file = get_next_list_elem( dir_file_list, cur_dir_file ) )
    {
	cur_dir_filename = (char*) get_list_elem_data( cur_dir_file );

	if (get_sav_index(cur_dir_filename, &sav_index)) {

	    strncpy(player_name, cur_dir_filename, sav_index);
	    player_name[sav_index] = '\0';

	    sprintf( file_name, "%s" DIR_SEPARATOR "%s", 
		     dir_name,
		     cur_dir_filename );

	    save_stream = fopen( file_name, "r" );

	    if ( fread( magic, sizeof(magic), 1, save_stream ) != 1 ||
		 strncmp( magic, SAVE_MAGIC_V1, sizeof(magic) ) != 0 ) 
	    {
		print_warning( IMPORTANT_WARNING,
			       "`%s' is not a valid saved game file",
			       file_name );
		fclose( save_stream);
		continue;
	    }


	    if (save_stream != NULL) {
		while (fread( &this_save, sizeof(this_save), 1, save_stream)) {
		    switch ( this_save.data_type ) {
		    case EVENT_INFO:
			set_last_completed_cup( player_name, 
						this_save.data.event.event, 
						this_save.data.event.difficulty, 
						this_save.data.event.cup );
			print_debug( DEBUG_SAVE,
				     "Read completed from `%s': "
				     "name: %s, event: %s, difficulty: %d, cup: %s",
				     cur_dir_filename, 
				     player_name,
				     this_save.data.event.event,
				     this_save.data.event.difficulty,
				     this_save.data.event.cup );
			break;

		    case RACE_RESULTS:
			set_saved_race_results( player_name,
						this_save.data.results.event,
						this_save.data.results.cup,
						this_save.data.results.race,
						this_save.data.results.difficulty,
						this_save.data.results.time,
						this_save.data.results.herring,
						this_save.data.results.score );
			print_debug( DEBUG_SAVE,
				     "Read results from `%s': "
				     "name: %s, event: %s, cup: %s, "
				     "race: %s, difficulty: %d, time: %g, "
				     "herring: %d, score: %d",
				     cur_dir_filename, 
				     player_name,
				     this_save.data.results.event,
				     this_save.data.results.cup,
				     this_save.data.results.race,
				     this_save.data.results.difficulty,
				     this_save.data.results.time,
				     this_save.data.results.herring,
				     this_save.data.results.score );
			break;

		    default:
			print_warning( IMPORTANT_WARNING, 
				       "Unrecognized data type in save file." );

		    }
		}


		if ( fclose( save_stream ) != 0 ) {
		    perror( "fclose" );
		}
	    } else {
		print_warning( IMPORTANT_WARNING,
			       "Couldn't read file `%s': %s",
			       file_name, strerror( errno ) );
	    }
	}
    }

    free_dir_file_list( dir_file_list );
}

bool_t get_last_completed_cup( char* player, char* event, 
			       difficulty_level_t d, char** cup ) 
{
    hash_table_t event_table;
    save_info_t *this_save;

    if ( get_hash_entry( progress_save_table, player, (hash_entry_t*)&event_table ) ) {
	if ( get_hash_entry( event_table, event, (hash_entry_t*)&this_save ) ) {
	    if (this_save[d].data_type == EVENT_INFO) {
		*cup = this_save[d].data.event.cup;
		return True;
	    }
	}
    }
    return False;
}


bool_t set_last_completed_cup( char* player, char* event, 
			       difficulty_level_t d, char* cup )
{
    hash_table_t event_table;
    save_info_t *this_save;
    difficulty_level_t level;
    int i;

    if ( !get_hash_entry( progress_save_table, player, (hash_entry_t*)&event_table ) ) {
	event_table = create_hash_table();
	add_hash_entry( progress_save_table, player, event_table );
    }

    if ( !get_hash_entry( event_table, event, (hash_entry_t*)&this_save ) ) {
	this_save = (save_info_t*)malloc(sizeof(save_info_t)*
					 DIFFICULTY_NUM_LEVELS);
	memset( this_save, 0, sizeof(save_info_t) * DIFFICULTY_NUM_LEVELS );

	for( i=0; i<DIFFICULTY_NUM_LEVELS; i++ ) {
	    level = (difficulty_level_t)i;
	    strcpy( this_save[level].data.event.event, event );
	    this_save[level].data.event.difficulty = d;

	    if ( level != d ) {
		this_save[level].data_type = INVALID_DATA;
	    } else {
		this_save[level].data_type = EVENT_INFO;
	    }
	}
	add_hash_entry( event_table, event, this_save );
    }

    this_save[d].data_type = EVENT_INFO;
    strcpy( this_save[d].data.event.cup, cup );
    return True;
}

bool_t get_saved_race_results( char *player,
			       char *event,
			       char *cup,
			       char *race,
			       difficulty_level_t d,
			       scalar_t *time,
			       int *herring,
			       int *score )
{
    hash_table_t player_table;
    hash_table_t event_table;
    hash_table_t cup_table;
    hash_table_t race_table;
    save_info_t *this_save;

    player_table = results_save_table[d];

    if ( get_hash_entry( player_table, player, (hash_entry_t*)&event_table ) &&
	 get_hash_entry( event_table, event, (hash_entry_t*)&cup_table ) &&
	 get_hash_entry( cup_table, cup, (hash_entry_t*)&race_table ) &&
	 get_hash_entry( race_table, race, (hash_entry_t*)&this_save ) )
    {
	*time = this_save->data.results.time;
	*herring = this_save->data.results.herring;
	*score = this_save->data.results.score;
	
	return True;
    }

    return False;
}

bool_t set_saved_race_results( char *player,
			       char *event,
			       char *cup,
			       char *race,
			       difficulty_level_t d,
			       scalar_t time,
			       int herring,
			       int score )
{
    hash_table_t player_table;
    hash_table_t event_table;
    hash_table_t cup_table;
    hash_table_t race_table;
    save_info_t *this_save;

    player_table = results_save_table[d];

    if ( !get_hash_entry( player_table, player, (hash_entry_t*)&event_table ) )
    {
	event_table = create_hash_table();
	add_hash_entry( player_table, player, (hash_entry_t)event_table );
    }

    if ( !get_hash_entry( event_table, event, (hash_entry_t*)&cup_table ) ) {
	cup_table = create_hash_table();
	add_hash_entry( event_table, event, (hash_entry_t)cup_table );
    }

    if ( !get_hash_entry( cup_table, cup, (hash_entry_t*)&race_table ) ) {
	race_table = create_hash_table();
	add_hash_entry( cup_table, cup, (hash_entry_t)race_table );
    }

    if ( !get_hash_entry( race_table, race, (hash_entry_t*)&this_save ) ) {
	this_save = (save_info_t*)malloc(sizeof(save_info_t));
	memset( this_save, 0, sizeof(save_info_t) );
	add_hash_entry( race_table, race, (hash_entry_t)this_save );
	this_save->data_type = RACE_RESULTS; 
    }

    check_assertion( this_save->data_type == RACE_RESULTS,
		     "Invalid data type" );

    strcpy( this_save->data.results.event, event );
    strcpy( this_save->data.results.cup, cup );
    strcpy( this_save->data.results.race, race );
    this_save->data.results.difficulty = d;
    this_save->data.results.time = time;
    this_save->data.results.herring = herring;
    this_save->data.results.score = score;

#ifdef __APPLE__
    // Write as soon as possible
    write_saved_games();
#endif

    return True;
}


/*---------------------------------------------------------------------------*/
/*! 
  Truncates a player's saved data file
  \author  jfpatry
  \date    Created:  2000-09-24
  \date    Modified: 2000-09-24
*/
static void truncate_and_init_save_file( char *player_name )
{
    char save_file[BUFF_LEN];
    FILE* save_stream;

    if ( get_save_game_file_name( save_file, player_name, 
				  sizeof(save_file) ) == 0 ) 
    {
	save_stream = fopen( save_file, "w" );
	if ( save_stream == NULL ) {
	    print_warning( IMPORTANT_WARNING,
			   "Couldn't open `%s' for writing: %s",
			   save_file, 
			   strerror( errno ) );
	} else {

	    check_assertion( strlen( SAVE_MAGIC_V1 ) == 4,
			     "Magic number has wrong size" );
	    fwrite( SAVE_MAGIC_V1, 4, 1, save_stream );

	    if ( fclose( save_stream ) != 0 ) {
		print_warning( IMPORTANT_WARNING,
			       "Couldn't close `%s': %s",
			       save_file, 
			       strerror( errno ) );
	    }
	}
    } else {
	print_warning( IMPORTANT_WARNING,
		       "Couldn't get save game file for player `%s'", 
		       player_name );
    }
}


void write_saved_games( void )
{
    hash_search_t player_scan_ptr;
    hash_table_t event_table;
    hash_search_t event_scan_ptr;
    hash_table_t cup_table;
    hash_search_t cup_scan_ptr;
    hash_table_t race_table;
    hash_search_t race_scan_ptr;
    hash_search_t save_scan_ptr;
    char *player_name;
    char *event_name;
    FILE* save_stream;
    char save_file[BUFF_LEN];
    save_info_t *this_save;
    difficulty_level_t level;
    int i;
    hash_table_t player_table;

    /* Create list of players */
    player_table = create_hash_table();

    begin_hash_scan( progress_save_table, &player_scan_ptr );
    while ( next_hash_entry( player_scan_ptr, &player_name, NULL ) ) 
    {
	if ( !get_hash_entry( player_table, player_name, NULL ) ) {
	    add_hash_entry( player_table, player_name, "" );
	}
    }
    end_hash_scan( player_scan_ptr );

    for (i=0; i<DIFFICULTY_NUM_LEVELS; i++) {
	begin_hash_scan( results_save_table[i], &player_scan_ptr );
	while ( next_hash_entry( player_scan_ptr, &player_name, NULL ) ) 
	{
	    if ( !get_hash_entry( player_table, player_name, NULL ) ) {
		add_hash_entry( player_table, player_name, "" );
	    }
	}
	end_hash_scan( player_scan_ptr );
    }


    /* truncate and initialize all save files */
    begin_hash_scan( player_table, &player_scan_ptr );
    while ( next_hash_entry( player_scan_ptr, &player_name, NULL )  )
    {
	truncate_and_init_save_file( player_name );
    }
    end_hash_scan( player_scan_ptr );


    /* Don't need player list anymore */
    del_hash_table( player_table );
    player_table = NULL;


    begin_hash_scan( progress_save_table, &player_scan_ptr );
    while ( next_hash_entry( player_scan_ptr, &player_name, 
			     (hash_entry_t*)&event_table ) ) 
    {
	if ( get_save_game_file_name( save_file, player_name, 
				      sizeof(save_file) ) == 0 ) 
	{
	    save_stream = fopen( save_file, "ab" );

	    if ( save_stream == NULL ) {
		print_warning( IMPORTANT_WARNING,
			       "Couldn't open `%s' for writing: %s",
			       save_file, 
			       strerror( errno ) );
	    } else {
		begin_hash_scan( event_table, &save_scan_ptr );

		while ( next_hash_entry( save_scan_ptr, &event_name, 
					 (hash_entry_t*)&this_save ) ) 
		{
		    for( i=0; i<DIFFICULTY_NUM_LEVELS; i++ ) {
			level = (difficulty_level_t)i;
			if ( this_save[level].data_type >= 0 ) {
			    fwrite( &(this_save[level]), 
				    sizeof(this_save[level]), 
				    1, 
				    save_stream );
			}
		    }
		}
		end_hash_scan( save_scan_ptr );

		if ( fclose( save_stream ) != 0 ) {
		    perror( "fclose" );
		}
		save_stream = NULL;
	    }
	}
    }
    end_hash_scan( player_scan_ptr );

    for (i=0; i<DIFFICULTY_NUM_LEVELS; i++) {
	begin_hash_scan( results_save_table[i], &player_scan_ptr );
	while ( next_hash_entry( player_scan_ptr, &player_name, 
				 (hash_entry_t*)&event_table ) ) 
	{
	    if ( get_save_game_file_name( save_file, player_name, 
					  sizeof(save_file) ) == 0 ) 
	    {
		save_stream = fopen( save_file, "ab" );

		if ( save_stream == NULL ) {
		    print_warning( IMPORTANT_WARNING,
				   "Couldn't open `%s' for writing: %s",
				   save_file, 
				   strerror( errno ) );
		} else {
		    begin_hash_scan( event_table, &event_scan_ptr );
		    while ( next_hash_entry( event_scan_ptr, NULL, 
					     (hash_entry_t*)&cup_table ) ) 
		    {
			begin_hash_scan( cup_table, &cup_scan_ptr );
			while ( next_hash_entry( cup_scan_ptr, NULL,
						 (hash_entry_t*)&race_table ) )
			{
			    begin_hash_scan( race_table, &race_scan_ptr );
			    while ( next_hash_entry( race_scan_ptr, NULL,
						 (hash_entry_t*)&this_save ) )
			    {
				fwrite( this_save, 
					sizeof(save_info_t), 
					1, 
					save_stream );
				
			    }
			    end_hash_scan( race_scan_ptr );
			}
			end_hash_scan( cup_scan_ptr );
		    }
		    end_hash_scan( event_scan_ptr );

		    if ( fclose( save_stream ) != 0 ) {
			perror( "fclose" );
		    }
		    save_stream = NULL;
		}
	    }
	}
	end_hash_scan( player_scan_ptr );
    }
}

#ifdef __APPLE__
char* editSynchronizeScoresRequest()
{
    
    list_t race_list = NULL;
    list_elem_t cur_elem = NULL;
    player_data_t *plyr = NULL;
    
    open_course_data_t *data;
    
    plyr = get_player_data( local_player() );
    
    g_game.current_event = "__Practice_Event__";
    g_game.current_cup = "__Practice_Cup__";
    g_game.difficulty = 1;
    
    race_list = get_open_courses_list();
    
    cur_elem = get_list_head( race_list );
    
    scalar_t time;
    int herring;
    int score;
    int minutes;
    int seconds;
    int hundredths;
    int i=0;
    char request[10000];
    char* tempRequest;
    strcpy(request,"");
    do
    {
        data = (open_course_data_t*) get_list_elem_data( cur_elem );
        if ( get_saved_race_results( plyr->name,
                                    g_game.current_event,
                                    g_game.current_cup,
                                    data->name,
                                    g_game.difficulty,
                                    &time,
                                    &herring,
                                    &score ) )
        {
            if (i>0) strcat(request,"&");
            get_time_components( time, &minutes, &seconds, &hundredths);
            asprintf(&tempRequest,"piste[%d]=%s&score[%d]=%d&herring[%d]=%d&time[%d]=%02d:%02d:%02d",i,data->name,i,score,i,herring,i,minutes,seconds,hundredths);
            strcat(request,tempRequest);
            free(tempRequest);
            i++;
        }
    }
    while (cur_elem = get_next_list_elem(race_list, cur_elem));
    return request;
}


#endif

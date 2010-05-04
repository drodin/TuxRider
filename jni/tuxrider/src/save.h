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

#ifndef _SAVE_H_
#define _SAVE_H_

#include "tuxracer.h"

#define MAX_EVENT_NAME_LENGTH 40
#define MAX_CUP_NAME_LENGTH 40
#define MAX_PLAYER_NAME_LENGTH 40

void init_saved_games( void );
bool_t get_last_completed_cup( char* player, char* event, 
			       difficulty_level_t d, char** cup );
bool_t set_last_completed_cup( char* player, char* event, 
			       difficulty_level_t d, char* cup );
bool_t get_saved_race_results( char *player,
			       char *event,
			       char *cup,
			       char *race,
			       difficulty_level_t d,
			       scalar_t *time,
			       int *herring,
			       int *score );

bool_t set_saved_race_results( char *player,
			       char *event,
			       char *cup,
			       char *race,
			       difficulty_level_t d,
			       scalar_t time,
			       int herring,
			       int score );
void write_saved_games( void );


void init_high_scores( void );
bool_t get_high_score( char* event, char* cup, char** player, int *score );
bool_t set_high_score( char* event, char* cup, char* player, int score );
void write_high_scores( void );

#ifdef __APPLE__
    char* editSynchronizeScoresRequest();
#endif

#endif /* _SAVE_H_ */

#ifdef __cplusplus
} /* extern "C" */
#endif

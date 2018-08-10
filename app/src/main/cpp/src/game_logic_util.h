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

#ifndef GAME_LOGIC_UTIL_H
#define GAME_LOGIC_UTIL_H 1

#include "tuxracer.h"

bool_t was_current_race_won();
bool_t is_current_race_last_race_in_cup( void );
bool_t did_player_beat_best_results( void );
bool_t is_current_cup_complete( void );
void update_player_score( player_data_t *plyr );
void get_time_components( scalar_t time, int *minutes, int *seconds,
			  int *hundredths );
#ifdef __APPLE__
    int calculate_player_score(player_data_t *plyr);
#endif

#endif /* GAME_LOGIC_UTIL_H */

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Emacs Customizations
;;; Local Variables: ***
;;; c-basic-offset:0 ***
;;; End: ***
*/

/* EOF */

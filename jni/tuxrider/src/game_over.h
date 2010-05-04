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

#ifndef _GAME_OVER_H_
#define _GAME_OVER_H_

void game_over_init();
void game_over_loop( scalar_t time_step );
void game_over_register();

#ifdef __APPLE__  
    void displaySavedAndRankings(const char* msg, const char* friends, const char* country, const char* world);
    void saveAndDisplayRankings();
#endif

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

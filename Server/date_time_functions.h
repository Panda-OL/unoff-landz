/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/

#ifndef DATE_TIME_FUNCTIONS_H_INCLUDED
#define DATE_TIME_FUNCTIONS_H_INCLUDED

#include <time.h>       // supports time_t data type
#include <sys/time.h>   // support for gettimeofday function


extern struct timeval time_check;//output struct for gettimeofday function


/** RESULT  : gets a date stamp in format 'date month year'

    RETURNS : void

    PURPOSE : stamp the log with the current date

    NOTES   : buffer must be 11 bytes long
*/
void get_verbose_date_str(time_t raw_time, char *buffer);


/** RESULT  : gets a time string in format 'hrs mins' based on the difference between time values

    RETURNS : void

    PURPOSE : to return the time the server has been up on the motd header

    NOTES   : buffer must be 15 bytes long
*/
void get_time_up_str(time_t raw_time, char *buffer);


/** RESULT  : sends the verbose date to the client

    RETURNS : void

    PURPOSE : used by GET_DATE protocol

    NOTES   :
**/
void send_verbose_date(int actor_node, int game_days);


/** RESULT  : sends the verbose time to the client

    RETURNS : void

    PURPOSE : used by GET_TIME protocol

    NOTES   :
**/
void send_verbose_time(int actor_node, int game_minutes);


/*****************************************************************************************************
***                                   C FUNCTIONS CALLED FROM C++ MODULES                          ***
******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT  : gets a time stamp in format 'hh:mm:ss'

    RETURNS : void

    PURPOSE : stamp the log with the time of an event

    NOTES   : buffer must be 9 bytes long
*/
void get_time_stamp_str(time_t raw_time, char *buffer);


/** RESULT  : gets a date stamp in format 'dd:mm:yyyy'

    RETURNS : void

    PURPOSE : stamp the log with the current time

    NOTES   : buffer must be 11 bytes long
*/
void get_date_stamp_str(time_t raw_time, char *buffer);


#ifdef __cplusplus
}
#endif


#endif // DATE_TIME_FUNCTIONS_H_INCLUDED

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

#ifndef STRING_FUNCTIONS_H_INCLUDED
#define STRING_FUNCTIONS_H_INCLUDED

#define MAX_LST_LINE_LEN 320

#define ASCII_BACKSLASH 47 //used to separate file names from path
#define ASCII_SPACE 32
#define ASCII_UNDERSCORE 95 // used to remove underscores from item names
#define ASCII_HASH 35
#define ASCII_NULL 0


#define MIN_PRINTABLE_ASCII 32
#define MAX_PRINTABLE_ASCII 125

/** RESULT  : adds text and data to string

    RETURNS : void

    PURPOSE : safe snprintf

    NOTES   : stops server if overrun detected
*/
void ssnprintf(char *str, int max_len, char *fmt, ...);


/** RESULT  : trims space from the right of a string

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_trim_right(char *str_in);


/** RESULT  : trims space from the left of a string

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_trim_left(char *str_in);


/** RESULT  : convers characters in string to upper case

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_conv_upper(char *str_in);


/** RESULT  : converts characters in string to lower case

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_conv_lower(char *str_in);


/** RESULT  : extracts a file name from a string containing a full path

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void extract_file_name(char *str_in, char *str_out);


/** RESULT  : converts underscores in a string to spaces

    RETURNS : void

    PURPOSE : code resusability

    NOTES   :
*/
void str_remove_underscores(char *str_in);


/** RESULT  : splits a string up into parts separated by ascii 32 spaces or "" quotes

    RETURNS : void

    PURPOSE : replacement for sscanf function which is unable to read sentences between "" quotes

    NOTES   :
*/
void parse_line(char *line, char output[][MAX_LST_LINE_LEN]);


/** RESULT  : compares upper case of two strings

    RETURNS : as per strcmp function

    PURPOSE :

    NOTES   :
*/
int strcmp_upper(char *str1, char *str2);


#endif // STRING_FUNCTIONS_H_INCLUDED

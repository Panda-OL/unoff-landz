/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

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

#ifndef MAP_OBJECT_H_INCLUDED
#define MAP_OBJECT_H_INCLUDED

#define MAP_OBJECT_TABLE_SQL "CREATE TABLE MAP_OBJECT_TABLE( \
        ID    INTEGER PRIMARY KEY     NOT NULL, \
        THREEDOL_ID   INT, \
        MAP_ID        INT, \
        TILE          INT, \
        E3D_ID        INT, \
        HARVESTABLE   INT, \
        RESERVE       INT)"

/** RESULT  : loads data from the map object table into the map object array

    RETURNS : void

    PURPOSE : retrieve map object data from permanent storage

    NOTES   :
**/
void load_db_map_objects();

#endif // MAP_OBJECT_H_INCLUDED
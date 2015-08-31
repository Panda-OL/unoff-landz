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
#include <stdio.h> // support printf
#include <stdlib.h> //support for NULL
#include <string.h>  //support for strcpy

#include "database_functions.h"
#include "../logging.h"
#include "../guilds.h"
#include "../server_start_stop.h"


void load_db_guilds(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading guilds...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM GUILD_TABLE";

    //check database table exists
    char database_table[80];
    strcpy(database_table, strstr(sql, "FROM")+5);
    if(table_exists(database_table)==false){

        log_event(EVENT_ERROR, "table [%s] not found in database", database_table);
        stop_server();
    }

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //read the sql query result into the guild array
    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int id=sqlite3_column_int(stmt, 0);
        strcpy(guilds.guild[id].guild_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(guilds.guild[id].guild_tag, (char*)sqlite3_column_text(stmt, 2));
        guilds.guild[id].guild_tag_colour=sqlite3_column_int(stmt, 3);
        strcpy(guilds.guild[id].guild_description, (char*)sqlite3_column_text(stmt, 4));
        guilds.guild[id].date_guild_created=sqlite3_column_int(stmt, 5);
        guilds.guild[id].permission_level=sqlite3_column_int(stmt, 6);
        guilds.guild[id].status=sqlite3_column_int(stmt, 7);

        log_event(EVENT_INITIALISATION, "loaded [%i]", id);

        i++;
    }

    //test that we were able to read all the rows in the query result
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);

    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    if(i==0){

        log_event(EVENT_ERROR, "no guilds found in database", i);
        stop_server();
    }
}


int add_db_guild(char *guild_name, char *guild_tag, int guild_tag_colour, char *guild_description,
    int guild_permission_level,
    int guild_status){

    /** public function - see header */

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="INSERT INTO GUILD_TABLE("  \
        "GUILD_NAME," \
        "GUILD_TAG," \
        "GUILD_TAG_COLOUR, " \
        "GUILD_DESCRIPTION," \
        "DATE_GUILD_CREATED," \
        "PERMISSION_LEVEL,"  \
        "STATUS" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?)";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //bind the data
    int date_guild_created=time(NULL);

    sqlite3_bind_text(stmt, 1, guild_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, guild_tag, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, guild_tag_colour);
    sqlite3_bind_text(stmt, 4, guild_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, date_guild_created);
    sqlite3_bind_int(stmt, 6, guild_permission_level);
    sqlite3_bind_int(stmt, 7, guild_status);

    //process the sql statement
    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //find the id of the new entry
    strcpy(sql, "SELECT MAX(GUILD_ID) FROM GUILD_TABLE");

    //prepare the sql statement
    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //process the sql statement
    int id=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        id=sqlite3_column_int(stmt, 0);
    }

    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("Guild [%s] added successfully\n", guild_tag);

    log_event(EVENT_INITIALISATION, "Added guild [%s] to GUILD_TABLE", guild_tag);

    return id;
}

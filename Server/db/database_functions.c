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

#include <stdio.h> //supports snprintf
#include <string.h> //supports strlen

#include "database_functions.h"

#include "db_character_tbl.h"
#include "db_map_tbl.h"
#include "db_character_type_tbl.h"
#include "db_character_race_tbl.h"
#include "db_character_inventory_tbl.h"
#include "db_gender_tbl.h"
#include "db_chat_channel_tbl.h"
#include "db_attribute_tbl.h"
#include "db_game_data_tbl.h"
#include "db_season_tbl.h"
#include "db_object_tbl.h"
#include "db_e3d_tbl.h"
#include "db_map_object_tbl.h"
#include "../global.h"
#include "../server_start_stop.h"
#include "../attributes.h"
#include "../chat.h"
#include "../logging.h"
#include "../file_functions.h"

sqlite3 *db;

int current_database_version();

static int prepare_query(const char *sql, sqlite3_stmt **stmt, const char *_func, int line){

    int rc=sqlite3_prepare_v2(db, sql, -1, stmt, NULL);

    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", _func, __FILE__, line, rc, sql);
        return -1;
    }

    return 0;
}

void open_database(const char *db_filename){

   /** public function - see header **/

    if(file_exists(db_filename)==false){

        printf("database file [%s] not found\n", db_filename);
        log_event(EVENT_ERROR, "database file [%s] not found", db_filename);
        stop_server();
        return;
    }

    int rc = sqlite3_open(db_filename, &db);
    if( rc !=SQLITE_OK ){

        log_sqlite_error("sqlite3_open", __func__ , __FILE__, __LINE__, rc, "");
    }
}

static int column_exists(const char *table, const char *column) {

    sqlite3_stmt *stmt;
    char sql[MAX_SQL_LEN]="";
    int rc;
    int name_column_idx=-1;

    snprintf(sql, MAX_SQL_LEN, "PRAGMA table_info('%s')", table);

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    for (int i=0; i<sqlite3_column_count(stmt); i++) {

        if(strcmp("name", sqlite3_column_name(stmt,i))==0) {
            name_column_idx = i;
            break;
        }
    }

    if(name_column_idx==-1) {
        sqlite3_finalize(stmt);
        return 0; //TODO: log information about strange table_info layout ?
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        if(0==strcmp(column,(const char *)sqlite3_column_text(stmt, name_column_idx))) {
            sqlite3_finalize(stmt);
            return 1;
        }
    }
    sqlite3_finalize(stmt);
    return 0;
}

int database_table_count(){

    /** public function - see header **/

    int rc;
    sqlite3_stmt *stmt;
    int table_count=0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM sqlite_master WHERE type='table';");

    if(-1==prepare_query(sql, &stmt, __func__, __LINE__))
        return 0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        table_count=sqlite3_column_int(stmt, 0);
    }

    rc=sqlite3_finalize(stmt);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return table_count;
}

void create_database_table(char *sql){

    /** RESULT  : creates a database table

       RETURNS  : void

       PURPOSE  : used by function create_new_database
    **/

    int rc;
    sqlite3_stmt *stmt;

    if(-1==prepare_query(sql, &stmt, __func__, __LINE__))
        return;

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, sql);
    }

    //extract table name from sql string
    char table_name[80]="";
    int str_len=strlen(sql);

    int i=0;

    for(i=13; sql[i]!='('; i++){

        table_name[i-13]=sql[i];

        if(i>=str_len) {

            log_event(EVENT_ERROR, "unable to extract table name from sql string [%s] in function %s: module %s: line %i", sql, __func__, __FILE__, __LINE__);
            stop_server();
        }
    }

    log_event(EVENT_INITIALISATION, "Created table [%s]", table_name);
}


void process_sql(const char *sql_str){

    /** public function - see header **/

    int rc=0;
    sqlite3_stmt *stmt;

    if(-1==prepare_query(sql_str, &stmt, __func__, __LINE__))
        return;

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }

    rc=sqlite3_finalize(stmt);
    if(rc != SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }
}

int current_database_version() {

    int rc;

    sqlite3_stmt *selectStmt;
    const char *sql_str = "SELECT db_version FROM GAME_DATA_TABLE";

    if(0==column_exists("GAME_DATA_TABLE","db_version"))
        return 0;

    if(-1==prepare_query(sql_str,&selectStmt,__func__,__LINE__))
        return -1;

    rc = sqlite3_step(selectStmt);
    if (rc == SQLITE_DONE) {
        log_event(EVENT_ERROR,"Database is missing GAME_DATA_TABLE contents.");
        return -1;
    }
    else if (rc != SQLITE_ROW) {
        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }
    else {
        return sqlite3_column_int(selectStmt, 0);
    }
    rc = sqlite3_finalize(selectStmt);

    return 0;
}

void create_default_database(){

    /** public function - see header **/

    //create logical divider in log file
    log_text(EVENT_INITIALISATION, "\nCreating database tables...");

    //create database tables
    create_database_table(CHARACTER_TABLE_SQL);
    create_database_table(INVENTORY_TABLE_SQL);
    create_database_table(GENDER_TABLE_SQL);
    create_database_table(E3D_TABLE_SQL);
    create_database_table(OBJECT_TABLE_SQL);
    create_database_table(MAP_TABLE_SQL);
    create_database_table(CHANNEL_TABLE_SQL);
    create_database_table(RACE_TABLE_SQL);
    create_database_table(CHARACTER_TYPE_TABLE_SQL);
    create_database_table(ATTRIBUTE_TABLE_SQL);
    create_database_table(ATTRIBUTE_VALUE_TABLE_SQL);
    create_database_table(GAME_DATA_TABLE_SQL);
    create_database_table(SEASON_TABLE_SQL);
    create_database_table(MAP_OBJECT_TABLE_SQL);

    // inserts a blank line to create a logical separator with subsequent log entries
    log_text(EVENT_INITIALISATION, "");

    add_db_game_data(1, 27225, 1, 27225, 360);

    add_db_channel(1, 0, CHAN_PERMANENT, "", "Main Channel", "A channel for chatting", 1);

    add_db_race(1, "Human", "tall");
    add_db_race(2, "Dwarf", "short");
    add_db_race(3, "Elf", "short");
    add_db_race(4, "Gnome", "short");
    add_db_race(5, "Orchan", "tall");
    add_db_race(6, "Dragoni", "tall");

    add_db_gender(1, "Male");
    add_db_gender(2, "Female");

    add_db_char_type(0, 1, 2);
    add_db_char_type(1, 1, 1);
    add_db_char_type(2, 3, 2);
    add_db_char_type(3, 3, 1);
    add_db_char_type(4, 2, 2);
    add_db_char_type(5, 2, 1);
    add_db_char_type(37, 4, 2);
    add_db_char_type(38, 4, 1);
    add_db_char_type(39, 5, 2);
    add_db_char_type(40, 5, 1);
    add_db_char_type(41, 6, 2);
    add_db_char_type(42, 6, 1);

    add_db_season(0, "Winter", "A Winter season", 0, 90);
    add_db_season(1, "Alddra", "A Spring season", 90, 180);
    add_db_season(2, "Kiwi", "An Summer season", 180, 270);
    add_db_season(3, "Butler", "An Autumn season", 270, 360);

    int i=0, j=0, k=0, l=0;
    float attribute_value;

    for(i=1; i<=6; i++){

        j++;
        add_db_attribute(j, "day vision", i, ATTR_DAY_VISION);

        attribute_value=10.0f;

        for(k=1; k<=50; k++){

            add_db_attribute_value (l, j, ATTR_DAY_VISION, k, attribute_value);
            attribute_value=attribute_value + 0.20f;
            l++;
        }
    }

    for(i=1; i<=6; i++){

        j++;
        add_db_attribute(j, "night vision", i, ATTR_NIGHT_VISION);

        attribute_value=10.0f;

        for(k=1; k<=50; k++){

            add_db_attribute_value (l, j, ATTR_NIGHT_VISION, k, attribute_value);
            attribute_value=attribute_value + 0.20f;
            l++;
        }
    }

    for(i=1; i<=6; i++){

        j++;
        add_db_attribute(j, "carry capacity", i, ATTR_CARRY_CAPACITY);

        attribute_value=100.0f;

        for(k=1; k<=50; k++){

            add_db_attribute_value (l, j, ATTR_CARRY_CAPACITY, k, attribute_value);
            attribute_value=attribute_value + 18.0f;
            l++;
        }
    }

    add_db_e3d(1, "cabbage.e3d", 405);
    add_db_e3d(2, "tomatoeplant1.e3d", 407);
    add_db_e3d(3, "tomatoeplant2.e3d", 407);
    add_db_e3d(4, "foodtomatoe.e3d", 407);
    add_db_e3d(5, "food_carrot.e3d", 408);
    add_db_e3d(8, "branch1.e3d", 140);
    add_db_e3d(9, "branch2.e3d", 140);
    add_db_e3d(10, "branch3.e3d", 140);
    add_db_e3d(11, "branch4.e3d", 140);
    add_db_e3d(12, "branch5.e3d", 140);
    add_db_e3d(13, "branch6.e3d", 140);
    add_db_e3d(15, "flowerorange1.e3d", 29);
    add_db_e3d(16, "flowerorange2.e3d", 29);
    add_db_e3d(17, "flowerorange3.e3d", 29);

    add_db_object(405, "cabbage", 1, 1, 2);
    add_db_object(407, "tomato", 1, 1, 2);
    add_db_object(408, "carrot", 1, 1, 2);
    add_db_object(140, "stick", 1, 0, 2);
    add_db_object(29, "tiger lily", 1, 0, 1);

/*
    add_db_object(1, "cabbage.e3d", "cabbage", 405, 1, 1);
    add_db_object(2, "tomatoeplant1.e3d", "tomato", 407, 1, 1);
    add_db_object(3, "tomatoeplant2.e3d", "tomato", 407, 1, 1);
    add_db_object(4, "foodtomatoe.e3d", "tomato", 407, 1, 1);
    add_db_object(5, "food_carrot.e3d", "carrot", 408, 1, 1);
    add_db_object(6, "log1.e3d", "log", 408, 1, 0);
    add_db_object(7, "log2.e3d", "log", 408, 1, 0);
    add_db_object(8, "flowerpink1.e3d", "Chrysanthemum", 28, 1, 0);
    add_db_object(9, "branch1.e3d", "stick", 140, 1, 0);
    add_db_object(10, "branch2.e3d", "stick", 140, 1, 0);
    add_db_object(11, "branch3.e3d", "stick", 140, 1, 0);
    add_db_object(12, "branch4.e3d", "stick", 140, 1, 0);
    add_db_object(13, "branch5.e3d", "stick", 140, 1, 0);
    add_db_object(14, "branch6.e3d", "stick", 140, 1, 0);
    add_db_object(15, "flowerorange1.e3d", "Tiger Lily", 29, 1, 0);
    add_db_object(16, "flowerorange2.e3d", "Tiger Lily", 29, 1, 0);
    add_db_object(17, "flowerorange3.e3d", "Tiger Lily", 29, 1, 0);
    add_db_object(18, "flowerwhite1.e3d", "Impatiens", 29, 1, 0);
    add_db_object(19, "flowerwhite2.e3d", "Impatiens", 29, 1, 0);
    add_db_object(20, "flowerwhite3.e3d", "Impatiens", 29, 1, 0);
*/

    add_db_map(1, "Isla Prima", "startmap.elm");
    add_db_map_objects("startmap.elm", 1);
}

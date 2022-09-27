#include "sqlite3.h"
#include <stdio.h>

int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS Power;" 
                "CREATE TABLE Power(Id INTEGER PRIMARY KEY, Current DOUBLE, Voltage DOUBLE);" 
                "INSERT INTO Power VALUES(NULL,1.56, 53656.5);" 
                "INSERT INTO Power VALUES(NULL,2.60, 23658.5);" 
                "INSERT INTO Power VALUES(NULL,0.80, 23766.7);" 
                "INSERT INTO Power VALUES(NULL,0.66, 23660.5);" 
                "INSERT INTO Power VALUES(NULL,0.68, 23662.5);" 
                "INSERT INTO Power VALUES(NULL,0.72, 23666.8);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}
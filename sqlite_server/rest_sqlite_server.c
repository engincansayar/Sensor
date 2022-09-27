#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "sqlite3.h"
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define MAX 2048
#define LIMIT 100

void *sensor_read(void *p_client_sc)
{
    pthread_mutex_lock(&lock);
    char *check;
    char *selection = "Failed to select data\n";
    char *open = "Cannot open database:";
    char http_header[MAX] = "HTTP/1.1 200 OK\r\n\n";
    char *get = "GET";
    char *post = "POST";
    char *delete = "DELETE";
    char *put = "PUT";
    char *post_message = "\nInput values successfully added to the file\n";
    char *put_message = "\nPut operation is not available.\n";
    char *delete_message = "\nThe content of the file is erased.\n";
    FILE *sensor_data;
    char buffer[MAX];
    char writer[MAX]="";
    char head[] = "#,";
    char end[] = ",0!";
    int client_sc = *((int *)p_client_sc);
    size_t size = read(client_sc,buffer,sizeof(buffer));
    printf("%s",buffer);
    check = strtok(buffer," ");
    if(!strcmp(check,get))
    {
        
        sqlite3 *db;
        char *err_msg = 0;
        sqlite3_stmt *res;
    
        int rc = sqlite3_open("test.db", &db);
    
        if (rc != SQLITE_OK) {
        
            fprintf(stderr, "Cannot open database: %s\n", 
                    sqlite3_errmsg(db));
            sqlite3_close(db);

            strcat(http_header,open);
            int len = strlen(http_header) - 1;
            write(client_sc, http_header, len);
            fclose(sensor_data);
            close(client_sc);
            free(p_client_sc);
            fflush(stdout);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        
            //return 1;
        }
    
        char *sql = "SELECT * FROM Power";
        char hold[LIMIT] = "";
        
        rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
        if (rc != SQLITE_OK ) {
        
            //fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
            sqlite3_close(db);
            strcat(http_header,selection);
            int len = strlen(http_header) - 1;
            write(client_sc, http_header, len);
            close(client_sc);
            free(p_client_sc);
            fflush(stdout);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        
            //return 1;
        } 
        int last;
        char column[LIMIT];
        int j =0;
        for(int i=1; i <=LIMIT; i++){

            if (rc == SQLITE_OK) {
        
                sqlite3_bind_int(res, 1, i);
            } 
            else {
        
                fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
            }
    
            int step = sqlite3_step(res);
    
            if (step == SQLITE_ROW) {
                sprintf(column,"%s",sqlite3_column_text(res, 0));
                last = atoi(column);
                if(last == 1){
                    if(j == 1){
                        break;
                    }
                    j++;

                }
                sprintf(hold,"\n%s. Current Value:%s\t%s.Voltage Value:%s\n",sqlite3_column_text(res, 0),sqlite3_column_text(res, 1),sqlite3_column_text(res, 0),sqlite3_column_text(res, 2));
                /*printf("\n%s. ", sqlite3_column_text(res, 0));
                printf("%s\t", sqlite3_column_text(res, 1));
                printf("%s. ", sqlite3_column_text(res, 0));
                printf("%s\n", sqlite3_column_text(res, 2));*/
                strcat(writer,hold);
            } 
        }
        sqlite3_finalize(res);
        sqlite3_close(db);
        strcat(http_header,writer);
        int len = strlen(http_header) - 1;
        write(client_sc, http_header, len);
        close(client_sc);
        free(p_client_sc);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
    if(!strcmp(check,post))
    {
        double current_lf = 0;
        char current[LIMIT] ="";
        double voltage = 0;
        char *new;
        const char equal = '=';
        char value[LIMIT] ="";
        int j = 0;
        
        for(int i=0;i<size;i++){
            if(buffer[i]==' '){
                //strcat(value,end);
                break;
            }
            if(buffer[i]==','){
                strcat(current,value);
                //char value[LIMIT] ="";
                current_lf = atof(current);
                j = 0;
                continue;

            }
            if((buffer[i]=='0') || (buffer[i]=='1') || (buffer[i]=='2') || (buffer[i]=='3') || (buffer[i]=='4') || (buffer[i]=='5') || (buffer[i]=='6') || (buffer[i]=='7') || (buffer[i]=='8') || (buffer[i]=='9') || (buffer[i]=='.'))
            {
                value[j] = buffer[i];
                j++;
            }
            
        }
        voltage = atof(value);
        
        sqlite3 *db;
        char *err_msg = 0;
    
        int rc = sqlite3_open("test.db", &db);

        if (rc != SQLITE_OK) {
        
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            strcat(http_header,open);
            sqlite3_free(err_msg);        
            sqlite3_close(db);
            strcat(http_header,post_message);
            int len = strlen(http_header);
            write(client_sc, http_header, len);
            close(client_sc);
            free(p_client_sc);
            fflush(stdout);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        }
        //char *sql = "INSERT INTO Power(Id,Current,Voltage) VALUES(NULL,current_lf, voltage);";
        char sql[MAX];
        sprintf(sql,"INSERT INTO Power(Id,Current,Voltage) VALUES(NULL,%lf, %lf)",current_lf,voltage);


        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
        if (rc != SQLITE_OK ) {
        
            fprintf(stderr, "SQL error: %s\n", err_msg);
            strcat(http_header,selection);
            sqlite3_free(err_msg);        
            sqlite3_close(db);
            int len = strlen(http_header);
            write(client_sc, http_header, len);
            close(client_sc);
            free(p_client_sc);
            fflush(stdout);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
    
            //return 1;
        } 
    
        sqlite3_close(db);
    
        //return 0;

        strcat(http_header,post_message);
        int len = strlen(http_header);
        write(client_sc, http_header, len);
        close(client_sc);
        free(p_client_sc);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
    if(!strcmp(check,delete))
    {
        sqlite3 *db;
        char *err_msg = 0;
        sqlite3_stmt *res;
    
        int rc = sqlite3_open("test.db", &db);
    
        if (rc != SQLITE_OK) {
        
            fprintf(stderr, "Cannot open database: %s\n", 
                    sqlite3_errmsg(db));
            sqlite3_close(db);

            strcat(http_header,open);
            int len = strlen(http_header) - 1;
            write(client_sc, http_header, len);
            close(client_sc);
            free(p_client_sc);
            fflush(stdout);
            pthread_mutex_unlock(&lock);
            pthread_exit(NULL);
        
            //return 1;
        }
        char *sql = "UPDATE sqlite_sequence SET seq = 0 WHERE name = Power";
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        sqlite3_close(db);
        int len = strlen(http_header) - 1;
        write(client_sc, http_header, len);
        close(client_sc);
        free(p_client_sc);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);

    }
    if(!strcmp(check,put)){
        strcat(http_header,put_message);
        int len = strlen(http_header) - 1;
        write(client_sc, http_header, len);
        close(client_sc);
        free(p_client_sc);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
}
int main()
{
    char buffer[MAX];
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // define the address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = INADDR_ANY;
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);
    while (1)
    {
        int client_socket;
        client_socket = accept(server_socket, NULL, NULL);
        pthread_t thr;
        int *p_client_socket = malloc(sizeof(int));
        *p_client_socket = client_socket;
        pthread_create(&thr, NULL, sensor_read, (void *)p_client_socket);
        pthread_detach(thr);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define MAX 2048
#define LIMIT 100
void *sensor_read(void *p_client_sc)
{
    pthread_mutex_lock(&lock);
    char *check;
    char *new;
    char *d;
    char http_header[MAX] = "HTTP/1.1 200 OK\r\n\n";
    char *get = "GET";
    char *post = "POST";
    char *delete = "DELETE";
    char *put = "PUT";
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
        sensor_data = fopen("//home//mint//Desktop//rest_server//sensor_data.txt", "r");
        fread(writer,size+1,1,sensor_data); 
        strcat(http_header,writer); 
        int len = strlen(http_header) - 1;
        write(client_sc, http_header, len);
        fclose(sensor_data);
        close(client_sc);
        free(p_client_sc);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
    if(!strcmp(check,post))
    {
        sensor_data = fopen("//home//mint//Desktop//rest_server//sensor_data.txt", "a");
        const char equal = '=';
        char value[LIMIT] = "";
        new = strchr(buffer,equal);
        size_t length = strlen(new);
        for(int i=0;i<length;i++){
            if(new[i]=='='){
                continue;
            }
            if(new[i]==' '){
                strcat(value,end);
                break;
            }
            value[i-1] = new[i];
        }
        strcat(writer,head);
        strcat(writer,value);
        fwrite(writer,1,size,sensor_data);
        fclose(sensor_data);
        int len = strlen(http_header) - 1;
        write(client_sc, http_header, len);
        close(client_sc);
        free(p_client_sc);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
    if(!strcmp(check,delete))
    {
        strcat(http_header,delete_message);
        fclose(fopen("//home//mint//Desktop//rest_server//sensor_data.txt", "w"));
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
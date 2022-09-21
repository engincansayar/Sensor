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
void *sensor_read(void *p_client_sc)
{
    char *line;
    FILE *sensor_data;
    FILE *sensor_data_1;
    char buffer[MAX];
    char writer[MAX];
    char head[3] = "#,";
    char end[3] = ",0!";
    int client_sc = *((int *)p_client_sc);
    read(client_sc,buffer,sizeof(buffer));
    printf("%s",buffer);
    sensor_data_1 = fopen("//home//mint//Desktop//rest_server//sensor_data.txt", "w");
    const char equal = '=';
    const char space = ' ';
    char value[100];
    char *new;
    new = strrchr(buffer,equal);
    int length = strlen(new);
    for(int i=0;i<length;i++){
        if(new[i]=='='){
            continue;
        }
        if(new[i]==' '){
            break;
        }
        value[i-1] = new[i];
    }
    strcat(writer,head);
    strcat(writer,value);
    strcat(writer,end);
    fprintf(sensor_data_1,"%s",writer);
    fclose(sensor_data_1);
    char http_header[MAX] = "HTTP/1.1 200 OK\r\n\n";
    char written_data[MAX];
    const char test = '!';
    int choose = 0;
    double current;
    double voltage;
    double watt;
    char counter_str[10];
    char current_str[50];
    char voltage_str[50];
    char power_str[50];
    const char ch_1 = '<';
    const char ch_2 = '/';
    int count = 0;
    const char ins = '#';
    pthread_mutex_lock(&lock);
    sensor_data = fopen("//home//mint//Desktop//rest//sensor_data.txt", "r");
    int counter = 1;
    while((line = fgets(written_data,MAX, sensor_data)) != 0){
        char *segment;
        segment = strtok(line,",");
        while(1){
            if(strchr(segment,ins))
            {
                segment = strtok(NULL,",");
                continue;
            }
            if(strchr(segment,test))
            {
                segment = strtok(NULL,"0!\n");
                break;
            }
            else if(choose % 2 == 0)
            { 
                current = atof(segment); 
                choose++;
                sprintf(current_str, "\n%d. current value: %.2lf\t", counter, current);
                strcat(http_header, current_str);  
                segment = strtok(NULL,",");
                continue;
            }
            else
            {
                voltage = atof(segment);
                sprintf(voltage_str, "%d. voltage value: %.2lf\t", counter, voltage);
                strcat(http_header, voltage_str);
                choose++;
                watt = current * voltage;
                sprintf(power_str, "%d. power is equal to %.2lf\n", counter, watt);
                strcat(http_header, power_str);
                counter++;
                //printf("\n%s\n", http_header);
                segment = strtok(NULL,",");
                continue;
            }
            segment = strtok(NULL,",");
            continue;
        }
        continue;
    }
    pthread_mutex_unlock(&lock);
    int len = strlen(http_header) - 1;
    write(client_sc, http_header, len);
    close(client_sc);
    free(p_client_sc);
    fclose(sensor_data);
    fflush(stdout);
    pthread_exit(NULL);
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
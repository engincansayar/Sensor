#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <netdb.h>
#include <signal.h>
#define PORT 8001
#define MAX_LINE 1000
#define MAX 2048
int main(){
    char buffer[MAX];
    char hello[MAX] = "\nHello Server\n";
    int sockfd;
    int size;
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_family = AF_INET;
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
    FILE *sensor_data;
    size = sizeof(hello);
    write(sockfd,hello, size);
    size=read(sockfd, buffer, sizeof(buffer));
    sensor_data = fopen("//home//mint//Desktop//sensor_client//data.txt","w");
    fwrite(buffer,1,size,sensor_data);
    puts(buffer);
    fclose(sensor_data);
    close(sockfd);
}
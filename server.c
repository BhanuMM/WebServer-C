#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>


int main(int argc, char *argv[])
{
struct sockaddr_in server_addr, client_addr;

socklen_t sin_len = sizeof (client_addr);

int fd_server, fd_client;

FILE *html_data;

char buf [2048];
int fding;
int on =1; 

fd_server = socket (AF_INET, SOCK_STREAM, 0);

if(fd_server < 0)
{
perror("socket"); 
exit(1);
}

setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr =INADDR_ANY;
server_addr.sin_port = htons (8080);

if (bind(fd_server, (struct sockaddr *) &server_addr, sizeof (server_addr))== -1)
{
    perror ("bind"); 
    close(fd_server); 
    exit(1);
}

if(listen (fd_server, 10) == -1)
{
    perror("listen");
    close(fd_server);
    exit(1);
}

while (1)
{
    fd_client= accept (fd_server, (struct sockaddr *) &client_addr, &sin_len);

    if(fd_client == -1) {
        perror("Connection failed\n");
        continue;
    }

    printf("Client connection Established.\n");

    if(!fork())
    {
        
        close(fd_server);
        memset (buf, 0, 2048);
        read(fd_client, buf, 2047);
        printf("%s\n", buf);
    
    if(!strncmp(buf, "GET /favicon.ico", 16))
    {
        fding = open("favicon.ico", O_RDONLY);
        sendfile(fd_client, fding, NULL, 4000);
        close(fding);
    }

    else if(!strncmp (buf, "GET /ucsc.png", 13))
    {
        fding = open("webserver.png", O_RDONLY); 
        sendfile(fd_client, fding, NULL, 34000); 
        close(fding);
    }
    else if(!strncmp (buf, "GET /music.mp3", 14))
    {
        fding = open("music.mp3", O_RDONLY); 
        sendfile(fd_client, fding, NULL, 34000); 
        close(fding);
    }
    else if (!strncmp (buf, "GET /about.html", 15))
    {
        
        html_data = fopen("about.html","r");
        char response_data[1024];
        fgets(response_data,1024,html_data);
        char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
        strcat(http_header,response_data);
        write(fd_client, http_header, sizeof(http_header) - 1); 
        
    }
     else if (!strncmp (buf, "GET /404.html", 13))
    {
        
        html_data = fopen("404.html","r");
        char response_data[1024];
        fgets(response_data,1024,html_data);
        char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
        strcat(http_header,response_data);
        write(fd_client, http_header, sizeof(http_header) - 1); 
        
    }
    else 
     html_data = fopen("index.html","r");
        char response_data[1024];
        fgets(response_data,1024,html_data);
        char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
        strcat(http_header,response_data);
        write(fd_client, http_header, sizeof(http_header) - 1); 
        
    close (fd_client);
    printf("closing...\n");
    exit(0);
}

 close(fd_client);
}
return 0;
}
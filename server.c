//A Simple web server written in C
// I am using tons of libraries instead to handle sockets.
// This server will just serve HTML
#include "headerFiles.h"

// variables
//webPage variable will store the simple webpage file
// First two lines are the header sent to browser
// r/n just return and go the new lines. 


char webPage[] = 
"HTTP/1.1. 200 OK\r\n"
"Content-Type:text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>C Server</title>\r\n"
"<style>body {background-color: #b2b2b2}</style></head>\r\n"
"<body><center><h1>Hello World!</h1><br>\r\n"
"<img src=\"clang.jpg\"></center></body></html>\r\n";

int setsocket();
void file_exchange(int source, int destination,int chuck_size);

int main(int argc, char *argv[])
{
    //handling the sockaddress for client and server
    struct sockaddr_in server_address, client_address;
    // Need the length of socket 
    socklen_t sin_len = sizeof(client_address);
    
    //file descriptor, return value from socket function
    int fd_server, fd_client;

    // buffer characters, stores the content sent by server

    char buf[2048];

    //file descriptor for the all files. 
    int fdimg;
    off_t file_pointer;

    int on = 1;

    /// INITIALIZATION ///

    //implementing the functions
    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_server < 0) {
        perror("Socket Error!");
        exit(1);
    }

    // set the socket instance if the socket exists.
    // look socket.h for detailed module description.
    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    //server setup 
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080); // if you go 1042 or below, you need sudo

    // setup error handling and binding
    if(bind( fd_server, (struct sockaddr *) &server_address, sizeof(server_address)) == -1){
        perror("Binding and Setup Error");
        close(fd_server);
        exit(1);
    }

    // Listen for the request
    if( listen(fd_server, 10) == -1 ){

        perror("Listening Error!");
        close(fd_server);
        exit(1);

    }

    /// END OF INITIALIZATION //

    //serving to client 
    while (1) {
        fd_client = accept(fd_server, (struct sockaddr *) &client_address, &sin_len);

        if (fd_client == -1) {
            perror("Connection Failed!  \n");
            continue;
        }

        printf("Successful clinet connection");

        //creating multiple processes 
        if(!fork()) {

            // child process
            close(fd_server);
            
            //clear the buffer
            memset(buf, 0, 2048);
            //read the buffer
            read(fd_client, buf, 2047);
            printf("%s\n",buf);

            //serving the favicon manually
            if(!strncmp(buf, "GET /favicon.ico", 16)) {
                fdimg = open("favicon.ico", O_RDONLY);
                //sendfile(fd_client, fdimg,NULL, 4000);
                
                file_exchange(fdimg, fd_client, 20);
                close(fdimg);

            }
            else if(!strncmp(buf, "GET /clang.jpg", 16)){

                fdimg = open("clang.jpg", O_RDONLY);
                //sendfile(fd_client, fdimg,NULL, 6000);
               
                file_exchange(fdimg, fd_client, 20);
                close(fdimg);

            }
            else {

            // Sent the webpage to client.
            write(fd_client, webPage, sizeof(webPage)-1);
            printf("Closing!!");
            exit(0);
            }


        }

        //parent process
        close(fd_client);

    }


    
    return 0;
}


// writes file from one descriptor to another.
void file_exchange(int source, int destination,int chuck_size) {
    char buff[6000];
    int ret;
    while ((ret = read(source, buff, chuck_size)) > 0) {
        if (write(destination, buff, ret) < 0) {
            perror("File Exchange Error! \n");
            exit(1);
        }
    }
}
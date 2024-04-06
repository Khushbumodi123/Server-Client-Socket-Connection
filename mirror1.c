// Mirror is a copy of server. It is used for load balancing. If the server is overloaded, the client can connect to the mirror.

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

// global variables
int clientdecsriptor;
int count = 0;
char client_rqst[1024];

int main(int argc, char *argv[])
{
    // declaring variables for port number
    int listen_socket, port_no;               // variables for listening socket and port number
    socklen_t length = sizeof(serveraddress); // to store length of socket address
    struct sockaddr_in serveraddress;         // to store server address information
    int opt = 1;                              // to store option value

    // this will check is required arguments are given or not, if it is not given then it gives error
    if (argc != 2)
    {
        fprintf(stderr, "Correct syntax for command: %s <Port number>\n", argv[0]);
        exit(0);
    }

    // this if block is responsible for creating socket and if it is not created successfully then it gives error ane exit from here
    //  AF_INET -> for internet socket, SOCK_STREAM -> type of socket here it is TCP socket, last argument is for protocol ,0 -> default protocol
    if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "Socket is not created\n");
        exit(0);
    }

    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(0);
    }
    // specifying address structure
    serveraddress.sin_family = AF_INET;                // specifying family
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY); // for any address
    sscanf(argv[1], "%d", &port_no);                   // reads the port number and convert it to the integert
    serveraddress.sin_port = htons((uint16_t)port_no); // specifying port number, here we have to use conversion function because data format is different, int to network byteorder

    if (bind(listen_socket, (struct sockaddr *)&serveraddress, sizeof(serveraddress)) < 0)
    {
        printf("bind failed");
        exit(0);
    } // Binds the socket to the specified port and address

    if (listen(listen_socket, 10) < 0)
    {
        printf("listen");
        exit(0);
    } // start listening on socket with a backlog of 10

    for (;;)
    {

        // connection accepting from client
        clientdecsriptor = accept(listen_socket, (struct sockaddr *)NULL, NULL);
        if (clientdecsriptor < 0)
        {
            printf("accept failed");
            exit(0);
        }
        // fork new child process and call pclientrequest()
        if (fork() == 0)
        {
            close(listen_socket);
            pclientrequest(clientdecsriptor);
            exit(0);
        }
        else
        {
            close(clientdecsriptor);
        }
    }

    return 0;
}
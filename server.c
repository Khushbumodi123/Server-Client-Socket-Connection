// Server code contains the basic system calls which include socket(),listen(),bind(),accept().
// The basic server code to handle the client request and forward it to the mirror server if the server is overloaded.This is for load balancing.

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
#include <netinet/in.h>

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

    int serv_or_mirror = 0; // variable to manipulate client request between server and mirror
    for (;;)
    {
        // ready for accepting connection from client
        clientdecsriptor = accept(listen_socket, (struct sockaddr *)NULL, NULL);
        if (clientdecsriptor < 0)
        {
            printf("error connecting\n");
            exit(1);
        }
        // incrmenting count
        count++;
        printf("No. of connection(s) established : %d\n", count);
        // condition to check server and mirror will execute
        //   mirror1
        if ((count > 3 && count < 7) || (count > 10 && count % 2 == 0))
        {
            printf("server overload\n");
            serv_or_mirror = 1;
            // forwarding serv_or_mirror flag to client
            send(clientdecsriptor, &serv_or_mirror, sizeof(serv_or_mirror), 0);
            // printf("Informed Client about overload.\n");
            // cconnection closed with client
            close(clientdecsriptor);
        }
        // mirror2
        else if ((count > 6 && count < 10) || (count > 10 && count % 2 == 0))
        {
            printf("server overload\n");
            serv_or_mirror = 2;
            // forwarding serv_or_mirror flag to client
            send(clientdecsriptor, &serv_or_mirror, sizeof(serv_or_mirror), 0);
            // printf("Informed Client about overload.\n");
            // cconnection closed with client
            close(clientdecsriptor);
        }
        // server
        else
        {
            serv_or_mirror = 0; // setting flag to zero
            // forwarding serv_or_mirror flag to clien
            send(clientdecsriptor, &serv_or_mirror, sizeof(serv_or_mirror), 0);
            // new child process creation
            if (fork() == 0)
            {
                close(listen_socket); // closing listening socket
                // handling request from server
                pclientrequest(clientdecsriptor);
                exit(0);
            }
        }

    } // end of while

    return 0;
} // end of main

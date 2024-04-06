// Client code implements basic system calls including socket() and connect()
// This program establishes a TCP connection to a server or mirror, facilitating client requests transmission.
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

// Global variables declaration
// for storing socket descriptors,a flag to identify whether the program is acting as a server or a mirror.

int serverDes = 0;          // Socket descriptor for the server
int mirrorDes = 0;          // Socket descriptor for the mirror
int process_serv_or_mirror; // Variable to store the active socket descriptor (server or mirror)
char message[1024];         // Character array to store messages

// Main function
int main(int argc, char *argv[])
{
    int port_no;
    struct sockaddr_in server_address;
    // check for number of command-line arguments
    if (argc != 4)
    {
        printf("Enter the command in the proper format :%s <IP> <server Port#> <mirror Port#>\n", argv[0]);
        exit(0);
    }

    // Creates a socket for the server
    if ((serverDes = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Socket not created\n");
        exit(1);
    }
    // sets up server address information
    server_address.sin_family = AF_INET;
    sscanf(argv[2], "%d", &port_no);
    server_address.sin_port = htons((uint16_t)port_no);

    // inet_pton - convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) < 0)
    {
        fprintf(stderr, "inet_pton() has failed\n");
        exit(2);
    }
    // and finally connect to server
    if (connect(serverDes, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "connect not successful\n");
        exit(3);
    }

    // "connecting  to mirror "
    int mirror_port_number;
    struct sockaddr_in mirror_address;
    // Creates a socket for the mirror
    if ((mirrorDes = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Mirror socket not created.\n");
        exit(1);
    }
    // sets up mirror address information
    mirror_address.sin_family = AF_INET;
    sscanf(argv[3], "%d", &mirror_port_number);
    mirror_address.sin_port = htons((uint16_t)mirror_port_number);

    if (inet_pton(AF_INET, argv[1], &mirror_address.sin_addr) < 0)
    {
        fprintf(stderr, "inet_pton() not successful\n");
        exit(2);
    }
    // connects to the mirror.
    if (connect(mirrorDes, (struct sockaddr *)&mirror_address, sizeof(mirror_address)) < 0)
    {
        perror("Error Connecting to mirror");
        exit(1);
    }

    //  function to determine whether to use the server or mirror socket.
    config_serv_or_mirror();

    while (1)
    {
        // receive message from server or mirror
        recv(process_serv_or_mirror, message, sizeof(message), 0);
        printf("Message received: %s\n", message);

        // send message to server or mirror
        send(process_serv_or_mirror, message, sizeof(message), 0);
    }
    return 0;
}
// checks  whether the program is acting as a server or mirror based on the received flag from the server.
void config_serv_or_mirror()
{
    int serv_or_mirror;

    // receive serv_or_mirror flag from server
    recv(serverDes, &serv_or_mirror, sizeof(int), 0);
    if (serv_or_mirror)
    {
        // set to mirror socket
        process_serv_or_mirror = mirrorDes;
    }
    else
    {
        // set to server socket
        process_serv_or_mirror = serverDes;
    }
}

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <map>
#include <iostream>
#include "parsingmachine.h"
#include "connection.h"

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;

int Connection::connect_socket(string address_line, int server_port)
{
    int peer_socket;
    struct sockaddr_in serv_addr;

    char *server_address = new char[address_line.length() + 1];
    strcpy(server_address, address_line.c_str());

    if ((peer_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error!\n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid address! This IP Address is not supported!\n");
        return -1;
    }

    if (connect(peer_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed: Can't establish a connection over this socket!\n");
        return -1;
    }

    return peer_socket;
}


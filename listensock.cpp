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
#include "listensock.h"
#include "peersock.h"

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;


ListenSock::ListenSock(string ip_address, int port)
{
    this->ip_address = ip_address;
    this->port = port;
    this->type = 0;
}
//Handler to accept peer connection
int ListenSock::handler(map<int, Connection*> &connections, vector<string> &peers, int fd, ParsingMachine &parser)
{
    int new_peer = accept(fd, (struct sockaddr *)&address, &address_length);
    if (new_peer >= 0)
    {
        printf("New incoming connection - %d\n", new_peer);
        inet_ntop(AF_INET, &(address.sin_addr), peer_address, INET_ADDRSTRLEN);
        peer_port = address.sin_port;
        connections[new_peer] = new PeerSock(peer_address, peer_port, 1);
    }
    else
    {
        perror("Client accept() failed");
    }

        return 1;
}

string ListenSock::print_address()
{
    return ip_address + ":" + std::to_string(port);
}

int ListenSock::get_type()
{
    return type;
}

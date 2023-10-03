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
#include "peersock.h"

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;


PeerSock::PeerSock(string ip_address, int port, int type)
{
    this->ip_address = ip_address;
    this->port = port;
    this->type = type;
}

//Handler to receive message from peer
int PeerSock::handler(map<int, Connection*> &connections, vector<string> &peers, int fd, ParsingMachine &parser)
{
    parser.parse(1, fd);
    return 1;
}

string PeerSock::print_address()
{
    return ip_address + ":" + std::to_string(port);
}

int PeerSock::get_type()
{
    return type;
}


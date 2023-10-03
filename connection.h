#ifndef connection_h
#define connection_h

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

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;

class Connection
{
public:
    virtual int handler(map<int, Connection*> &connections, vector<string> &peers, int fd, ParsingMachine &parser) = 0;
    virtual string print_address() = 0;
    virtual int get_type() = 0;
    static int connect_socket(string address_line, int server_port);
};

#endif

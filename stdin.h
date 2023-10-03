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

class StdIn : public Connection
{
private:
public:
    int handler(map<int, Connection*> &connections, vector<string> &peers, int fd, ParsingMachine &parser);
    string print_address();
    int get_type();
};
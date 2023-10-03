#ifndef parsingmachine_h
#define parsingmachine_h

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
#include <algorithm>

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;

class Connection;

class ParsingMachine
{
private:
    enum State                         
    {
        INIT, SIZE, TYPE, MESSAGE
    };

    State myState;
    char message_size[2];
    char message_type[1];
    char* message;
    map<int, Connection*> *connections;
    vector<string> *peers;
    int fd, size, length, action, type, processed, sofar, peer_status;
    
public:

    ParsingMachine(map<int, Connection*> &connections, vector<string> &peers);
    void init();
    void parse(int action, int fd, int type = 0, string content = "");
    int parse_send(string content);
    int parse_recv();
    void tran(State target);
    void parsing_size();
    void parsing_type();
    void parsing_message();
    void send_bytes(char *message);
    void receive_bytes(char *message);
    void close_socket();
    void update_peers();
    void dispatch(State state);
};


#endif

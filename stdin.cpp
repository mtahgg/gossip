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
#include "stdin.h"
#include "peersock.h"

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;

//Handler to take std input
int StdIn::handler(map<int, Connection*> &connections, vector<string> &peers, int fd, ParsingMachine &parser)
{
    int peer_socket;
    string line;
    getline(cin, line);
    string command = line.substr(0, line.find(" "));
    
    if (command.compare("connect") == 0) //Connect a peer by address:port
    {
        string address_port = line.substr(line.find(" ") + 1);
        string address = address_port.substr(0, address_port.find(':'));
        int port = stoi(address_port.substr(address_port.find(':') + 1, address_port.length() - 1));
        peer_socket = connect_socket(address, port);
        if (peer_socket == -1)
        {
            perror("  Coonnect to peer server failed");
        }
        else
        {
            connections[peer_socket] = new PeerSock(address, port, 0);
            peers.push_back(address_port);
            printf("  New outgoing connection - %d\n", peer_socket);
        }
    }
    else if (command.compare("send") == 0) //Send a message to all peers
    {
        string content = line.substr(line.find(" ") + 1);
        
        printf("Sending message...\n");
        auto it = connections.begin();
        std::advance(it, 2); 
        while (it != connections.end())
        {
            if(it->second->get_type()==0){
                parser.parse(0, it->first, 1, content);
            }
            ++it;
        }
        printf("Message has been sent!\n");
    }
    else if (command.compare("connection") == 0) //Generate a table of descriptor and all connections
    {
        printf("fd | Address:Port\n");
        for (auto it = ++connections.begin(); it != connections.end(); it++)
        {
            printf("%d | %s\n", it->first, it->second->print_address().c_str());
        }
    }
    else if (command.compare("peer") == 0) //Generate a table of only Outgoing peer connections
    {
        printf("Peer Address:Port\n");
        for (auto elem : peers)
        {
            printf("%s\n", elem.c_str());
        }
    }
    else if (command.compare("exit") == 0)
    {
        printf("  Terminating service.\n");
        return 0;
    }
    else
    {
        printf("  Not a valid command. Below are valid command:\n  connect address:port\n  send message\n  connection\n  peer\n exit\n");
    }

    return 1;
}

string StdIn::print_address(){}
int StdIn::get_type(){}


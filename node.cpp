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
#include <chrono>
#include <ctime>
#include "parsingmachine.h"
#include "peersock.h"
#include "listensock.h"
#include "stdin.h"

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;
using namespace std::chrono;


#define SERVER_IP "172.22.70.223"
#define PORT 18080

class Connection;

//Functions to add StdIn and Listening Socket into connections
void init_connections(map<int, Connection*> &connections)
{
    int server;
    struct sockaddr_in address;
    char server_address[INET_ADDRSTRLEN];
    int port;
    int opted = 1;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Opening of Socket Failed !");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR,
                   &opted, sizeof(opted)))
    {
        perror("Can't set the socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("Binding of socket failed !");
        exit(EXIT_FAILURE);
    }

    if (listen(server, 3) < 0)
    {
        perror("Can't listen from the server !");
        exit(EXIT_FAILURE);
    }

    inet_ntop(AF_INET, &(address.sin_addr), server_address, INET_ADDRSTRLEN);
    connections[server] = new ListenSock(server_address, PORT);
    connections[0] = new StdIn();
}

int main(int argument, char const *argv[])
{ 
    int ready_socket, timeout = (10 * 1000), flag = true;
    time_point t = system_clock::now();
    time_point e = t + seconds(10);
    double d = duration_cast<milliseconds>(e - t).count(); //Set block duration to 10 seconds
    vector<string> peers; //Save outgoing connection
    map<int, Connection*> connections; //Save both incoming and outgoing connection
    ParsingMachine parser(connections, peers);

    //Add StdIn and Listening Socket into connections
    init_connections(connections);
    peers.push_back(SERVER_IP + std::string(":") + std::to_string(PORT));

    //Main loop, do poll() and call each connection's handler
    while (flag)
    {
        int nfds = connections.size();
        struct pollfd fds[nfds];
        memset(fds, 0, nfds);
        int i = 0;
        for (auto it : connections)
        {
            fds[i].fd = it.first;
            fds[i].events = POLLIN;
            i++;
        }

        d = duration_cast<milliseconds>(e - t).count();

        if(d < 0)
        {
            d = 0;
            e = t + seconds(10);
        }
        ready_socket = poll(fds, nfds, d);

        if (ready_socket < 0)
        {
            perror("poll() failed");
            break;
        }

        if (ready_socket == 0 && connections.size() > 2)
        {
            printf("Broadcast peer connection table\n");
            auto it = connections.begin();
            std::advance(it, 2);
            std::advance(it, rand() % (connections.size() - 2));
            parser.parse(0, it->first, 0);
            
            t = system_clock::now();
            e = t + seconds(10);
            continue;
        }

        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents == POLLIN)
            {
                flag = connections[fds[i].fd]->handler(connections, peers, fds[i].fd, parser);
            }
            else if (fds[i].revents == 0)
            {
                continue;
            }
            if (fds[i].revents != POLLIN)
            {
                printf("Error! revents = %d, at %d\n", fds[i].revents, fds[i].fd);
                flag = true;
                break;
            }
        }
        t = system_clock::now();
    }
    
    //Close all connections before ending
    auto it = connections.begin();
    std::advance(it, 1); 
    while (it != connections.end())
    {
        close(it->first);
        ++it;
    }
    peers.clear();
    connections.clear();

    return 0;
}
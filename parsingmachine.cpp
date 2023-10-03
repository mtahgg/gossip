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
#include "parsingmachine.h"
#include "connection.h"
#include "peersock.h"

using std::cin;
using std::find;
using std::remove;
using std::map;
using std::string;
using std::vector;


ParsingMachine::ParsingMachine(map<int, Connection*> &connections, vector<string> &peers)
{
    this->connections = &connections;
    this->peers = &peers;
    init();
}

void ParsingMachine::init(){
    tran(INIT);
    this->fd = 0;
    this->size = 0;
    this->length = 0;
    this->action = 0;
    this->type = 0;
    this->processed = 0;
    this->sofar = 0;
    this->peer_status = 1;
}

void ParsingMachine::parse(int action, int fd, int type, string content){
    this->fd = fd;
    this->action = action; //0 is sending, 1 is receiving
    this-> type = type; //0 is broadcasting message, 1 is normal message
    if(this->action == 0){
        parse_send(content);
    }else{
        parse_recv();
    }
}

int ParsingMachine::parse_send(string content)
{
    //type=0, process the message for address broadcasting, else send the message to all
    if(type == 0){
        string peer_addresses = "";
        for(auto elem : *peers){
            peer_addresses = peer_addresses + elem + ",";
        }
        size = peer_addresses.length()+1;
        message_size[0] = size&0xff;
        message_size[1] = (size >> 8)&0xff;
        message_type[0] = 0x00;
        message = (char*)malloc(size);
        strcpy(message, peer_addresses.c_str());
        printf("%s\n", message);
    } else{
        size = content.length()+1;
        message_size[0] = size&0xff;
        message_size[1] = (size >> 8)&0xff;
        message_type[0] = 0x01;
        message = (char*)malloc(size);
        strcpy(message, content.c_str());
    }
    

    length = 2;
    sofar = 0;
    while(sofar < length){
        dispatch(SIZE);
        if (peer_status == -1) return 0;
    }

    length = 1;
    sofar = 0;
    while(sofar < length){
        dispatch(TYPE);
        if (peer_status == -1) return 0;
    }

    length = size;
    sofar = 0;
    while(sofar < length){
        dispatch(MESSAGE);
        if (peer_status == -1) return 0;
    }

    dispatch(INIT);
    return 0;
}

int ParsingMachine::parse_recv()
{
    length = 2;
    sofar = 0;
    while(sofar < length){
        dispatch(SIZE);
        if (peer_status < 1) return 0;
    }
    size = message_size[0] | (message_size[1] << 8);
    message = (char*)malloc(size);

    length = 1;
    sofar = 0;
    while(sofar < length){
        dispatch(TYPE);
        if (peer_status < 1) return 0;
    }
    type = message_type[0];

    length = size;
    sofar = 0;
    while(sofar < length){
        dispatch(MESSAGE);
        if (peer_status < 1) return 0;
    }

    dispatch(INIT);
    return 0;

}

void ParsingMachine::tran(State target)
{
    myState = target;
}

void ParsingMachine::parsing_size()
{
    if(action==0){
        send_bytes(message_size);
    }else{
        receive_bytes(message_size);
    }
}

void ParsingMachine::parsing_type()
{
    if(action==0){
        send_bytes(message_type);
    }else{
        receive_bytes(message_type);
    }
    
}

void ParsingMachine::parsing_message()
{
    if(action==0){
        send_bytes(message);
    }else{
        receive_bytes(message);
        printf("Received a message: %s\n", message);
        if(type == 0){
            update_peers();
        }
    }

    free(message);
}

void ParsingMachine::send_bytes(char *message)
{

    processed = send(fd, message + sofar, length - sofar, 0);
    if(processed < 0){
            printf("  Send failed at at %d:", fd);
            perror("\n");
            peer_status = -1;
            close_socket();
    }else{
        sofar += processed;
    }


}

void ParsingMachine::receive_bytes(char *message)
{
    processed = read(fd, message + sofar, length - sofar);
    if(processed > 0){
        sofar += processed;
    }else if(processed == 0){
        peer_status = 0;
        printf("Close connection at %d:", fd);
        close_socket();
    }else{
        printf("read() failed at %d:", fd);
        perror("\n");
        peer_status = -1;
        close_socket();
    }
    
}

void ParsingMachine::close_socket(){
    close(fd);
    peers->erase(find(peers->begin(), peers->end(),(*connections)[fd]->print_address()));
    connections->erase(fd);
}

void ParsingMachine::update_peers()
{
    string str = message;
    int startIndex = 0, endIndex = 0;
    char separator = ',';
    for (int i = 0; i <= str.size(); i++) {
        
        if (str[i] == separator && i < str.size()) {
            endIndex = i;
            string peer;
            peer.append(str, startIndex, endIndex - startIndex);
            if (find(peers->begin(), peers->end(),peer) != peers->end()){
                //printf("%s exists\n", peer.c_str());
                startIndex = endIndex + 1;
                continue;
            }
            printf("Connecting %s\n", peer.c_str());
            string address = peer.substr(0, peer.find(':'));
            int port = stoi(peer.substr(peer.find(':') + 1, peer.length() - 1));
            int peer_socket = Connection::connect_socket(address, port);
            if (peer_socket == -1)
            {
                printf("  Update to peer server %s failed", address.c_str());
                perror("");
            }
            else
            {
                printf("New peer is %s", peer.c_str());
                peers->push_back(peer);
                (*connections)[peer_socket] = new PeerSock(address, port, 0);
                printf("  New outgoing connection - %d\n", peer_socket);

            }
            startIndex = endIndex + 1;
        }
    }
}

void ParsingMachine::dispatch(State state)
{
    switch (myState)
    {
        case INIT:
            switch (state)
            {
                case SIZE:
                    printf("  Transfer to SIZE state\n");
                    tran(SIZE);
                    parsing_size();
                    break;
            }
            break;

        case SIZE:
            switch (state)
            {
                case SIZE:
                    parsing_size();
                    break;

                case TYPE:
                    tran(TYPE);
                    printf("  Transfer to TYPE state\n");
                    parsing_type();
                    break;
            }
            break;

        case TYPE:
            switch (state)
            {
                case TYPE:
                    parsing_type();
                    break;

                case MESSAGE:
                    tran(MESSAGE);
                    printf("  Transfer to MESSAGE state\n");
                    parsing_message();
                    break;
            }
            break;
        
        case MESSAGE:
            switch (state)
            {
                case MESSAGE:
                    parsing_message();
                    break;

                case INIT:
                    tran(INIT);
                    printf("  Transfer to INIT state\n");
                    init();
                    break;
            }
            break;
    }
}



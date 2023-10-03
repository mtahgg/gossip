# Gossip

A server running the Gossip Protocol is able to connect and share information with other servers by broadcasting itself to them using a random algorithm.

node.cpp            Main program
connection.h		    Connection class header file
connection.cpp		  Connection class function
parsingmachine.h	  ParsingMachine class header file
parsingmachine.cpp	ParsingMachine class function
peersock.h		      PeerSock class header file, inherit from Connection class
peersock.cpp		    PeerSock class function
listensock.h		    ListenSock class header file, inherit from Connection class
listensock.cpp		  ListenSock class function
stdin.h			        StdIn class header file, inherit from Connection class
stdin.cpp		        StdIn class function

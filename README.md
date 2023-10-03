# Gossip

A server running the Gossip Protocol is able to connect and share information with other servers by broadcasting itself to them using a random algorithm.<br>

node.cpp            Main program<br>
connection.h		    Connection class header file<br>
connection.cpp		  Connection class function<br>
parsingmachine.h	  ParsingMachine class header file<br>
parsingmachine.cpp	ParsingMachine class function<br>
peersock.h		      PeerSock class header file, inherit from Connection class<br>
peersock.cpp		    PeerSock class function<br>
listensock.h		    ListenSock class header file, inherit from Connection class<br>
listensock.cpp		  ListenSock class function<br>
stdin.h			        StdIn class header file, inherit from Connection class<br>
stdin.cpp		        StdIn class function<br>

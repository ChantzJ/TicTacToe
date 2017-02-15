#include "MyPacket.h"    // defined by us
#include "lab1Client.h"  // some supporting functions.
#include "TicTacToe.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string.h>

#define BUFLEN 256

/*********************************
 * Name:  Tcpconnect
 * Purpose: Handles the TCP connection portion of the game
 *          
 * Receive: ServerInfo *serverinfo, int argc, char* argv[]
 * Return:  None
 *********************************/
void TcpConnect(ServerInfo* serverInfo, int argc, char *argv[])
{
	sockaddr_in tcpServerAddr; //Socket address of server
	char *serverNameStr = 0; //Name of server for getting IP
	int bytesReceived = 0;// Error checking incoming
	int bytesSent = 0;//Error checking outgoing
	unsigned short int tcpServerPort; //TCP Port of server
	char recvBuf[BUFLEN + 20]; //Buffer for receiving messages
	char typeName[typeNameLen]; //array for the typename
	
	// parse the argvs, obtain server_name and tcpServerPort
	parseArgv(argc, argv, &serverNameStr, tcpServerPort);
	
	// get the address of the server
    struct hostent *hostEnd = gethostbyname(serverNameStr);
	serverInfo->hostEnd = hostEnd;
	std::cout << "[TCP] Tic Tac Toe client started..." << std::endl;
	std::cout << "[TCP] Connecting to server: " << serverNameStr
		<< ":" << tcpServerPort << std::endl;

 	// Create the socket using TCP (SOCK_STREAM)
	// check for errors
	serverInfo->TcpServerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverInfo->TcpServerFd < 0) 
	{
		std::cerr << "[ERR] Unable to create TCP socket." << std::endl;
		exit(1);
	}
	
	// Create socket to connect to server with
	// Check for errors
	memset(&tcpServerAddr, 0, sizeof(tcpServerAddr));
	tcpServerAddr.sin_family = AF_INET;
	tcpServerAddr.sin_port = htons(tcpServerPort);
	memcpy(&tcpServerAddr.sin_addr, hostEnd -> h_addr, hostEnd -> h_length);			
	if(connect(serverInfo->TcpServerFd, (sockaddr *)&tcpServerAddr, 
		sizeof(tcpServerAddr)) < 0)
	{
		std::cerr << "[ERR] Unable to connect to server." << std::endl;
		if(close(serverInfo->TcpServerFd) < 0)
		{
			std::cerr << "[ERR] Error when closing TCP socket" << std::endl;
		}
		exit(1);
	}
	
	MyPacket myPacket; //Packet used to send message
	
	// TCP Send connection request (JOIN)
	memset(myPacket.buffer, 0, kBufferLen);
	myPacket.type = JOIN;	
	bytesSent = send(serverInfo->TcpServerFd, &myPacket, sizeof(myPacket), 0);
	if ( bytesSent < 0) {
		std::cerr << "[ERR] Error receiving sending to server." << std::endl;
		exit(1);
	}
	getTypeName(myPacket.type, typeName);	
	std::cout << "[TCP] Sent: " << typeName << std::endl;
		
	// TCP Receive connection request (JOIN_GRANT, O/X)
	// Check for errors
	memset(recvBuf, 0, sizeof(recvBuf));
    bytesReceived = recv(serverInfo->TcpServerFd, recvBuf, sizeof(recvBuf), 0);
	MyPacket theirPacket = *(MyPacket*)recvBuf; //Packet used to receive message
	serverInfo->PlayerMark = theirPacket.buffer[0];
	getTypeName(theirPacket.type, typeName);	
    if(bytesReceived < 0) {
		std::cerr << "[ERR] Error receiving message from server." << std::endl;
		exit(1);
	} else {
		std::cout << "[TCP] Rcvd: " << typeName << " " << theirPacket.buffer << std::endl;		
	}
	
	// TCP Send port request (GET_UDP_PORT)
	memset(myPacket.buffer, 0, kBufferLen);
	myPacket.type = GET_UDP_PORT;
	bytesSent = send(serverInfo->TcpServerFd, &myPacket, sizeof(myPacket), 0);
	if ( bytesSent < 0) {
		std::cerr << "[ERR] Error receiving sending to server." << std::endl;
		exit(1);
	}
	getTypeName(myPacket.type, typeName);
	std::cout << "[TCP] Sent: " << typeName << std::endl;
	
	//TCP Receive connection request (UDP_PORT)
	memset(recvBuf, 0, sizeof(recvBuf));
	bytesReceived = recv(serverInfo->TcpServerFd, recvBuf, sizeof(recvBuf), 0);
	theirPacket = *(MyPacket*)recvBuf;	
	getTypeName(theirPacket.type, typeName);	
   if(bytesReceived < 0) {
		std::cerr << "[ERR] Error receiving message from server." << std::endl;
		exit(1);
	} else {
		std::cout << "[TCP] Rcvd: " << typeName << " " << theirPacket.buffer << std::endl;		
	}
	
	// Get and store UDP port
	serverInfo->UdpPort = atoi(theirPacket.buffer);
	
	// Close socket when finished with it
	if(close(serverInfo->TcpServerFd) < 0) {
		std::cerr << "[ERR] Error when closing connection socket." << std::endl;
		exit(1);
	}
}
/*********************************
 * Name:  UdpConnect
 * Purpose: Handles the UDP connection portion of the game
 *          
 * Receive: ServerInfo *serverinfo
 * Return:  None
 *********************************/
void UdpConnect(ServerInfo* serverInfo)
{
 	sockaddr_in udpServerAddr; //Socket address of server
	int bytesReceived = 0; //Error checking incoming
	int bytesSent = 0; //Error checking outgoing
	char typeName[typeNameLen];
	socklen_t udpServerAddrLen = sizeof(udpServerAddr);
	
	// Create UDP socket
	int udpSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(udpSocketFd < 0)
	{
		std::cerr << "[ERR] Unable to create UDP socket." << std::endl;
		exit(1);
	}
	
	//Init server address
	memset(&udpServerAddr, 0, sizeof(udpServerAddr));
	udpServerAddr.sin_family = AF_INET;
	udpServerAddr.sin_port = htons(serverInfo->UdpPort);
	memcpy(&udpServerAddr.sin_addr, serverInfo->hostEnd -> h_addr, serverInfo->hostEnd -> h_length);
	
	MyPacket myPacket; //Packet used to send message
	MyPacket theirPacket; //Packet used to receive message

	//UDP Request the board (GET_BOARD)
	memset(myPacket.buffer, 0, kBufferLen);
	myPacket.type = GET_BOARD;
	myPacket.buffer[0] = serverInfo->PlayerMark; //Send mark to server
	getTypeName(myPacket.type, typeName);	
	std::cout << "[UDP:" << serverInfo->UdpPort << "] Sent: " << typeName << std::endl;
	
	bytesSent = sendto(udpSocketFd, &myPacket, sizeof(myPacket),
		0, (sockaddr *) &udpServerAddr, sizeof(udpServerAddr));
	
	// Make sure was sent
	if (bytesSent < 0)
	{
		std::cerr << "[ERR] Error sending message to client." << std::endl;
		exit(1);
	}	

	// Use the UDP for game play
	while (1) 
	{
		// wipe out anything in theirPacket
		memset(&theirPacket, 0, sizeof(theirPacket));
		// wipe out anything in myPacket
		memset(&myPacket, 0, sizeof(myPacket));
		
		// receive
		bytesReceived = recvfrom(udpSocketFd, &theirPacket, sizeof(theirPacket), 0,
			(sockaddr *) &udpServerAddr, &udpServerAddrLen);
		
		// check for error
		if (bytesReceived < 0) 
		{
			std::cerr << "[ERR] Error receiving message from Server (UDP:"
				<< serverInfo->UdpPort << ")" << std::endl;
			exit(1);
		}
		
		
		if (theirPacket.type == YOUR_TURN)
		{
			// Game used to display the board and validate positions
			TicTacToe game(theirPacket.buffer);
			
			// Display the board
			game.printBoard();
			
			// Packet type defaults to (PLAYER_MARK) here
			myPacket.type = PLAYER_MARK;
			getTypeName(myPacket.type, typeName);
			std::cout << "Select a position (a-i, or z to quit): ";
			std::cin >> myPacket.buffer[0];	

			// Check for valid input. 
			std::string str = "zabcdefghi"; //input must be in this range of chars
			std::size_t validCharacter = str.find(myPacket.buffer[0]);

			// position must be valid and available
			while (!game.positionAvailable(myPacket.buffer[0]) || validCharacter == std::string::npos)
			{
				std::cout << "Position unavailable, try again (a-i, or z to quit): ";
				std::cin >> myPacket.buffer[0];	
				validCharacter = str.find(myPacket.buffer[0]);
			}

			// If they quit then they're marked as a quitter
			if (myPacket.buffer[0] == 'z')
			{				
				myPacket.type = EXIT;
				serverInfo->quitter = true;
			}
			
			std::cout << "[UDP:" << serverInfo->UdpPort << "] Sent: " << typeName << " " << myPacket.buffer[0] << std::endl;
			
			// Send packet
			bytesSent = sendto(udpSocketFd, &myPacket, sizeof(myPacket),
				0, (sockaddr *) &udpServerAddr, sizeof(udpServerAddr));
			
			// Make sure was sent
			if (bytesSent < 0)
			{
				std::cerr << "[ERR] Error sending message to client." << std::endl;
				exit(1);
			}	
		}
		
		if (theirPacket.type == UPDATE_BOARD)
		{
			//UDP Request the board (GET_BOARD)
			memset(myPacket.buffer, 0, kBufferLen);
			myPacket.type = GET_BOARD;
			myPacket.buffer[0] = serverInfo->PlayerMark; //Send mark to server
			getTypeName(myPacket.type, typeName);	
			std::cout << "[UDP:" << serverInfo->UdpPort << "] Sent: " << typeName << std::endl;
			
			bytesSent = sendto(udpSocketFd, &myPacket, sizeof(myPacket),
				0, (sockaddr *) &udpServerAddr, sizeof(udpServerAddr));
			
			// Make sure was sent
			if (bytesSent < 0)
			{
				std::cerr << "[ERR] Error sending message to client." << std::endl;
				exit(1);
			}	
		}
		
		if (theirPacket.type == YOU_LOSE)
		{
			// Game used to display the board and validate positions
			TicTacToe game(theirPacket.buffer);
			
			// Display the board
			game.printBoard();
			
			std::cout << "Too bad! You lose!" << std::endl;
			break;
		}
		
		if (theirPacket.type == YOU_WON)
		{
			// Game used to display the board and validate positions
			TicTacToe game(theirPacket.buffer);
			
			// Display the board
			game.printBoard();
			
			std::cout << "Congratulations! You won!" << std::endl;
			break;
		}
		
		if (theirPacket.type == TIE)
		{
			// Game used to display the board and validate positions
			TicTacToe game(theirPacket.buffer);
			
			// Display the board
			game.printBoard();
			
			std::cout << "No winners! It's a tie!" << std::endl;
			break;
		}
		
		// When 'q' entered
		if (theirPacket.type == EXIT_GRANT)
		{
			if (serverInfo->quitter)
			{
				std::cout << "You have left the game. You lose!" << std::endl;
			}
			else
			{
				std::cout << "Opponent has left the game. You win!" << std::endl;
			}
			break;
		}
	}
	
	// Close connection
	if (close(udpSocketFd) < 0) {
		std::cerr << "[ERR] Error when closing UDP server socket" << std::endl;
		exit(1);
  }
}
/*********************************
 * Name:  main
 * Purpose: Store info about the server for ease of access
 *          
 * Receive: int argc, char* argv[]
 * Return:  int
 *********************************/
int main(int argc, char *argv[])
{
	// Stores information needed in other functions
	ServerInfo serverInfo;
	
	// Make sure quitter is false in case someone quits
	serverInfo.quitter = false;
	
 	//Do TCP section
	TcpConnect(&serverInfo, argc, argv);

	//Do UDP section
	UdpConnect(&serverInfo);
		
	return 0;
}
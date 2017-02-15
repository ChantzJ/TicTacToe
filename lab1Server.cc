#include "MyPacket.h"  // defined by us
#include "TicTacToe.h"
#include "lab1Server.h"

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

// Function prototypes
void gameHandler(int* tcpPlayerFd);
bool handlePlayerJoin(int& tcpConnectionFd, const char& mark);

int main(int argc, char *argv[]) {
  sockaddr_in tcpServerAddr;
  sockaddr_in tcpConnectionAddr;
  socklen_t   tcpServerAddrLen = sizeof(tcpServerAddr);
  socklen_t   tcpConnectionAddrLen = sizeof(tcpConnectionAddr);

  int         tcpServerFd;

  parseArgv(argc, argv);  // In fact, this is not necessary.
                          // But I leave it here to capture invalid
                          // parameters.

  std::cout << "[TCP] Tic-Tac-Toe server started..." << std::endl;

  // create a TCP listening socket for clients to connect
  tcpServerFd = socket(AF_INET, SOCK_STREAM, 0);

  // if the return value is -1, the creation of socket is failed.
  if (tcpServerFd < 0) {
    std::cerr << "[ERR] Unable to create TCP socket." << std::endl;
    exit(1);
  }

  // initialize the socket address strcut by setting all bytes to 0
  memset(&tcpServerAddr, 0, sizeof(tcpServerAddr));

  // details are covered in class/slides
  tcpServerAddr.sin_family = AF_INET;
  tcpServerAddr.sin_port = 0;  // ask the OS to assign the port
  tcpServerAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket address to the listening socket
  if (bind(tcpServerFd,
           (sockaddr *)&tcpServerAddr,
           sizeof(tcpServerAddr)) < 0) {
    std::cerr << "[ERR] Unable to bind TCP socket." << std::endl;
    exit(1);
  }

  // listen to the socket, wait for incoming connections
  if (listen(tcpServerFd, 1)) {
    std::cerr << "[ERR] Unable to listen TCP socket." << std::endl;
    exit(1);
  }

  // Obtain the socket information created by the OS
  if (getsockname(tcpServerFd, (sockaddr *) &tcpServerAddr,
           &tcpServerAddrLen) < 0) {
    std::cout << "[ERR] Unable to obtain TCP socket information."
              << std::endl;
    exit(1);
  }
  std::cout << "[TCP] Port: " << ntohs(tcpServerAddr.sin_port) << std::endl;

  int tcpPlayerFd[2];

  tcpPlayerFd[0] = -1;
  tcpPlayerFd[1] = -1;

  // first we need to accept two players
  for (int i = 0; i < 2; i++) {
    while (tcpPlayerFd[i] == -1) {
      // accept the incoming connection as player1
      // store client addr in tcpConnectionAddr
      tcpPlayerFd[i] = accept(tcpServerFd,
                              (sockaddr *) &tcpConnectionAddr,
                              &tcpConnectionAddrLen);

      // If the server fail to accept
      if (tcpPlayerFd[i] < 0) {
        std::cerr << "[ERR] Error on accepting player " << i << "."
                  << std::endl;
      } else if (!handlePlayerJoin(tcpPlayerFd[i], TicTacToe::getMark(i))) {
        if (close(tcpPlayerFd[i]) < 0) {
          std::cerr << "[ERR] Error when closing bad player tcp "
                    << "connection." << std::endl;
          exit(1);
        }
        tcpPlayerFd[i] = -1;
        std::cerr << "[ERR] The TCP connection sends invalid message."
                  << std::endl;
      }
    }
  }
  // when we reach here, we have two players

  gameHandler(tcpPlayerFd);

  if (close(tcpServerFd) < 0) {
    std::cerr << "[ERR] Error when closing TCP server socket." << std::endl;
    exit(1);
  }

  return 0;
}

/*********************************
 * Name:  handlePlayerJoin
 * Purpose: The function to handle TCP communication from/to a client
 * Receive: The tcpConnectionFd socket. The client has to perform a connect
 *      through TCP to obtain a mark assignemnt (O or X) for gameplay 
 * Return:  true if this is a valid join
*********************************/
bool handlePlayerJoin(int& tcpConnectionFd, const char& mark) {
  MyPacket incomingPkt;
  MyPacket outgoingPkt;
  int      bytesReceived;
  int      bytesSent;
  char     typeName[typeNameLen];

  // wipe out anything in incomingPkt's buffer
  memset(&incomingPkt, 0, sizeof(incomingPkt));
  // receive
  bytesReceived = recv(tcpConnectionFd,
             &incomingPkt,
             sizeof(incomingPkt),
             0);

  // check
  if (bytesReceived < 0) {
    std::cerr << "[ERR] Error receiving message from client" << std::endl;
    return false;
  } else {
    getTypeName(incomingPkt.type, typeName);
    std::cout << "[TCP] Recv: " << typeName << " "
              << incomingPkt.buffer << std::endl;
  }

  // if this pkt is not trying to establish a game connection JOIN,
  // we discard this message and give up this player
  if (incomingPkt.type != JOIN) {
    std::cerr << "[ERR] Wrong packet type. Expecting JOIN" << std::endl;
    return false;
  } else {  // the command received is JOIN
    memset(&outgoingPkt, 0, sizeof(outgoingPkt));
    outgoingPkt.type = JOIN_GRANT;
    outgoingPkt.buffer[0] = mark;

    bytesSent = send(tcpConnectionFd,
                     &outgoingPkt,
                     sizeof(outgoingPkt),
                     0);

    // check
    if (bytesSent < 0) {
      std::cerr << "[ERR] Error sending message to client." << std::endl;
      exit(1);
    } else {
      getTypeName(outgoingPkt.type, typeName);
      std::cout << "[TCP] Sent: " << typeName << " "
                << outgoingPkt.buffer << std::endl;
    }
    return true;
  }
}

/*********************************
 * Name:  gameHandler
 * Purpose: The function to create UDP socket and handle the actual gameplay 
 * Receive: The tcpPlayerFd connectsions. The client has to perform a GET_UDP_PORT
 *      through TCP to login to the server and to obtain the UDP socket
 *      for gameplay 
 * Return:  None
*********************************/
void gameHandler(int* tcpPlayerFd) {
  unsigned short int udpServerPort;
  sockaddr_in udpServerAddr;
  socklen_t   udpServerAddrLen = sizeof(udpServerAddr);
  sockaddr_in udpClientAddr;
  socklen_t   udpClientAddrLen = sizeof(udpClientAddr);

  int     udpServerFd;

  MyPacket  incomingPkt;
  MyPacket  outgoingPkt;
  int     bytesReceived;
  int     bytesSent;

  char    portString[16];
  char    typeName[typeNameLen];

  TicTacToe game;

  // create a UDP listening socket for clients to connect
  udpServerFd = socket(AF_INET, SOCK_DGRAM, 0);

  // if the return value is -1, the creation of UDP socket is failed.
  if (udpServerFd < 0) {
    std::cerr << "[ERR] Unable to create UDP socket." << std::endl;
    exit(1);
  }

  // initialize the socket address strcut by setting all bytes to 0
  memset(&udpServerAddr, 0, sizeof(udpServerAddr));

  // details are covered in class/slides
  udpServerAddr.sin_family = AF_INET;
  udpServerAddr.sin_port = 0;  // Ask the OS to assign the port
  udpServerAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket address to the listening socket
  if (bind(udpServerFd,
      (sockaddr *) &udpServerAddr,
      sizeof(udpServerAddr)) < 0) {
    std::cerr << "[ERR] Unable to bind UDP socket." << std::endl;
    exit(1);
  }

  udpClientAddrLen = sizeof(udpClientAddr);

  // Get the port number chosen by OS
  // Send it to the client, so the client know which UDP port
  // to connect to.
  getsockname(udpServerFd, (sockaddr *) &udpServerAddr,
              &udpServerAddrLen);
  udpServerPort = ntohs(udpServerAddr.sin_port);

  std::cout << "[UDP:" << udpServerPort << "] Gameplay server started."
            << std::endl;
  snprintf(portString, sizeof(portString), "%d", udpServerPort);

  for (int i = 0; i < 2; i++) {
    // wipe out anything in incomingPkt's buffer
    memset(&incomingPkt, 0, sizeof(incomingPkt));
    // receive
    bytesReceived = recv(tcpPlayerFd[i],
                         &incomingPkt,
                         sizeof(incomingPkt),
                         0);

    // check
    if (bytesReceived < 0) {
      std::cerr << "[ERR] Error receiving message from client"
                << std::endl;
      exit(1);
    } else {
      getTypeName(incomingPkt.type, typeName);
      std::cout << "[TCP] Recv: " << typeName << " "
                << incomingPkt.buffer << std::endl;
    }

    // if this pkt is not trying to establish a game connection JOIN,
    // we discard this message and terminate this child process
    if (incomingPkt.type != GET_UDP_PORT) {
      std::cerr << "[ERR] Wrong packet type. Expecting GET_UDP_PORT"
                << std::endl;
      exit(1);
    } else {
      memset(&outgoingPkt, 0, sizeof(outgoingPkt));
      outgoingPkt.type = UDP_PORT;
      memcpy(outgoingPkt.buffer, portString, sizeof(portString));

      bytesSent = send(tcpPlayerFd[i],
                       &outgoingPkt,
                       sizeof(outgoingPkt),
                       0);
      // check
      if (bytesSent < 0) {
        std::cerr << "[ERR] Error sending message to client."
                  << std::endl;
        exit(1);
      } else {
        getTypeName(outgoingPkt.type, typeName);
        std::cout << "[TCP] Sent: " << typeName << " "
                  << outgoingPkt.buffer << std::endl;
      }
    }

    if (close(tcpPlayerFd[i]) < 0) {
      std::cerr << "[ERR] Error when closing TCP client socket."
                << std::endl;
      exit(1);
    }
  }
  // When reaching here, both players have got the UDP port

  game.startGame();
  int currentPlayer = 0;
  sockaddr_in opponentAddr;

  // Use the UDP for game play
  while (1) {
    // wipe out anything in incomingPkt
    memset(&incomingPkt, 0, sizeof(incomingPkt));
    // receive
    bytesReceived = recvfrom(udpServerFd, &incomingPkt,
                             sizeof(incomingPkt), 0,
                             (sockaddr *) &udpClientAddr,
                             &udpClientAddrLen);
    // check
    if (bytesReceived < 0) {
      std::cerr << "[ERR] Error receiving message from client (UDP:"
                << udpServerPort << ")" << std::endl;
      exit(1);
    }

    // wipe out anything in outgoingPkt
    memset(&outgoingPkt, 0, sizeof(outgoingPkt));
    getTypeName(incomingPkt.type, typeName);
    std::cout << "[UDP:" << udpServerPort << "] Rcvd: "
              << typeName << " "
              << incomingPkt.buffer << std::endl;

    memset(&outgoingPkt, 0, sizeof(outgoingPkt));
	
	if (incomingPkt.type == GET_BOARD)
	{
		char playerMark = incomingPkt.buffer[0];
		
		if (playerMark == TicTacToe::getMark(currentPlayer))
		{
			outgoingPkt.type = YOUR_TURN;
			memcpy(outgoingPkt.buffer, game.getBoard(), game.getBoardSize());

			bytesSent = sendto(udpServerFd, &outgoingPkt, sizeof(outgoingPkt), 0,
				(sockaddr *) &udpClientAddr, udpClientAddrLen);
			
			// check
			if (bytesSent < 0)
			{
				std::cerr << "[ERR] Error sending message to client." << std::endl;
				exit(1);
			}
			
			// convert the type in int to char typeName		
			getTypeName(outgoingPkt.type, typeName);
			std::cout << "[UDP:" << udpServerPort << "] Sent: " << typeName << " " 
				<< outgoingPkt.buffer << std::endl;
		} 
		else
		{
			memcpy(&opponentAddr, &udpClientAddr, sizeof(udpClientAddr));
		}
    } else if (incomingPkt.type == PLAYER_MARK) {
      char position = incomingPkt.buffer[0];
      game.mark(position, TicTacToe::getMark(currentPlayer));

      memcpy(outgoingPkt.buffer, game.getBoard(), game.getBoardSize());
      if (game.hasWon() == true) {
        outgoingPkt.type = YOU_WON;
      } else if (game.isTie()) {
        outgoingPkt.type = TIE;
      } else {
        outgoingPkt.type = UPDATE_BOARD;
      }

      bytesSent = sendto(udpServerFd, &outgoingPkt,
                         sizeof(outgoingPkt), 0,
                         (sockaddr *) &udpClientAddr,
                         udpClientAddrLen);
      // check
      if (bytesSent < 0) {
        std::cerr << "[ERR] Error sending message to client."
                  << std::endl;
        exit(1);
      }
      // convert the type in int to char typeName
      getTypeName(outgoingPkt.type, typeName);
      std::cout << "[UDP:" << udpServerPort << "] Sent: " << typeName
                << " " << outgoingPkt.buffer << std::endl;

      if (game.hasWon() == true) {
        outgoingPkt.type = YOU_LOSE;
      } else if (game.isTie() == true) {
        outgoingPkt.type = TIE;
      }
      bytesSent = sendto(udpServerFd, &outgoingPkt,
                 sizeof(outgoingPkt), 0,
                 (sockaddr *) &opponentAddr,
                 udpClientAddrLen);
      // check
      if (bytesSent < 0) {
        std::cerr << "[ERR] Error sending message to client."
                  << std::endl;
        exit(1);
      }
      // convert the type in int to char typeName
      getTypeName(outgoingPkt.type, typeName);
      std::cout << "[UDP:" << udpServerPort << "] Sent: " << typeName
                << " " << outgoingPkt.buffer << std::endl;

      if (currentPlayer == 0) {
        currentPlayer = 1;
      } else {
        currentPlayer = 0;
      }

      if ((game.hasWon() == true) || (game.isTie() == true)) {
        break;
      }
    } else if (incomingPkt.type == EXIT) {
      std::cout << "[UDP:" << udpServerPort
                << "] Player has left the game."
                << std::endl;

      memset(&outgoingPkt, 0, sizeof(outgoingPkt));
      outgoingPkt.type = EXIT_GRANT;
      bytesSent = sendto(udpServerFd, &outgoingPkt,
                         sizeof(outgoingPkt), 0,
                         (sockaddr *) &udpClientAddr,
                         udpClientAddrLen);
      // check
      if (bytesSent < 0) {
        std::cerr << "[ERR] Error sending message to client."
                  << std::endl;
        exit(1);
      }
      // convert the type in int to char typeName
      getTypeName(outgoingPkt.type, typeName);
      std::cout << "[UDP:" << udpServerPort << "] Sent: " << typeName
                << " " << outgoingPkt.buffer << std::endl;

      bytesSent = sendto(udpServerFd, &outgoingPkt,
                         sizeof(outgoingPkt), 0,
                         (sockaddr *) &opponentAddr,
                         udpClientAddrLen);
      // check
      if (bytesSent < 0) {
        std::cerr << "[ERR] Error sending message to client."
                  << std::endl;
        exit(1);
      }
      // convert the type in int to char typeName
      getTypeName(outgoingPkt.type, typeName);
      std::cout << "[UDP:" << udpServerPort << "] Sent: " << typeName
                << " " << outgoingPkt.buffer << std::endl;

      break;
    }
  }
  if (close(udpServerFd) < 0) {
    std::cerr << "[ERR] Error when closing UDP server socket" << std::endl;
    exit(1);
  }
}


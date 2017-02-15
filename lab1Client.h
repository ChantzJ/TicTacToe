#ifndef _LAB1_CLIENT_H_
#define _LAB1_CLIENT_H_

#include "MyPacket.h"  // defined by us
#include "TicTacToe.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

/*********************************
 * Name:  ServerInfo
 * Purpose: Store info about the server for ease of access
 *          
 * Receive: none
 * Return:  none
 *********************************/
struct ServerInfo
{
	int TcpServerFd;
	struct hostent *hostEnd;
	int UdpPort;
	char PlayerMark;
	bool quitter;
};

/*********************************
 * Name:  helpMessage
 * Purpose: display to the users how to run this program
 *          also show users the available paramters
 * Receive: none
 * Return:  none
 *********************************/
void helpMessage() {
  std::cout << "[ERR] Usage ./lab1Client [options]" << std::endl;
  std::cout << "[ERR] The following options are available:" << std::endl;
  std::cout << "    -s addr   Specify server IP address" << std::endl;
  std::cout << "    -p port   Specify server port number" << std::endl;
  std::cout << "    -h        Display help message" << std::endl;
}

/*********************************
 * Name:  parseArgv
 * Purpose: parse the parameters
 * Receive: argv and argc
 * Return:  return by call by reference
 *          serverNameStr: the domain name of the server
 *          tcpServerPort: the port number that the server listens for 
 *          incoming TCP connections.
 *********************************/
int parseArgv(int argc, char *argv[],
              char **serverNameStr, unsigned short int &tcpServerPort) {
  char *endptr;  // for strtol

  for (int i = 1; i < argc; i++) {
    if ((!strncmp(argv[i], "-s", 2)) ||
       (!strncmp(argv[i], "-S", 2))) {
      *serverNameStr = argv[++i];
    } else if ((!strncmp(argv[i], "-p", 2)) ||
        (!strncmp(argv[i], "-P", 2))) {
      tcpServerPort = strtol(argv[++i], &endptr, 0);
      if (*endptr) {
        std::cerr << "[ERR] Invalid port number" << std::endl;
        exit(1);
      }
    } else if ((!strncmp(argv[i], "-h", 2)) ||
          (!strncmp(argv[i], "-H", 2))) {
      helpMessage();
      exit(1);
    } else {
      std::cerr << "[ERR] Invalid parameter:" << argv[i] << std::endl;
      helpMessage();
      exit(1);
    }
  }

  if ((*serverNameStr == 0) || (tcpServerPort == 0)) {
    std::cerr << "[ERR] You must specify both server IP address and port."
              << std::endl;
    helpMessage();
    exit(1);
  }
  return 0;
}

/*********************************
 * Name:  getCommand
 * Purpose: This function acquires command from the users. We construct 
 *          corresponding packets based on these commands.
 *          packets from those commands.
 * Receive: outgoingPkt: the packet we're going to construct
 * Return:  outgoingPkt: the packet constructed
 *********************************/
bool getCommand(MyPacket& outgoingPkt, TicTacToe& game, const char& myMark) {
  // NOTE: Feel free to modify this code.
  //       BUT PLEASE DO NOT modify this function for accepting JOIN from
  //       the player. JOIN must be issued by the client
  //       program automatically.
  char command[16];
  char param[16];

  std::cout << "[CMD] ";
  std::cin >> command;  // first, get an input as the command

  if (std::cin.eof() || std::cin.fail()) {
    return false;
  }

  if (strcmp(command, "EXIT\0") == 0) {
    // Otherwise, the player has logged in, construct an exit message
    // to leave this game.
    memset(outgoingPkt.buffer, 0, kBufferLen);
    outgoingPkt.type = EXIT;
    return true;
  } else if (strcmp(command, "MARK\0") == 0) {
    memset(param, '\0', sizeof(param));
    // If the command is MARK, we need to get another input.
    std::cin >> param;

    if ((param[1] != '\0') || (game.positionAvailable(param[0]) == false)
      || param[0] < 'a' || param[0] > 'i') {
      if (param[1] != '\0') {
        std::cout << "This is an invalid parameter." << std::endl;
      } else {
        std::cout << "[GAM] Invalid position. Either it is already "
                  << " taken or it is out of range (a - i)."
                  << std::endl;
      }
      return false;
    }
    game.mark(param[0], myMark);
    game.printBoard();

    // If the string/combination is vlaid, construct a message to the
    // server
    memset(outgoingPkt.buffer, 0, kBufferLen);
    outgoingPkt.type = PLAYER_MARK;
    memcpy(outgoingPkt.buffer, param, 16);
    return true;
  } else {
    // As mentioned above, reject all invalid inputs.
    std::cout << "[GAM] Invalid command." << std::endl;
    return false;
  }
}

#endif  // _LAB1_CLIENT_H_

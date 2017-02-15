// Simple packet structure
#ifndef _MY_PACKET_H_
#define _MY_PACKET_H_

#include <string.h>

#include <iostream>

#define JOIN          1000
#define JOIN_GRANT    1001
#define GET_UDP_PORT  2000
#define UDP_PORT      2001
#define GET_BOARD     3000
#define YOUR_TURN     3001
#define OPPONENT_TURN 3002
#define PLAYER_MARK   4001
#define UPDATE_BOARD  4002
#define YOU_WON       4003
#define YOU_LOSE      4004
#define TIE           4005
#define EXIT          9000
#define EXIT_GRANT    9001

const unsigned int typeNameLen = 16;
const unsigned int kBufferLen = 16;

struct MyPacket {
  unsigned int type;
  char buffer[kBufferLen];
};

/*********************************
 * Name:  getTypeName
 * Purpose: When receiving a message, we can only see the type as integer
 *      (ie, 2000, 2001, 3000 ... etc). This function converts those
 *      integers into coresponding char arrays.
 * Receive: The type in integer
 * Return:  the type in char array
 *********************************/
void getTypeName(int type, char *typeName) {
  memset(typeName, 0, typeNameLen);
  switch (type) {
    case JOIN:
      memcpy(typeName, "JOIN\0", typeNameLen);
      break;
    case JOIN_GRANT:
      memcpy(typeName, "JOIN_GRANT\0", typeNameLen);
      break;
    case GET_UDP_PORT:
      memcpy(typeName, "GET_UDP_PORT\0", typeNameLen);
      break;
    case UDP_PORT:
      memcpy(typeName, "UDP_PORT\0", typeNameLen);
      break;
    case GET_BOARD:
      memcpy(typeName, "GET_BOARD\0", typeNameLen);
      break;
    case YOUR_TURN:
      memcpy(typeName, "YOUR_TURN\0", typeNameLen);
      break;
    case OPPONENT_TURN:
      memcpy(typeName, "OPPONENT_TURN\0", typeNameLen);
      break;
    case PLAYER_MARK:
      memcpy(typeName, "PLAYER_MARK\0", typeNameLen);
      break;
    case UPDATE_BOARD:
      memcpy(typeName, "UPDATE_BOARD\0", typeNameLen);
      break;
    case YOU_WON:
      memcpy(typeName, "YOU_WON\0", typeNameLen);
      break;
    case YOU_LOSE:
      memcpy(typeName, "YOU_LOSE\0", typeNameLen);
      break;
    case TIE:
      memcpy(typeName, "TIE\0", typeNameLen);
      break;
    case EXIT:
      memcpy(typeName, "EXIT\0", typeNameLen);
      break;
    case EXIT_GRANT:
      memcpy(typeName, "EXIT_GRANT\0", typeNameLen);
      break;
    default:
      std::cerr << "[SYS] Invalid command type: " << type << "."
                <<  std::endl;
      break;
  }
}

#endif  // _MY_PACKET_H_

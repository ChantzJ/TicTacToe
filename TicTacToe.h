#ifndef _TIC_TAC_TOE_
#define _TIC_TAC_TOE_

#include <string.h>
#include <iostream>

class TicTacToe {
 public:
  /*********************************
   * Name:  TicTacToe::constructor
   * Purpose: Create a new game
   * Receive: None
   * Return:  None
   ********************************/
  TicTacToe() { startGame();}

  /*********************************
   * Name:  TicTacToe::constructor
   * Purpose: Create a game from a char array (for printing purpose)
   * Receive: inBoard: a char* buffer from messages
   * Return:  None
   ********************************/
  explicit TicTacToe(char* inBoard);

  /*********************************
   * Name:  TicTacToe::mark
   * Purpose: mark a space at position position iwth mark mark
   * Recieve: position: the space position to be marked.
   *      mark: the mark, either nought or cross
   * Return:  None
   *********************************/
  void mark(const char& position, const char& mark);

  /*********************************
   * Name:  TicTacToe::positionAvailable
   * Purpose: check if a space at a position is available
   * Recieve: position: the space position to be checked
   * Return:  true if the position is available, false otherwise.
   *********************************/
  bool positionAvailable(const char& position);

  /*********************************
   * Name:  TicTacToe::startGame
   * Purpose: Start a new game by setting all spaces to unoccupied
   * Receive: None
   * Return:  None
   *********************************/
  void startGame();

  /*********************************
   * Name:  TicTacToe::getBoard()
   * Purpose: get the current board as a char*, so that we can put the 
   *      board in a message
   * Recieve: None
   * Return:  char* for the board
   *********************************/
  char* getBoard();

  /*********************************
   * Name:  TicTacToe::getBoardSize()
   * Purpose: get the board size
   * Recieve: None
   * Return:  size of the board
   *********************************/
  int getBoardSize() { return kBoardSize;}

  /*********************************
   * Name:  TicTacToe::printBoard
   * Purpose: print the Tic-Tac-Toe board
   * Recieve: None
   * Return:  None
   *********************************/
  void printBoard();

  /*********************************
   * Name:  TicTacToe::getMark
   * Purpose: get the mark at a position
   * Recieve: position
   * Return:  the mark at the position
   *********************************/
  static char getMark(const int& i) {
    if (i == 0) {
      return nought;
    } else if (i == 1) {
      return cross;
    }
    return ' ';
  }

  /*********************************
   * Name:  TicTacToe::hasWon
   * Purpose: check if there is a winner
   * Recieve: None
   * Return:  true if there is a winner
   *********************************/
  bool hasWon();

  /*********************************
   * Name:  TicTacToe::isTie
   * Purpose: check if the game ends with a tie
   * Recieve: None
   * Return:  true if the game ends with a tie
   *********************************/
  bool isTie();

 private:
  static const int kBoardSize = 9;
  static const char unoccupied = '_';
  static const char nought = 'O';
  static const char cross = 'X';

  char board[kBoardSize];
};

#endif  // _TIC_TAC_TOE_

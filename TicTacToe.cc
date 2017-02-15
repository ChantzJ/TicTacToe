#include "TicTacToe.h"

#include <time.h>
#include <string.h>

#include <iostream>
#include <cstdlib>

TicTacToe::TicTacToe(char* inBoard) {
  memcpy(board, inBoard, getBoardSize());
}

void TicTacToe::startGame() {
  // clear the board by setting all positions to '_'
  memset(board, unoccupied, sizeof(board));
}

void TicTacToe::mark(const char& position, const char& mark) {
  board[position - 'a'] = mark;
}

bool TicTacToe::positionAvailable(const char& position) {
  if (board[position - 'a'] != unoccupied) {
    return false;
  } else {
    return true;
  }
}

char* TicTacToe::getBoard() {
  return board;
}

bool TicTacToe::hasWon() {
  if ((board[0] == board[1])&& (board[1] == board[2])
     && (board[0] != unoccupied)) {
    return true;
  } else if ((board[3] == board[4])&& (board[4] == board[5])
        && (board[3] != unoccupied)) {
    return true;
  } else if ((board[6] == board[7])&& (board[7] == board[8])
        && (board[6] != unoccupied)) {
    return true;
  } else if ((board[0] == board[3])&& (board[3] == board[6])
        && (board[0] != unoccupied)) {
    return true;
  } else if ((board[1] == board[4])&& (board[4] == board[7])
        && (board[1] != unoccupied)) {
    return true;
  } else if ((board[2] == board[5])&& (board[5] == board[8])
        && (board[2] != unoccupied)) {
    return true;
  } else if ((board[0] == board[4])&& (board[4] == board[8])
        && (board[0] != unoccupied)) {
    return true;
  } else if ((board[2] == board[4])&& (board[4] == board[6])
        && (board[2] != unoccupied)) {
    return true;
  } else {
    return false;
  }
}

bool TicTacToe::isTie() {
  if (hasWon()== true) { return false;}
  for (int i = 0; i < getBoardSize(); i++) {
    if (board[i] == unoccupied) {
      return false;
    }
  }
  // reaches here means all spaces are marked
  return true;
}

void TicTacToe::printBoard() {
  char pboard[9];  // for printing
  for (int i = 0; i < 9; i++) {
    if (board[i] == unoccupied) {
      pboard[i] = i + 'a';
    } else {
      pboard[i] = board[i];
    }
  }

  std::cout << "|---|---|---|" << std::endl;
  std::cout << "| " << pboard[0] << " | " << pboard[1] << " | "
            << pboard[2] << " |" << std::endl;
  std::cout << "|---+---+---|" << std::endl;
  std::cout << "| " << pboard[3] << " | " << pboard[4] << " | "
            << pboard[5] << " |" << std::endl;
  std::cout << "|---+---+---|" << std::endl;
  std::cout << "| " << pboard[6] << " | " << pboard[7] << " | "
            << pboard[8] << " |" << std::endl;
  std::cout << "|---|---|---|" << std::endl;
}

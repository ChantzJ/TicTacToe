all: TicTacToe.o lab1Server.o lab1Client.o
	g++ -g -o lab1Server lab1Server.o TicTacToe.o 
	g++ -g -o lab1Client lab1Client.o TicTacToe.o

TicTacToe.o: TicTacToe.cc TicTacToe.h
	g++ -Wall -c TicTacToe.cc

lab1Server.o: lab1Server.cc lab1Server.h MyPacket.h TicTacToe.h
	g++ -Wall -c lab1Server.cc 

lab1Client.o: lab1Client.cc lab1Client.h MyPacket.h TicTacToe.h 
	g++ -Wall -c lab1Client.cc 

clean:
	rm -rf lab1Server lab1Client *.o


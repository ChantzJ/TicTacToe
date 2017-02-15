#ifndef _LAB1_SERVER_H_
#define _LAB1_SERVER_H_

#include <cstdlib>

/*********************************
 * Name:    helpMessage
 * Purpose: display to the users how to run this program
 *          also show users the available parameters
 * Recieve: none
 * Return:  none
 *********************************/
void helpMessage() {
    std::cerr << "[ERR] Usage ./lab1Server [options]" << std::endl;
    std::cout << "[ERR] The following option is available:" << std::endl;
    std::cout << "      -h        Display help message." << std::endl;
}

/*********************************
 * Name:    parseArgv
 * Purpose: parse the parameters
 * Recieve: argv and argc
 * Return:  none
 * NOTE:    In fact we do not need this. I leave it here, so that
 *          it can capture invalid parameters.
 *********************************/
void parseArgv(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if ((!strncmp(argv[i], "-h", 2)) ||
           (!strncmp(argv[i], "-H", 2))) {
            helpMessage();
            exit(1);
        } else {
            std::cerr << "[ERR] Invalid parameter:" << argv[i] << std::endl;
            helpMessage();
            exit(1);
        }
    }
}
#endif  // _LAB1_SERVER_H_

/*
    File     draw.h
    Authors  Henry Hickman, Taran Jennison
    Date     15 October 2019
    Brief    H file for communication between FunKits
*/


#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/**Handles recieving communication from the sender
*/
int recv(void);

/**Sends a character to the other player
@param char the character to be sent
*/
void send(char sender);

#endif
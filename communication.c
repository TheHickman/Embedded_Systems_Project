/*
    File     draw.h
    Authors  Henry Hickman, Taran Jennison
    Date     15 October 2019
    Brief    C file for communication between FunKits
*/

#include "system.h"
#include "ir_uart.h"
#include "communication.h"

int recv(void)
{
    uint8_t recieved = 0;
    int val = 0;
    while (recieved < 1) {
        if (ir_uart_read_ready_p()) {
            val = ir_uart_getc();
            recieved = 1;
        }
    }
    return val;
}

void send(char sender)
{
    uint8_t sent = 0;
    while (sent < 1) {
        if (ir_uart_write_ready_p()) {
            ir_uart_putc(sender);
            sent = 1;
        }
    }
}
#include "../../include/portmap.h"

/**
 * Purpose: To read and return a byte from a specified port.
 * << inb >>
 * @params: 
 *      uint16_t port -> the specified port number
 * @returns:
 *      uint8_t result -> a byte representing the key pressed
 */ 
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/**
 * Purpose: To write a byte to a specified port. Ex. Used to tell the keyboard
 * - when to activate things like CapsLock indicator light.
 * << outb >>
 * @params: 
 *      uint16_t port -> the specified port number
 *      uint8_t data -> a byte to send to the port
 * @returns: void
 */ 
void outb(uint16_t port, uint8_t data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

void outw(uint16_t port, uint16_t value) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

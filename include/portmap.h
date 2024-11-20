#ifndef OS_INCLUDE_PORT_MAP_H
#define OS_INCLUDE_PORT_MAP_H
#include <stdint.h>

/* Port IO Functions */
/* Input Function */
uint8_t inb(uint16_t port);
/* Output Function */
void outb(uint16_t port, uint8_t data);

void outw(uint16_t port, uint16_t value);

#endif
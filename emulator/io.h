#ifndef IO_H
#define IO_H

#include <stdint.h>

uint8_t io_port_read(uint8_t);
void io_port_write(uint8_t, uint8_t);

#endif

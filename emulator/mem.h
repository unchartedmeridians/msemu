#ifndef MEM_H
#define MEM_H

#include <stdint.h>

void mem_set_bios(uint8_t*);
uint8_t mem_read(uint16_t addr);
uint16_t mem_read16(uint16_t addr);
void mem_write(uint16_t, uint8_t);

#endif

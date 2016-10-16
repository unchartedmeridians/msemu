#include <stdio.h>
#include <stdint.h>
#include "mem.h"

uint8_t ram[0x2000] = { 0 };
uint8_t* bios;

void mem_set_bios(uint8_t* bios_p) {
    bios = bios_p;
}


uint8_t mem_read(uint16_t addr) {
    if (addr < 0x400) // ROM (unpaged)
        return bios[addr];
    else if (addr < 0x4000) // ROM mapper slot 0 (filled w/ last 15kb of bank)
        return bios[addr];
    else if (addr < 0x8000) // ROM mapper slot 1
        ;
    else if (addr < 0xc000) // ROM/RAM mapper slot 2
        ;
    else if (addr < 0xe000) // System RAM
        return ram[addr - 0xc000];
    else if (addr < 0xfff7) // System RAM (mirrored)
        return ram[addr - 0xe000];
    else if (addr < 0xfffc)
        ; // 3D glasses control, mirrored 4 times
    else if (addr == 0xfffc)
        ; // Cartridge RAM mapper control
    else if (addr == 0xfffd)
        ; // Mapper slot 0 control
    else if (addr == 0xfffe)
        ; // Mapper slot 1 control
    else if (addr == 0xffff)
        ; // Mapper slot 2 control
    return 0;
}

void mem_write(uint16_t addr, uint8_t data) {
    if (addr < 0x400) // ROM (unpaged)
        ;
    else if (addr < 0x4000) // ROM mapper slot 0 (filled w/ last 15kb of bank)
        ;
    else if (addr < 0x8000) // ROM mapper slot 1
        ;
    else if (addr < 0xc000) // ROM/RAM mapper slot 2
        ;
    else if (addr < 0xe000) // System RAM
         ram[addr - 0xc000] = data;
    else if (addr < 0xfff7) // System RAM (mirrored)
        ram[addr - 0xe000] = data;
    else if (addr < 0xfffc)
        ; // 3D glasses control, mirrored 4 times
    else if (addr == 0xfffc)
        ; // Cartridge RAM mapper control
    else if (addr == 0xfffd)
        ; // Mapper slot 0 control
    else if (addr == 0xfffe)
        ; // Mapper slot 1 control
    else if (addr == 0xffff)
        ; // Mapper slot 2 control
}

uint16_t mem_read16(uint16_t addr) {
    uint16_t data;
    data = mem_read(addr);
    data |= ((uint16_t)mem_read(++addr)) << 8;
    return data;
}

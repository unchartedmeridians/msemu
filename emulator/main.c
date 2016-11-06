#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "cpu.h"
#include "vdp.h"

int main (int argc, char *argv[])
{
    uint8_t* bios = 0;

    if (argc != 2) {
        printf("Invalid number of arguments.\n");
        return 0;
    }
    else {
        FILE *bios_file = fopen(argv[1], "r");

        if (bios_file) {
            fseek(bios_file, 0, SEEK_END);
            long length = ftell(bios_file);
            fseek(bios_file, 0, SEEK_SET);
            bios = malloc(length);
            if (bios) {
                fread(bios, 1, length, bios_file);
            }
        }
        else {
            printf("Could not open file.\n");
            return 0;
        }
    }
    mem_set_bios(bios);
    cpu_init(0x00);
    vdp_init();
    while (1) {
        cpu_run(228);
        vdp_run();
    }
    return 0;
}

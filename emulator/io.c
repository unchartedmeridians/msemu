#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "io.h"
#include "vdp.h"

#define VDP_DATA_PORT 0xBE
#define VDP_CONTROL_PORT 0xBF
#define VDP_V_COUNTER 0x7E // read only
#define VDP_H_COUNTER 0x7F // read only
#define SN76489_DATA 0x7E // write only
#define SN76489_MIRROR 0x7F // write only

uint8_t io_port_read(uint8_t port)
{
    switch (port) {
        case VDP_DATA_PORT:
            return vdp_data_port_read();
            break;
        case VDP_CONTROL_PORT:
            return vdp_control_port_read();
            break;
        case VDP_V_COUNTER:
            return vdp_v_counter_read();
            break;
        case VDP_H_COUNTER:
            return vdp_h_counter_read();
            break;
        default:
            fprintf(stderr, "Attempted to read from unknown port 0x%hhX\n", port);
            abort();
    }
    return 0xFF;
}

void io_port_write(uint8_t port, uint8_t data)
{
    switch (port) {
        case VDP_DATA_PORT:
            vdp_data_port_write(data);
            break;
        case VDP_CONTROL_PORT:
            vdp_control_port_write(data);
            break;
        case SN76489_DATA:
            //TODO
            break;
        case SN76489_MIRROR:
            //TODO
            break;
        default:
            fprintf(stderr, "Attempted to write to unknown port 0x%hhX\n", port);
            abort();
    }
}

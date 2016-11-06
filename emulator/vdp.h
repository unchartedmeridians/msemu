#ifndef VDP_H
#define VDP_H

#include <stdbool.h>
#include <stdint.h>

uint8_t vdp_data_port_read(void);
void    vdp_data_port_write(uint8_t);
uint8_t vdp_control_port_read(void);
uint8_t vdp_v_counter_read(void);
uint8_t vdp_h_counter_read(void);
void    vdp_control_port_write(uint8_t);
void    vdp_init(void);
void    vdp_run(void);

#endif

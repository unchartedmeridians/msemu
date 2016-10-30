#include <stdint.h>
#include <stdbool.h>

uint8_t vram[0x4000] = { 0 };
uint8_t cram[0x20] = { 0 };

struct Vdp {
    uint8_t reg_mode_control1;                  // 0x00
    uint8_t reg_mode_control2;                  // 0x01
    uint8_t reg_name_table_base_addr;           // 0x02
    uint8_t reg_color_table_base_addr;          // 0x03
    uint8_t reg_bg_pattern_gen_base_addr;       // 0x04
    uint8_t reg_sprite_attr_table_base_addr;    // 0x05
    uint8_t reg_sprite_pattern_gen_base_addr;   // 0x06
    uint8_t reg_overscan_color;                 // 0x07
    uint8_t reg_bg_x_scroll;                    // 0x08
    uint8_t reg_bg_y_scroll;                    // 0x09
    uint8_t reg_line_counter;                   // 0x0A

    bool    position_flag; // if 1, we're waiting on 2nd byte of command word
};


uint8_t v_counter_read(void) {
    //TODO
}

uint8_t h_counter_read(void) {
    //TODO
}

void psg_write(uint8_t data) {
    //TODO
}

void data_write(uint8_t data) {
    //TODO
}

uint8_t data_read(void) {
    //TODO
}

void vdp_run(void) {


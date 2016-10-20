#include <stdint.h>

uint8_t vram[0x4000] = { 0 };
uint8_t cram[0x32] = { 0 };

struct Vdp {
    uint8_t reg_control;
    uint8_t reg_mode_control1;
    uint8_t reg_mode_control2;
    uint8_t reg_name_table_base_addr;
    uint8_t reg_color_table_base_addr;
    uint8_t reg_bg_pattern_gen_base_addr;
    uint8_t sprite_attr_table_base_addr;
    uint8_t sprite_pattern_gen_base_addr;
    uint8_t overscan_color;
    uint8_t bg_x_scroll;
    uint8_t bg_y_scroll;
    uint8_t line_counter;
};

#include <stdint.h>
#include <stdbool.h>

#define VRAM_READ_ADDR  0x00
#define VRAM_WRITE_ADDR 0x01
#define REG_WRITE       0x10
#define CRAM_WRITE_ADDR 0x11

#define REG_MODE_CONTROL1                   0x00
#define REG_MODE_CONTROL2                   0x01
#define REG_NAME_TABLE_BASE_ADDR            0x02
#define REG_COLOR_TABLE_BASE_ADDR           0x03
#define REG_BG_PATTERN_GEN_BASE_ADDR        0x04
#define REG_SPRITE_ATTR_TABLE_BASE_ADDR     0x05
#define REG_SPRITE_PATTERN_GEN_BASE_ADDR    0x06
#define REG_OVERSCAN_COLOR                  0x07
#define REG_BG_X_SCROLL                     0x08
#define REG_BG_Y_SCROLL                     0x09
#define REG_LINE_COUNTER                    0x0A

uint8_t vram[0x4000] = { 0 };
uint8_t cram[0x20] = { 0 };

struct Vdp {
    /*
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
    */
    uint8_t registers[11];
    uint8_t reg_code;
    uint16_t reg_rw_address;
    uint8_t data_buffer;
    bool    position_flag; // if true, we're waiting on 2nd byte of command word
};

struct Vdp vdp;

void vram_read(uint16_t addr) {
    //TODO: assert in range?
    return vram[addr];
}

void reg_rw_address_increment {
    vdp.reg_rw_address++;
    // if we overflow 0x3FFF, wrap
    if (vdp.reg_rw_address & 0x1100000000000000) {
        vdp.reg_rw_address = 0;
    }
}

void control_port_write(uint8_t data) {
    if (!position_flag) {
        vdp.reg_rw_address &= 0x00000000;
        vdp.reg_rw_address |= data;
        vdp.position_flag = true;
    } else {
        uint16_t addr_data = data & 0x0011111100000000;
        vdp.reg_rw_address |= data;
        vdp.reg_code |= (data >> 6);
        vdp.position_flag = false;

        switch (vdp.reg_code) {
            case VRAM_READ_ADDR:
                vdp.data_buffer = vram_read(reg_rw_address);
                break;
            case VRAM_WRITE_ADDR:
                break;
            case VRAM_REG_WRITE:
                {
                    // bits 8-11
                    uint8_t reg_data = data &= 0x11111111;
                    uint8_t reg_num = 0x1111 | (vdp.reg_rw_address >> 8);
                    if (reg_num <= 0x0A) { // writing to 0x0B+ has no effect
                        vdp.regs[reg_num] = reg_data;
                    }
                }
            case CRAM_WRITE_ADDR:
                break;
            default:
                break;
        }
    }
}

uint8_t data_port_read(void) {
    //TODO
}
void data_port_write(uint8_t data) {
}

uint8_t data_port_read(void) {


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
    //TODO
}

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "vdp.h"
#include "cpu.h"

#define VRAM_READ_ADDR  0b00
#define VRAM_WRITE_ADDR 0b01
#define VDP_REG_WRITE   0b10
#define CRAM_WRITE_ADDR 0b11

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

#define MC1_LINE_IRQ_ENABLED    0b00010000

#define MC2_VBLANK_IRQ_ENABLED   0b00100000

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
    uint16_t reg_rw_addr;
    uint8_t data_buffer;
    bool    com_word_pos_flag; // if true, we're waiting on 2nd byte of command word
    uint8_t v_counter;
    bool vblank_interrupt; // triggered when v counter hits line 193 (at 224x192)
};

uint8_t vram[0x4000] = { 0 };
uint8_t cram[0x20] = { 0 };
struct Vdp vdp;

void vdp_init(void) {
    for (int i = 0; i < 11; i++)
    {
        vdp.registers[i] = 0;
    }
    vdp.reg_code = 0;
    vdp.reg_rw_addr = 0;
    vdp.data_buffer = 0;
    vdp.com_word_pos_flag = 0;
    vdp.v_counter = 0;
    vdp.vblank_interrupt = false;
}

void reg_rw_addr_increment (void) {
    vdp.reg_rw_addr++;
    // if we overflow 0x3FFF, wrap
    if (vdp.reg_rw_addr & 0b1100000000000000) {
        vdp.reg_rw_addr = 0;
    }
}

uint8_t vram_read(uint16_t addr) {
    //TODO: assert in range?
    return vram[addr];
}

void vram_write(uint16_t addr, uint8_t data) {
    uint8_t masked_addr = addr & 0b0011111111111111; // to address 0x4000 bytes of vram
    vram[masked_addr] = data;
}

void cram_write(uint16_t addr, uint8_t data) {
    uint8_t masked_addr = addr & 0b11111; // to address 0x20 bytes of cram
    cram[masked_addr] = data;
}

uint8_t vdp_data_port_read(void) {
    vdp.com_word_pos_flag = false;
    uint8_t ret_data = vdp.data_buffer;
    reg_rw_addr_increment();
    vdp.data_buffer = vram_read(vdp.reg_rw_addr);
    return ret_data;
}

void vdp_data_port_write(uint8_t data) {
    vdp.com_word_pos_flag = false;
    if (vdp.reg_code == CRAM_WRITE_ADDR) {
        cram_write(vdp.reg_rw_addr, data);
        // no data buffer write mentioned in docs?
    } else {
        vram_write(vdp.reg_rw_addr, data);
        vdp.data_buffer = data;
    }
    reg_rw_addr_increment();
}

uint8_t vdp_control_port_read(void) {
    //TODO: implement fully
    vdp.com_word_pos_flag = false;
    uint8_t control_word = 0;
    if (vdp.vblank_interrupt) {
        control_word |= 0b10000000;
        vdp.vblank_interrupt = false;
    }
    return control_word;
}

void vdp_control_port_write(uint8_t data) {
    //TODO: make sure this actually works right!
    if (!vdp.com_word_pos_flag) {
        vdp.reg_rw_addr &= 0b00000000;
        vdp.reg_rw_addr |= data;
        vdp.com_word_pos_flag = true;
    } else {
        uint16_t addr_data = (data << 8) & 0b0011111100000000;
        vdp.reg_rw_addr &= 0b0000000011111111;
        vdp.reg_rw_addr |= addr_data;
        vdp.reg_code |= (data >> 6);
        vdp.com_word_pos_flag = false;
        switch (vdp.reg_code) {
            case VRAM_READ_ADDR:
                vdp.data_buffer = vram_read(vdp.reg_rw_addr);
                reg_rw_addr_increment();
                break;
            case VRAM_WRITE_ADDR:
                break;
            case VDP_REG_WRITE:
                {
                    // bits 8-11
                    uint8_t reg_data = vdp.reg_rw_addr & 0b11111111;
                    uint8_t reg_num = 0b1111 & (vdp.reg_rw_addr >> 8);
                    if (reg_num <= 0x0A) { // writing to 0x0B+ has no effect
                        //TODO: implement register writing with separate funcs?
                        vdp.registers[reg_num] = reg_data;
                        if (reg_num == REG_LINE_COUNTER)
                        {
                            //TODO
                        }
                    }
                    break;
                }
            case CRAM_WRITE_ADDR:
                break;
            default:
                break;
        }
    }
}

uint8_t vdp_v_counter_read(void) {
    return vdp.v_counter;
}

uint8_t vdp_h_counter_read(void) {
    fprintf(stderr, "H counter read not implemented.\n");
    char fart = getchar();
}

void psg_write(uint8_t data) {
    //TODO
}

bool screen_top = false;
void v_counter_increment(void) {
    //TODO: change depending on mode - see msvdp-20021112.txt sections 11 and 12
    if (vdp.v_counter == 0xFF) {
        vdp.v_counter = 0;
    }
    else if ((vdp.v_counter == 0xDA) && screen_top == false) {
        vdp.v_counter = 0xD5;
        screen_top = true;
    } else {
        vdp.v_counter++;
    }
    if (vdp.v_counter == 0xC1) { // first line of vblank at assumed res
        vdp.vblank_interrupt = true;
    }
}

void vdp_run(void) {
    v_counter_increment();
    //TODO: line interrupts?
    if ((vdp.vblank_interrupt = true)
            && (vdp.registers[REG_MODE_CONTROL2] & MC2_VBLANK_IRQ_ENABLED))
    {
        cpu_irq();
    }
}

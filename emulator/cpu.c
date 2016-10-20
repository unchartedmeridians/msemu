#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "mem.h"
#include "io.h"

struct Cpu {

    uint8_t reg_a;
    uint8_t reg_f;
    uint8_t reg_b;
    uint8_t reg_c;
    uint8_t reg_d;
    uint8_t reg_e;
    uint8_t reg_h;
    uint8_t reg_l;

    uint16_t reg_sp;
    uint16_t reg_pc;
    uint16_t reg_ix;
    uint16_t reg_iy;

    uint8_t reg_i;
    uint8_t reg_r;

    uint8_t reg_a_alt;
    uint8_t reg_f_alt;
    uint8_t reg_b_alt;
    uint8_t reg_c_alt;
    uint8_t reg_d_alt;
    uint8_t reg_e_alt;
    uint8_t reg_h_alt;
    uint8_t reg_l_alt;

    bool iff1;
    bool iff2;
};

int err;
struct Cpu cpu;

bool check_parity(uint8_t byte) {
    byte ^= byte >> 4;
    byte ^= byte >> 2;
    byte ^= byte >> 1;
    return (~byte) & 1;
}

void cpu_call(uint16_t addr) {
    uint8_t pc_msb = (uint8_t)cpu.reg_pc >> 8;
    uint8_t pc_lsb = (uint8_t)cpu.reg_pc & 0xFF;
    cpu.reg_sp--;
    mem_write(cpu.reg_sp, pc_msb);
    cpu.reg_sp--;
    mem_write(cpu.reg_sp, pc_lsb);
    cpu.reg_pc = addr;
}

void cpu_flag_set(char flag) {
    assert( flag == 's' ||
            flag == 'z' ||
            flag == 'h' ||
            flag == 'p' ||
            flag == 'n' ||
            flag == 'c' );
    switch (flag) {
        case 's':
            cpu.reg_f |= 1 << 7;
            break;
        case 'z':
            cpu.reg_f |= 1 << 6;
            break;
        case 'h':
            cpu.reg_f |= 1 << 4;
            break;
        case 'p':
            cpu.reg_f |= 1 << 2;
            break;
        case 'n':
            cpu.reg_f |= 1 << 1;
            break;
        case 'c':
            cpu.reg_f |= 1;
    }
}

void cpu_flag_clear(char flag) {
    assert( flag == 's' ||
            flag == 'z' ||
            flag == 'h' ||
            flag == 'p' ||
            flag == 'n' ||
            flag == 'c' );
    switch (flag) {
        case 's':
            cpu.reg_f &= ~(1 << 7);
            break;
        case 'z':
            cpu.reg_f &= ~(1 << 6);
            break;
        case 'h':
            cpu.reg_f &= ~(1 << 4);
            break;
        case 'p':
            cpu.reg_f &= ~(1 << 2);
            break;
        case 'n':
            cpu.reg_f &= ~(1 << 1);
            break;
        case 'c':
            cpu.reg_f &= ~(1);
    }
}

bool cpu_flag_test(char flag) {
    assert( flag == 's' ||
            flag == 'z' ||
            flag == 'h' ||
            flag == 'p' ||
            flag == 'n' ||
            flag == 'c' );
    switch (flag) {
        case 's':
            return (cpu.reg_f >> 7) & 1;
        case 'z':
            return (cpu.reg_f >> 6) & 1;
        case 'h':
            return (cpu.reg_f >> 4) & 1;
        case 'p':
            return (cpu.reg_f >> 2) & 1;
        case 'n':
            return (cpu.reg_f >> 1) & 1;
        case 'c':
            return cpu.reg_f & 1;
    }
    return 0;
}
void reg_pair_write(char* name, uint16_t data) {
    assert( !strcmp(name, "af") ||
            !strcmp(name, "bc") ||
            !strcmp(name, "de") ||
            !strcmp(name, "hl") );
    uint8_t lsb = data & 0xFF;
    uint8_t msb = data >> 8;
    if (strcmp(name, "af") == 0) {
        cpu.reg_a = msb;
        cpu.reg_f = lsb;
    } else if (strcmp(name, "bc") == 0) {
        cpu.reg_b = msb;
        cpu.reg_c = lsb;
    } else if (strcmp(name, "de") == 0) {
        cpu.reg_d = msb;
        cpu.reg_e = lsb;
    } else {
        cpu.reg_h = msb;
        cpu.reg_l = lsb;
    }
}

uint16_t reg_pair_read(char* name) {
    assert( !strcmp(name, "af") ||
            !strcmp(name, "bc") ||
            !strcmp(name, "de") ||
            !strcmp(name, "hl") );
    uint16_t data = 0;
    if (strcmp(name, "af") == 0) {
        data |= cpu.reg_a << 8;
        data |= cpu.reg_f;
    } else if (strcmp(name, "bc") == 0) {
        data |= cpu.reg_b << 8;
        data |= cpu.reg_c;
    } else if (strcmp(name, "de") == 0) {
        data |= cpu.reg_d << 8;
        data |= cpu.reg_e;
    } else {
        data |= cpu.reg_h << 8;
        data |= cpu.reg_l;
    }
    return data;
}

uint8_t fetch_next(void) {
    return mem_read(cpu.reg_pc++);
}

uint16_t fetch_next16(void) {
    uint16_t data = mem_read16(cpu.reg_pc);
    cpu.reg_pc += 2;
    return data;
}

void exe_instr(uint16_t opcode) {
        switch (opcode) {
            case 0x01: // LD dd, nn where dd = BC
                reg_pair_write("bc", fetch_next16());
                break;
            case 0x06: // LD r, n where r = B
                cpu.reg_b = fetch_next();
                break;
            case 0x0D: // DEC m where m = C
                cpu.reg_c--;
                if ((int16_t)cpu.reg_c < 0) {
                    cpu_flag_set('s');
                } else {
                    cpu_flag_clear('s');
                }
                if (cpu.reg_c == 0) {
                    cpu_flag_set('z');
                } else {
                    cpu_flag_clear('z');
                }
                if (cpu.reg_c == (uint8_t)0x1111) { //TODO: more generalized solution
                    cpu_flag_set('h');
                } else {
                    cpu_flag_clear('h');
                }
                if (cpu.reg_c == 0x80 - 1) {
                    cpu_flag_set('p');
                } else {
                    cpu_flag_clear('p');
                }
                cpu_flag_set('h');
                break;

            case 0x0E: // LD r, n where r = C
                cpu.reg_c = fetch_next();
                break;
            case 0x10: // DJNZ, e
                {
                    int8_t displacement = fetch_next();
                    cpu.reg_b--;
                    if (cpu.reg_b != 0) {
                        cpu.reg_pc += displacement;
                    }
                    break;
                }
            case 0x11: // LD dd, nn where dd = DE
                reg_pair_write("de", fetch_next16());
                break;
            case 0x18: // JR e
                cpu.reg_pc += fetch_next();
                break;
            case 0x1B: // DEC ss where ss = DE
                {
                    uint16_t de = reg_pair_read("de");
                    de--;
                    reg_pair_write("de", de);
                    break;
                }
            case 0x20: // JR NZ, e
                {
                    int8_t displacement = fetch_next(); // signed since can jump backwards
                    if(cpu_flag_test('z') == false) {
                        cpu.reg_pc += displacement;
                    }
                    break;
                }
            case 0x21: // LD dd, nn where dd = HL
                reg_pair_write("hl", fetch_next16());
                break;
            case 0x22: // LD (nn), HL
                {
                    uint16_t addr = fetch_next16();
                    mem_write(addr, cpu.reg_l);
                    addr++;
                    mem_write(addr, cpu.reg_h);
                    break;
                }
            case 0x28: // JR Z,e
                {
                    int8_t displacement = fetch_next(); // signed since can jump backwards
                    if (cpu_flag_test('z') == true) {
                            cpu.reg_pc += displacement;
                    }
                    break;
                }
            case 0x31: // LD dd, nn where dd = SP
                cpu.reg_sp = fetch_next16();
                break;
            case 0x32: // LD (nn), A
                mem_write(fetch_next16(), cpu.reg_a);
                break;

            case 0x36: // LD (HL), n
                {
                    uint8_t data = fetch_next();
                    uint16_t target = reg_pair_read("hl");
                    mem_write(target, data);
                    break;
                }
            case 0x3E: // LD r,n where r = A
                cpu.reg_a = fetch_next();
                break;
            case 0x77: // LD (HL), r where r = A
                mem_write(reg_pair_read("hl"), cpu.reg_a);
                break;
            case 0x79: // LD r,r' where r = A, r' = C
                cpu.reg_a = cpu.reg_c;
                break;
            case 0x7A: // LD r, r' where r = A, r' = D
                cpu.reg_a = cpu.reg_d;
                break;
            case 0x7B: // LD r,r' where r = A, r' = E
                cpu.reg_a = cpu.reg_e;
                break;
            case 0x7D: // LD r,r' where r = A, r' = L
                cpu.reg_a = cpu.reg_l;
                break;
            case 0x7E: // LD r,(HL) where r = A
                cpu.reg_a = mem_read(reg_pair_read("hl"));
                break;
            case 0xAF: // XOR s where s = A
                cpu.reg_a = 1;
                cpu.reg_a ^= cpu.reg_a;
                if ((int8_t)cpu.reg_a < 0) {
                    cpu_flag_set('s');
                } else {
                    cpu_flag_clear('s');
                }
                if (cpu.reg_a == 0) {
                    cpu_flag_set('z');
                } else {
                    cpu_flag_clear('z');
                }
                cpu_flag_clear('h');
                if (check_parity(cpu.reg_a)) {
                    cpu_flag_set('p');
                } else {
                    cpu_flag_clear('p');
                }
                cpu_flag_clear('n');
                cpu_flag_clear('c');
                break;

            case 0xB3: // OR s where s = E
                cpu.reg_a |= cpu.reg_e;
                if ((int8_t)cpu.reg_a < 0) {
                    cpu_flag_set('s');
                } else {
                    cpu_flag_clear('s');
                }
                if (cpu.reg_a == 0) {
                    cpu_flag_set('z');
                } else {
                    cpu_flag_clear('z');
                }
                cpu_flag_clear('h');
                cpu_flag_clear('p'); //TODO: docs say set if overflow?
                cpu_flag_clear('n');
                cpu_flag_clear('c');
                break;

            case 0xB7: // OR s where s = A
                cpu.reg_a |= cpu.reg_a;
                if ((int8_t)cpu.reg_a <0) {
                    cpu_flag_set('s');
                } else
                {
                    cpu_flag_clear('s');
                }
                if (cpu.reg_a == 0) {
                    cpu_flag_set('z');
                } else {
                    cpu_flag_clear('z');
                }
                cpu_flag_clear('h');
                cpu_flag_clear('p'); //TODO: docs say set if overflow?
                cpu_flag_clear('n');
                cpu_flag_clear('c');
                break;

            case 0xC9: // RET
                {
                    uint16_t addr_lsb = mem_read(cpu.reg_sp);
                    cpu.reg_sp++;
                    uint16_t addr_msb = mem_read(cpu.reg_sp);
                    cpu.reg_sp++;
                    cpu.reg_pc = addr_msb << 8;
                    cpu.reg_pc |= addr_lsb;
                    break;
                }
            case 0xCD: // CALL nn
                {
                    cpu_call(fetch_next16());
                    break;
                }
            case 0xD3: // OUT (n), A
                {
                    uint8_t port = fetch_next();
                    io_port_write(port, cpu.reg_a);
                    break;
                }
            case 0xE7: // RST p where p = 0x0020
                cpu_call(0x0020);
                break;
            case 0xEF: // RST p where p = 0x0028
                cpu_call(0x0028);
                break;
            case 0xED56: // IM 1
                cpu.iff1 = true;
                break;
            case 0xEDB0: // LDIR
                {
                    uint16_t hl = reg_pair_read("hl");
                    uint16_t de = reg_pair_read("de");
                    uint16_t bc = reg_pair_read("bc");
                    if (bc == 0)
                    {
                        bc = 0xFFFF
                    }
                    uint8_t data = mem_read(hl);
                    mem_write(de, data);
                    hl++;
                    de++;
                    bc--;
                    reg_pair_write("hl", hl);
                    reg_pair_write("de", de);
                    reg_pair_write("bc", bc);
                    if (reg_pair_read("bc") != 0) {
                        cpu.reg_pc -= 2;
                    }
                    cpu_flag_clear('h');
                    cpu_flag_clear('p');
                    cpu_flag_clear('n');
                    break;
                }
            case 0xEDB3: // OTIR
                {
                    //TODO: interrupts?
                    uint16_t addr = reg_pair_read("hl");
                    uint8_t data = mem_read(addr);
                    if (cpu.reg_b == 0)
                    {
                        cpu.reg_b = 0xFF
                    }
                    cpu.reg_b--;
                    io_port_write(cpu.reg_c, data);
                    addr++;
                    reg_pair_write("hl", addr);
                    if (cpu.reg_b != 0) {
                        cpu.reg_pc = cpu.reg_pc - 2;
                    }
                    cpu_flag_set('z');
                    cpu_flag_set('n');
                    break;
                }
            case 0xF3: // DI
                cpu.iff1 = false;
                cpu.iff2 = false;
                break;
            case 0xFB: // EI
                cpu.iff1 = true;
                cpu.iff2 = true;
                break;
            default:
                fprintf(stderr, "Unknown opcode: 0x%hX at 0x%hX\n", opcode, (uint16_t)(cpu.reg_pc-1));
                abort();
        }
}

void cpu_run(void) {
    cpu.reg_pc = 0;
    uint16_t opcode;

    while (1) {
        opcode = fetch_next();
        if (opcode == 0xED) {
            opcode <<= 8;
            opcode |= fetch_next();
        }
        printf("%hX: %hX (B:%hhX, A:%hhX)\n", (uint16_t)(cpu.reg_pc-1), opcode, cpu.reg_b, cpu.reg_a);
        exe_instr(opcode);
   }
}



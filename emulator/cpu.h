#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

void cpu_init(uint16_t);
void cpu_run(int);
bool cpu_interrupts_enabled(void);
bool cpu_irq(void);

#endif

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "../include/type.h"

typedef void (*interrupt_handler_t)(void);

void init_idt(void);
void register_interrupt_handler(uint8_t int_no, interrupt_handler_t handler);
void interrupt_handler(uint32_t int_no);
void enable_interrupts(void);
void disable_interrupts(void);

#endif


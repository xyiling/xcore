#ifndef PIC_H
#define PIC_H

#include "../include/type.h"

void init_pic(void);
void pic_send_eoi(uint8_t irq);

#endif


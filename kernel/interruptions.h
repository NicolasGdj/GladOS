//
// Created by cartignh on 08/06/2022.
//

#include <stdint.h>

#ifndef PROJET_SYS_INTERRUPTIONS_H
#define PROJET_SYS_INTERRUPTIONS_H

void interruptions_init();

void ISR_IT_32(void);
void ISR_IT_49(void);
void ISR_IT_0E(void);

void unmask_irq(uint8_t irq_n);

void init_isr_ITX(int it_number, void (*isr)(void), uint8_t dpl);

void clock_ISR();

void keyboard_ISR();

int syscall_handler(uint32_t fct_id, uint32_t parameter1, uint32_t parameter2, uint32_t parameter3, uint32_t parameter4, uint32_t parameter5);

#endif //PROJET_SYS_INTERRUPTIONS_H

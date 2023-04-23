//
// Created by cartignh on 08/06/2022.
//

#include "interruptions.h"
#include "segment.h"
#include "cpu.h"
#include <stdint.h>
#include "clock.h"
#include "../shared/syscalls.h"
#include "process.h"
#include "message_queue.h"
#include "../shared/console.h"
#include "boot/processor_structs.h"
#include "kbd.h"

void interruptions_init() {
    // Init clock ISR (32)
    init_isr_ITX(32, ISR_IT_32, 0);

    // Init gotokernel ISR (49)
    init_isr_ITX(49, ISR_IT_49, 3);

    // Init keyboard ISR (33)
    init_isr_ITX(33, ISR_IT_33, 0);

    // Init pagefault ISR (0E)
    init_isr_ITX(0x0E, ISR_IT_0E, 0);

    // Unmask IRQ 0
    unmask_irq(0);
    // Unmask IRQ 1 (keyboard)
    unmask_irq(1);
}

// irq_n: the number of the IRQ to unmask
// Example: IRQ 0 for the clock
void unmask_irq(uint8_t irq_n) {
    uint8_t mask = inb(0x21);
    // Set the irq_n bit to 0 (unmask; starting from the right)
    // and return the updated mask
    outb(mask & ~(1 << irq_n), 0x21);
}

void init_isr_ITX(int it_number, void (*isr)(void), uint8_t dpl) {
    if (dpl >= 4) return;
    // We get the entry 32 from the Interrupt Descriptor Table
    uint32_t *IDT_entry_X = (uint32_t *) (0x1000 + 2 * 4 * it_number);
    // Write the first word
    *(IDT_entry_X) = KERNEL_CS << 16 | (((uint32_t) isr) & 0xffff);
    // Write the second word
    // In C, + 1 automatically goes to the next "array cell"
    *(IDT_entry_X + 1) = (((uint32_t) isr) & 0xffff0000) | (0x8e00 +  /* DPL */ (dpl << 13));
}

void clock_ISR() {
    // Make the interruption controller immediately listen again for other interruptions
    outb(0x20, 0x20);

    clock_interruption_handler();
}

void keyboard_ISR() {
    outb(0x20, 0x20);

    keyboard_handler();
}

void page_fault_handler() {
    outb(0x20, 0x20);
    font_color(0xC);
    printf("Segmentation fault. Killed process ( pid = %d )\n", getpid());
    font_color(0xF);
    kill(getpid());
}


#define USER_START 0x1000000
#define USER_STACK_HEAP 0x2800000

int is_valid_userland_ptr(const void *ptr) {
    return ptr == NULL || (int)ptr >= USER_START || (int)ptr > USER_STACK_HEAP;
}

int syscall_handler(uint32_t fct_id, uint32_t parameter1, uint32_t parameter2, uint32_t parameter3, uint32_t parameter4,
                    uint32_t parameter5) {

    // Make the interruption controller immediately listen again for other interruptions
    outb(0x20, 0x20);

    (void) parameter1;
    (void) parameter2;
    (void) parameter3;
    (void) parameter4;
    (void) parameter5;

    switch (fct_id) {
        case EXIT:
            exit((int) parameter1);
        case KILL:
            return kill((int) parameter1);

        case CONSOLE_PUTBYTES:
            console_putbytes((char *) parameter1, (int) parameter2);
            break;
        case CONS_WRITE:
            if(!is_valid_userland_ptr((void*) parameter1)) {
                return -1;
            }
            cons_write((const char *) parameter1, (long) parameter2);
            break;
        case CONS_READ:
            if(!is_valid_userland_ptr((void*) parameter1)) {
                return -1;
            }
            return cons_read((char *) parameter1, (unsigned long) parameter2);
        case CONS_ECHO:
            cons_echo((int) parameter1);
            break;
        case GETPID:
            return getpid();
        case GETPRIO:
            return getprio((int) parameter1);
        case CHPRIO:
            return chprio((int) parameter1, (int) parameter2);
        case PCOUNT:
            if(!is_valid_userland_ptr((void*) parameter2)) {
                return -1;
            }
            return pcount((int) parameter1, (int *) parameter2);
        case PCREATE:
            return pcreate((int) parameter1);
        case PDELETE:
            return pdelete((int) parameter1);
        case PRECEIVE:
            if(!is_valid_userland_ptr((void*) parameter2)) {
                return -1;
            }
            return preceive((int) parameter1, (int *) parameter2);
        case PSEND:
            return psend((int) parameter1, (int) parameter2);
        case PRESET:
            return preset((int) parameter1);
        case CLOCK_SETTINGS:
            if(!is_valid_userland_ptr((void*) parameter1) || !is_valid_userland_ptr((void*) parameter2)) {
                return -1;
            }
            clock_settings((unsigned long *) parameter1, (unsigned long *) parameter2);
            break;
        case CURRENT_CLOCK:
            return current_clock();
        case WAIT_CLOCK:
            wait_clock((unsigned long) parameter1);
            break;
        case START:
            if(!is_valid_userland_ptr((void*) parameter1) || !is_valid_userland_ptr((void*) parameter4)) {
                return -1;
            }
            return start((int (*)(void *)) parameter1, (unsigned long) parameter2, (int) parameter3, (char*) parameter4, (void*) parameter5);
        case WAITPID:
            if(!is_valid_userland_ptr((void*) parameter2)) {
                return -1;
            }
            return waitpid((int)parameter1, (int*)parameter2);
        case REBOOT:
            reboot();
            break;
        case SLEEP:
            sleep((unsigned long) parameter1);
            break;
        case SYSINFO:
            sysinfo();
            break;
        case WAITPID_NOHANG:
            if(!is_valid_userland_ptr((void*) parameter2)) {
                return -1;
            }
            return waitpid_nohang((int)parameter1, (int*)parameter2);
        case PRINT_PROMPT:
            if(!is_valid_userland_ptr((void*) parameter1)) {
                return -1;
            }
            print_prompt((char *) parameter1);
            break;
        case CLEAR:
            clear_screen();
            break;
        case FONT_COLOR:
            return font_color((uint32_t) parameter1);
        case FONT_BACKGROUND_COLOR:
            return font_background_color((uint32_t) parameter1);
        default:
            break;
    }
    return 0;
}
//
// Created by cartignh on 20/06/2022.
//

#include "debug.h"
#include "./syscalls.h"
#include "../shared/syscalls.h"
#include <stdint.h>

int chprio(int pid, int newprio) {
    return syscall(CHPRIO, (uint32_t) pid, (uint32_t) newprio, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void console_putbytes(const char *str, int len) {
    syscall(CONSOLE_PUTBYTES, (uint32_t) str, (uint32_t) len, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int cons_read(char *string, unsigned long length) {
    return syscall(CONS_READ, (uint32_t) string, (uint32_t) length, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void cons_write(const char *str, long size) {
    syscall(CONS_WRITE, (uint32_t) str, (uint32_t) size, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void cons_echo(int on) {
    syscall(CONS_ECHO, (uint32_t) on, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void exit(int retval) {
    syscall(EXIT, (uint32_t) retval, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);

    while (1) { assert(0); }
}

int getpid(void) {
    return syscall(GETPID, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int getprio(int pid) {
    return syscall(GETPRIO, (uint32_t) pid, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int kill(int pid) {
    return syscall(KILL, (uint32_t) pid, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int pcount(int fid, int *count) {
    return syscall(PCOUNT, (uint32_t) fid, (uint32_t) count, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int pcreate(int count) {
    return syscall(PCREATE, (uint32_t) count, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int pdelete(int fid) {
    return syscall(PDELETE, (uint32_t) fid, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int preceive(int fid, int *message) {
    return syscall(PRECEIVE, (uint32_t) fid, (uint32_t) message, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int preset(int fid) {
    return syscall(PRESET, (uint32_t) fid, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int psend(int fid, int message) {
    return syscall(PSEND, (uint32_t) fid, (uint32_t) message, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void clock_settings(unsigned long *quartz, unsigned long *ticks) {
    syscall(CLOCK_SETTINGS, (uint32_t) quartz, (uint32_t) ticks, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

unsigned long current_clock(void) {
    return syscall(CURRENT_CLOCK, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void wait_clock(unsigned long wakeup) {
    syscall(WAIT_CLOCK, (uint32_t) wakeup, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg) {
    return syscall(START, (uint32_t) ptfunc, (uint32_t) ssize, (uint32_t) prio, (uint32_t) name, (uint32_t) arg);
}

int waitpid(int pid, int *retval) {
    return syscall(WAITPID, (uint32_t) pid, (uint32_t) retval, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int waitpid_nohang(int pid, int *retval) {
    return syscall(WAITPID_NOHANG, (uint32_t) pid, (uint32_t) retval, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int reboot() {
    return syscall(REBOOT, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int sleep(unsigned long time) {
    return syscall(SLEEP, (uint32_t) time,(uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

int sys_info() {
    return syscall(SYSINFO, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

void print_prompt(char *prompt) {
    syscall(PRINT_PROMPT, (uint32_t) prompt, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}
void clear() {
    syscall(CLEAR, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

uint32_t font_color(uint32_t new) {
    return syscall(FONT_COLOR, (uint32_t) new, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}

uint32_t font_background_color(uint32_t new) {
    return syscall(FONT_BACKGROUND_COLOR, (uint32_t) new, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0, (uint32_t) 0);
}
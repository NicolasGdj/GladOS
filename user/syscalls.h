//
// Created by cartignh on 20/06/2022.
//

#ifndef PROJET_SYS_SYSCALLS_H
#define PROJET_SYS_SYSCALLS_H

#include <stdint.h>

extern int syscall(uint32_t fct_id, uint32_t parameter1, uint32_t parameter2, uint32_t parameter3, uint32_t parameter4, uint32_t parameter5);

int chprio(int pid, int newprio);
void console_putbytes(const char *str, int len);
int cons_read(char *string, unsigned long length);
void cons_write(const char *str, long size);
void cons_echo(int on);
void exit(int retval);
int getpid(void);
int getprio(int pid);
int kill(int pid);
int pcount(int fid, int *count);
int pcreate(int count);
int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);
int psend(int fid, int message);
void clock_settings(unsigned long *quartz, unsigned long *ticks);
unsigned long current_clock(void);
void wait_clock(unsigned long wakeup);
int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg);
int waitpid(int pid, int *retval);
int sleep(unsigned long time);
int reboot();
int sys_info();
int waitpid_nohang(int pid, int *retval);
void print_prompt(char *prompt);
void clear();
uint32_t font_color(uint32_t new);
uint32_t font_background_color(uint32_t new);
#endif //PROJET_SYS_SYSCALLS_H

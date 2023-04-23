//
// Created by cartignh on 20/06/2022.
//

#ifndef PROJET_SYS_SYSCALLS_IDS_H
#define PROJET_SYS_SYSCALLS_IDS_H

enum Syscalls {
    EXIT = 0,
    KILL = 1,
    CONSOLE_PUTBYTES = 2,
    CONS_WRITE = 3,
    CONS_READ = 4,
    CONS_ECHO = 5,
    GETPID = 6,
    GETPRIO = 7,
    CHPRIO = 8,
    PCOUNT = 9,
    PCREATE = 10,
    PDELETE = 11,
    PRECEIVE = 12,
    PSEND = 13,
    PRESET = 14,
    CLOCK_SETTINGS = 15,
    CURRENT_CLOCK = 16,
    WAIT_CLOCK = 17,
    START = 18,
    WAITPID = 19,
    REBOOT = 20,
    SLEEP = 21,
    SYSINFO = 22,
    CLEAR = 23,
    WAITPID_NOHANG = 24,
    PRINT_PROMPT = 25,
    FONT_COLOR = 26,
    FONT_BACKGROUND_COLOR = 27,
};


#endif //PROJET_SYS_SYSCALLS_IDS_H

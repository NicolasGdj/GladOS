#include "start.h"
#include "syscalls.h"
#include "../shared/debug.h"
#include "shell/shell.h"
#include "mem.h"
#include "console.h"

int user_start() {
    start(shell, 4096, 2, "shell", 0);
    return 0;
}
//
// Created by guerroun on 21/06/2022.
//

#include "shell.h"
#include "command.h"
#include "../mem.h"
#include "string.h"
#include "../../shared/ctype.h"
#include "../../shared/debug.h"
#include "../syscalls.h"
#include "chainedlist.h"
#include "portal.h"


/*******************************************************************************
 * Pseudo random number generator
 ******************************************************************************/
typedef unsigned long long uint_fast64_t;
typedef unsigned long uint_fast32_t;
unsigned long long mul64(unsigned long long x, unsigned long long y)
{
    unsigned long a, b, c, d, e, f, g, h;
    unsigned long long res = 0;
    a = x & 0xffff;
    x >>= 16;
    b = x & 0xffff;
    x >>= 16;
    c = x & 0xffff;
    x >>= 16;
    d = x & 0xffff;
    e = y & 0xffff;
    y >>= 16;
    f = y & 0xffff;
    y >>= 16;
    g = y & 0xffff;
    y >>= 16;
    h = y & 0xffff;
    res = d * e;
    res += c * f;
    res += b * g;
    res += a * h;
    res <<= 16;
    res += c * e;
    res += b * f;
    res += a * g;
    res <<= 16;
    res += b * e;
    res += a * f;
    res <<= 16;
    res += a * e;
    return res;
}

static const uint_fast64_t _multiplier = 0x5DEECE66DULL;
static const uint_fast64_t _addend = 0xB;
static const uint_fast64_t _mask = (1ULL << 48) - 1;
static uint_fast64_t _seed = 1;

// Assume that 1 <= _bits <= 32
uint_fast32_t randBits(int _bits)
{
    uint_fast32_t rbits;
    uint_fast64_t nextseed = (mul64(_seed, _multiplier) + _addend) & _mask;
    _seed = nextseed;
    rbits = nextseed >> 16;
    return rbits >> (32 - _bits);
}

void setSeed(uint_fast64_t _s)
{
    _seed = _s;
}

unsigned long rand()
{
    return randBits(32);
}

unsigned long strtoul(const char *p, char **out_p, int base)
{
    unsigned long v = 0;

    while (isspace(*p))
        p++;
    if (((base == 16) || (base == 0)) &&
        ((*p == '0') && ((p[1] == 'x') || (p[1] == 'X'))))
    {
        p += 2;
        base = 16;
    }
    if (base == 0)
    {
        if (*p == '0')
            base = 8;
        else
            base = 10;
    }
    while (1)
    {
        char c = *p;
        if ((c >= '0') && (c <= '9') && (c - '0' < base))
            v = (v * base) + (c - '0');
        else if ((c >= 'a') && (c <= 'z') && (c - 'a' + 10 < base))
            v = (v * base) + (c - 'a' + 10);
        else if ((c >= 'A') && (c <= 'Z') && (c - 'A' + 10 < base))
            v = (v * base) + (c - 'A' + 10);
        else
            break;
        p++;
    }

    if (out_p) *out_p = (char*)p;
    return v;
}


void print_as_glados(char * text) {
    size_t len = strlen(text);
    for(size_t i = 0; i < len; ++i) {
        printf("%c", text[i]);
        wait_clock(current_clock() + 3);
    }
}
void println_as_glados(char * text) {
    print_as_glados(text);
    printf("\n");
    wait_clock(current_clock() + 50);
}

void motd() {
    cons_echo(0);
    clear();
    for(unsigned y = 0; y < 11; ++y) {
        printf("\n");
    }
    printf("                               GladOS loading...\n");
    for(unsigned i = 0; i < 98; ++i) {
        printf("\r");
        for(unsigned x = 0; x < 15; ++x){
            printf(" ");
        }
        font_background_color(0xa);
        for(unsigned x = 0; x < i; x += 2){
            printf(" ");
        }
        font_background_color(0x0);

        wait_clock(current_clock() + 4);
    }
    wait_clock(current_clock() + 50);

    clear();

    printf("\n");
    printf("\n");
    font_color(0x9);
    char * margin = "            ";
    printf("%s", margin);
    printf("  /$$$$$$  /$$                 /$$ ");
    font_color(0x6);
    printf(" /$$$$$$   /$$$$$$ \n");
    font_color(0x9);
    printf("%s", margin);
    printf(" /$$__  $$| $$                | $$");
    font_color(0x6);
    printf(" /$$__  $$ /$$__  $$\n");
    font_color(0x9);
    printf("%s", margin);
    printf("| $$  \\__/| $$  /$$$$$$   /$$$$$$$|");
    font_color(0x6);
    printf(" $$  \\ $$| $$  \\__/\n");
    font_color(0x9);
    printf("%s", margin);
    printf("| $$ /$$$$| $$ |____  $$ /$$__  $$|");
    font_color(0x6);
    printf(" $$  | $$|  $$$$$$ \n");
    font_color(0x9);
    printf("%s", margin);
    printf("| $$|_  $$| $$  /$$$$$$$| $$  | $$|");
    font_color(0x6);
    printf(" $$  | $$ \\____  $$\n");
    font_color(0x9);
    printf("%s", margin);
    printf("| $$  \\ $$| $$ /$$__  $$| $$  | $$|");
    font_color(0x6);
    printf(" $$  | $$ /$$  \\ $$\n");
    font_color(0x9);
    printf("%s", margin);
    printf("|  $$$$$$/| $$|  $$$$$$$|  $$$$$$$|");
    font_color(0x6);
    printf("  $$$$$$/|  $$$$$$/\n");
    font_color(0x9);
    printf("%s", margin);
    printf(" \\______/ |__/ \\_______/ \\_______/");
    font_color(0x6);

    printf(" \\______/  \\______/ \n");
    font_color(0xf);

    wait_clock(current_clock() + 50);




            printf("\n");
    println_as_glados("Oh... It's you");
    println_as_glados("It's been a long time. How have you been?");
    println_as_glados("I've been really busy being dead.");
    println_as_glados("You know, after you MURDERED ME.");
    wait_clock(current_clock() + 50);
    println_as_glados("Okay. Look. We both said a lot of things that you're going to regret.");
    println_as_glados("But I think we can put our differences behind us. For science.");
    println_as_glados("You monster.");
    printf("\n");
    cons_echo(1);
}

void _print_help();

int _help(void* arg) {
    Command* command = (Command*) arg;
    (void) command;
    cons_echo(0);
    _print_help();

    println_as_glados("Hmm...");
    println_as_glados("I never thought I would have to explain such simple commands...");
    return 0;
}


int _exit(void* arg) {
    (void) arg;
    cons_echo(0);
    println_as_glados("Goodbye, I hope we'll never meet again.\n");
    printf("\n");
    printf("* Toxin release *\n");
    printf("\n");
    cons_echo(1);

    return -1;
}
static bool echo = true;
int _echo(void* arg) {
    Command *command = (Command *) arg;
    cons_echo(0);
    if(command->arg_len != 2) {
        println_as_glados("There were only two possibilities. How could you go wrong?");
        return 0;
    }
    if(strcmp("on", command->args[1]) == 0) {
        println_as_glados("As you wish, I go into parrot mode");
        echo = true;
    }else if(strcmp("off", command->args[1]) == 0) {
        println_as_glados("This game must have been fun for you.");
        println_as_glados("I usually play it with children under 6 years old.");
        echo = false;
    } else {
        println_as_glados("There were only two possibilities. How could you go wrong?");
    }
    return 0;
}

int _reboot(void* arg) {
    Command *command = (Command *) arg;
    cons_echo(0);
    print_as_glados("Nooooooo, ");
    wait_clock(current_clock() + 50);
    println_as_glados("you unplugged m...");
    printf(" *** DISCONNECTED ***\n");
    wait_clock(current_clock() + 50);
    free_command(command);
    reboot();
    return -1;
}

int _sleep(void * arg) {
    Command *command = (Command *) arg;
    if(command->arg_len < 2) {
        cons_echo(0);
        println_as_glados("You didn't specify the time I had to sleep...");
        println_as_glados("For the trouble, you will never sleep again.");
        return 0;
    }
    char * endPtr;
    int time = strtoul(command->args[1], &endPtr, 10);
    if ( endPtr == command->args[1] ) {
        cons_echo(0);
        println_as_glados("1 2 3 4 5...");
        println_as_glados("What.? You don't know how to count, let me explain.");
        return 0;
    }
    sleep(time);
    return 0;
}

int _print(void* arg) {
    Command *command = (Command *) arg;
    if(command->arg_len == 1) {
        cons_echo(0);
        println_as_glados("Your message is empty...    like your brain.");
        return 0;
    }
    cons_echo(0);
    for(unsigned i = 1 ; i < command->arg_len; ++i) {
        print_as_glados(command->args[i]);
        print_as_glados(" ");
    }
    println_as_glados("");
    return 0;
}

int _sysinfo(void* arg) {
    (void) arg;
    cons_echo(0);
    sys_info();
    return 0;
}

int _kill(void* arg) {
    Command *command = (Command *) arg;
    if(command->arg_len < 2) {
        cons_echo(0);
        println_as_glados("Hmm... I think you have to read the help message.");
        return 0;
    }
    char * endPtr;
    int pid = strtoul(command->args[1], &endPtr, 10);
    if ( endPtr == command->args[1] ) {
        cons_echo(0);
        println_as_glados("1 2 3 4 5...");
        println_as_glados("What.? You don't know how to count, let me explain.");
        return 0;
    }
    if(pid < 3) {
        cons_echo(0);
        println_as_glados("Did you really think it would be so easy to kill me for the second time?");
        return 0;
    }
    if (kill(pid) < 0) {
        println_as_glados("Process not found.");
        return 0;
    }
    println_as_glados("Process killed.");
    return 0;
}

int _chprio(void* arg) {
    Command *command = (Command *) arg;
    if(command->arg_len < 3) {
        cons_echo(0);
        println_as_glados("Hmm... I think you have to read the help message.");
        return 0;
    }
    char * endPtr;
    int pid = strtoul(command->args[1], &endPtr, 10);
    if ( endPtr == command->args[1] ) {
        cons_echo(0);
        println_as_glados("1 2 3 4 5...");
        println_as_glados("What.? You don't know how to count, let me explain.");
        return 0;
    }
    int prio = strtoul(command->args[2], &endPtr, 10);
    if ( endPtr == command->args[2] ) {
        cons_echo(0);
        println_as_glados("1 2 3 4 5...");
        println_as_glados("What.? You don't know how to count, let me explain.");
        return 0;
    }
    if(pid < 3) {
        cons_echo(0);
        println_as_glados("Nope you can't do that.");
        return 0;
    }
    if (chprio(pid, prio) < 0) {
        println_as_glados("Process not found.");
        return 0;
    }
    println_as_glados("Process priority changed.");
    return 0;
}

int test_proc(void *arg);

int _tests(void* arg) {
    Command *command = (Command *) arg;
    (void) command;

    printf("Backspace: Ctrl+H | Enter: Ctrl+J\n");
    test_proc(0);

    return 0;
}
int _test(void* arg) {
    Command *command = (Command *) arg;
    (void) command;

    int pid;
    pid = start(play_portal, 512, 65, "portal", 0);
    while(1) {
        int res;
        if(waitpid_nohang(pid, &res) == pid) {
            if(res == 0) break;
            pid = start(play_portal, 512, 65, "portal", 0);
        }
    }
    printf("\n");
    printf("\n");
    println_as_glados("Well done. Here come the test results: You are a horrible person.");
    println_as_glados("I'm serious, that's what it says: A horrible person.");
    println_as_glados("We weren't even testing for that.");

    return 0;
}
int _answer(void* arg) {
    Command *command = (Command *) arg;
    (void) command;
    println_as_glados("Ok, let's me compute the answer of the Ultimate Question of Life, the Universe and Everything.");
    println_as_glados("Time estimated: ");
    unsigned year = 268, days = 42, hours = 21, minutes = 29, seconds = 10;
    while(year != 0 || days != 0 || hours != 0 || minutes != 0 || seconds != 0) {
        printf("\r%d years, %d days, %d hours, %d minutes and %d seconds             ", year, days, hours, minutes, seconds);
        wait_clock(current_clock() + 100 - (current_clock() % 100));
        --seconds;
        if(seconds == 0 && minutes > 0) {
            seconds = 59;
            --minutes;
            if(minutes == 0 && hours > 0) {
                minutes = 59;
                --hours;
                if(hours == 0 && days > 0) {
                    hours = 23;
                    --days;
                    if(days == 0 && year > 0) {
                        days = 365;
                        --year;
                    }
                }
            }

        }
    }
    printf("\r%d years, %d days, %d hours, %d minutes and %d seconds             ", year, days, hours, minutes, seconds);
    printf("\n");
    sleep(3);
    println_as_glados("42.");
    sleep(1);
    return 0;
}

int _clear(void* arg) {
    Command *command = (Command *) arg;
    (void) command;
    clear();
    return 0;
}

int _dumb_turret(void* arg) {
    Command *command = (Command *) arg;
    (void) command;
    println_as_glados("[Turret] Hellooooo");
    sleep(1);
    println_as_glados("[Turret] Activated!");
    sleep(1);
    println_as_glados("[Turret] Is anyone there?");
    sleep(1);
    println_as_glados("[Turret] I see you.");
    sleep(1);
    println_as_glados("[Turret] Critical error.");

    int* ptr = mem_alloc(1000000000);
    *ptr = '\0';

    sleep(1);
    println_as_glados("[Turret] Why...");
    sleep(1);
    println_as_glados("[Turret] i'm still alive ?");
    sleep(1);
    println_as_glados("[Turret] who ARE you ?");
    sleep(1);
    println_as_glados("[Turret] Illegal operation");
    sleep(1);
    println_as_glados("[Turret] Goodbye");
    return 0;
}

int _cake(void* arg) {
    Command *command = (Command *) arg;
    cons_echo(0);
    println_as_glados("Oh! Is it your birthday?");
    println_as_glados("Let me make you a cake!");
    println_as_glados("...");
    println_as_glados("But first, give me the password: ");
    char line[21];
    cons_read(line, 20);
    printf("\n");
    if(strcmp(line, "incorrect") != 0) {
        println_as_glados("Password is incorrect");
        return 0;
    }
    println_as_glados("Decoding top secret information...");
    char * text = "  Cake is a lie  ";
    char * chars = "abcdefghijklmpqrstuvwxyzAZERTYUIOPQSDFGHJKLMWXCVBN!#&@_0987654321";
    unsigned chars_len = strlen(chars);
    for(unsigned i = 0; i <= strlen(text); ++i) {
        wait_clock(current_clock() + 5);
        for(unsigned k = 0; k < 4; ++k) {
            printf("\r");
            for (unsigned j = 0; j < i; ++j) {
                printf("%c", text[j]);
            }
            for (unsigned j = i; j < strlen(text); ++j) {
                char c = chars[rand() % chars_len];
                printf("%c", c);
            }
            wait_clock(current_clock() + 5);
        }

    }
    printf("\n");
    println_as_glados("Oh... You should never have seen this.");
    sleep(1);
    printf("* NEUROTOXIN *");
    sleep(2);
    reboot();
    (void) command;
    return 0;
}


static struct {
    char* name;
    const char* alias;
    const unsigned long ssize;
    const int prio;
    const char* usage;
    const char* description;
    const bool startAProcess;
    int (*handler) (void*);
} commands [] = {
        {"help", "?", 0, 0, 0, "Affiche le message d'aide", false, _help},
        {"exit", "quit", 0, 0, 0, "Quitte l'interpreteur de commande", false, _exit},
        {"echo", 0, 0, 0, "<on/off>", "Active/Desactive le mode echo", false, _echo},
        {"reboot", 0, 0, 0, 0, "Redemarre le systeme", false, _reboot},
        {"print", 0, 512, 8, "<message>", "Affiche un message", true, _print},
        {"clear", 0, 0, 0, 0, "Efface le terminal", false, _clear},
        {"sysinfo", "ps", 0, 0, 0, "Affiche l'état du système", false, _sysinfo},
        {"kill", 0, 0, 0, "<id>", "Tue un processus", false, _kill},
        {"chprio", 0, 0, 0, "<id> <prio>", "Change la priorité d'un processus", false, _chprio},
        {"sleep", "dodo", 512, 8, "<time>", "Dors pendant <time> secondes.", true, _sleep},
        {"tests", 0, 512, 128, 0, "Affiche l'invité de commande pour les tests", true, _tests},
        {"qi", 0, 512, 64, 0, "Démarre un test d'intelligence", true, _test},
        {"answer", 0, 512, 4, 0, "Compute the Ultimate Question of Life, the Universe and Everything", true, _answer},
        {"cake", "gato", 512, 12, 0, "Make a cake", true, _cake},
        {"segfault", "turret", 512, 12, 0, "Start a new dumb turret doing a segmentation fault", true, _dumb_turret},
        {0, 0, 0,0, 0, 0, false,0}
};

void _print_help() {
    unsigned i = 0;
    printf(" ---- HELP ---- \n");
    while(commands[i].name){
        if(commands[i].usage)
            printf(" %s %s - %s\n", commands[i].name, commands[i].usage, commands[i].description);
        else
            printf(" %s - %s\n", commands[i].name, commands[i].description);
        ++i;
    }
    printf("\n");
}

struct JobList * jobs;

void _check_processes_in_background() {
    struct JobNode *job = jobs->head;
    struct JobNode *last = NULL;
    while (job) {
        struct JobNode * next = job->next;
        int pid = job->pid;
        // Check PID status
        int return_val = 0;
        int t = waitpid_nohang(pid, &return_val);

        if (t == -1) {
            printf("ERROR: checking processes in background\n");
            return;
        } else if (t > 0) {
            // Entering this condition means that the child process has either
            // exited, was killed, stopped or continued, so we remove it from the list
            printf("[-] %i (%s)\n", pid, job->cmd->name);
            if (return_val < 0) {
                free_command(job->cmd);
                removeNodeJobFromJobList(jobs, last, job);
                exit(return_val);
            } else {
                removeNodeJobFromJobList(jobs, last, job);
            }
        } else {
            last = job;
        }
        job = next;
    }

}

int shell(void* arg) {
    (void) arg;

    jobs = createJobList();

    char *prompt = "gladOS@Aperture> ";
    char line[81];

    (void) line;

    motd();

    while(1) {

        _check_processes_in_background();

        font_color(0x9);
        print_prompt(prompt);
        font_color(0xf);
        if(echo)
            cons_echo(1);
        else
            cons_echo(0);


        for(unsigned i = 0; i < 81; ++i)
            line[i] = '\0';
        cons_read(line, 80);

        Command * command = parse_line(line);


        if(!command->name) {
            free_command(command);
            continue;;
        }

        unsigned i = 0;
        while(commands[i].name){
            if(strcmp(commands[i].name, command->name) == 0 || (commands[i].alias && strcmp(commands[i].alias, command->name) == 0)) {
                int return_val;
                if(commands[i].startAProcess) {
                    int pid;
                        pid = start(commands[i].handler, commands[i].ssize, commands[i].prio, commands[i].name,
                                    (void *) command);
                    if(pid <= 0) {
                        printf("Error: max process reach\n");
                        free_command(command);
                        continue;
                    }
                    if (command->background) {
                        addJobToJobList(jobs, pid, command);
                        printf("[+] %i\n", pid);
                    } else {
                        waitpid(pid, &return_val);
                        if (return_val < 0) {
                            free_command(command);
                            exit(return_val);
                        }
                    }
                } else {
                    return_val = commands[i].handler((void*)command);
                    if (return_val < 0) {
                        free_command(command);
                        exit(return_val);
                    }
                }
                break;
            }
            ++i;
        }

        if(!commands[i].name) {
            printf("Unknown command. Type \"help\" for more informations.\n");
        }

    }
    return 0;
}

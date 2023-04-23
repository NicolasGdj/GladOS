//
// Created by guerroun on 21/06/2022.
//

#ifndef PSYS_BASE_COMMAND_H
#define PSYS_BASE_COMMAND_H

#include "stdbool.h"

#define NB_ARGS 20

typedef struct command{
    char * name;
    char ** args;
    unsigned arg_len;
    bool background;
} Command;

char* read_word(char * line, unsigned start, unsigned end);

void add_word(Command* cmd , char* word);

Command* parse_line(char * line);

void free_command(Command* cmd);

#endif //PSYS_BASE_COMMAND_H

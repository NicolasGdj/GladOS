//
// Created by guerroun on 21/06/2022.
//

#include "command.h"
#include "../../shared/string.h"
#include "../mem.h"
char* read_word(char * line, unsigned start, unsigned end) {
    unsigned word_len = end - start;
    char *word = mem_alloc(word_len * sizeof(char) + 1);
    for (unsigned y = 0; y < word_len; ++y) {
        word[y] = line[start + y];
    }
    word[word_len] = '\0';
    return word;
}

void add_word(Command* cmd , char* word) {
    if(strlen(word) <= 1 && (word[0] == '\0' || word[0] == ' ')) return;
    if(cmd->arg_len == NB_ARGS - 1) return;
    if(cmd->arg_len == 0)
        cmd->name = word;
    cmd->args[cmd->arg_len++] = word;
}

Command* parse_line(char * line) {

    Command* cmd = mem_alloc(sizeof (Command));
    cmd->arg_len = 0;
    cmd->args = mem_alloc(NB_ARGS * sizeof(char*));

    char current_char;
    unsigned i = 0;
    unsigned last_word = i;
    while((current_char = line[i]) != '\0') {
        if(current_char == '&') {
            cmd->background = true;
            break;
        }else if(current_char == ' ') {
            if(cmd->arg_len < NB_ARGS - 2 ) {
                add_word(cmd, read_word(line, last_word, i));
                last_word = i + 1;
            } else {
               ++i;
                continue;
            }
        }
        ++i;
    }
    add_word(cmd, read_word(line, last_word, i));

    return cmd;
}

void free_command(Command* cmd) {

    for(unsigned i = 0; i < cmd->arg_len; ++i) {
        mem_free(cmd->args[i], strlen(cmd->args[i]) * sizeof (char) + 1);
    }
    mem_free(cmd->args, NB_ARGS * sizeof(char*));
    mem_free(cmd, sizeof (Command));
}



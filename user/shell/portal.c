//
// Created by guerroun on 23/06/2022.
//

#include "../../shared/debug.h"
#include "../syscalls.h"
#include "chainedlist.h"
#include "portal.h"

#include "portal.h"

#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 13
#define GRAVITY_TICK 2
#define TURRET_TICK 75

typedef struct location{
    unsigned x;
    unsigned y;
} Location;

//Map
char map[SCREEN_WIDTH][SCREEN_HEIGHT] = {};

// Player
Location player_location = {1, 2};
bool player_jump = false;
char player_char = '>';

// Portals

bool portal_blue_exist = false;
bool portal_blue_in_place = false;
Location portal_blue_location = {0, 0};
int portal_blue_dir = 0;

bool portal_orange_exist = false;
bool portal_orange_in_place = false;
Location portal_orange_location = {0, 0};
int portal_orange_dir = 0;

//Turret

Location turret_location = {-1,-1};
int turret_dir = -1;

void fill_row_map(unsigned row, char * str) {
    for(unsigned i = 0; i < strlen(str); ++i) {
        map[i][row] = str[i];
        if(str[i] == 'T'){
            turret_location.x = i;
            turret_location.y = row;
        }
    }
}

void init_map() {
    player_location.x = 1;
    player_location.y = 3;
    fill_row_map(12, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    fill_row_map(11, "Y           XY                                   V");
    fill_row_map(10, "Y        XX XY              XY  YYY  Y  Y Y YYY  V");
    fill_row_map(9,  "Y        YX XY              YY  Y   Y-Y YY  Y-   V");
    fill_row_map(8,  "Y      XXXX XY           XXXXY  YYY Y Y Y Y YYY  V");
    fill_row_map(7,  "Y        YX XY              XY                   V");
    fill_row_map(6,  "XXX      YX XXXXXX          XY    Y   Y  YYY     V");
    fill_row_map(5,  "XXX      YX X               YY    Y   Y  Y-      V");
    fill_row_map(4,  "Y      XXXX X              XXY    YYY Y  YYY     V");
    fill_row_map(3,  "Y        YY        T        XY                   V");
    fill_row_map(2,  "XXXXXXXXXXXXXXXXXXXX        XXXXXXXXXXXXXXXXXXXXXX");
    fill_row_map(1,  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
}

int keyboard_wait_queue;
int keyboard_detection(void * arg) {
    (void) arg;
    while(1) {
        int msg;
        preceive(keyboard_wait_queue, &msg);
        preset(0);
        char c;
        cons_read(&c, 1);
        switch (c) {
            case 'd':
                if(player_char != '>') {
                    player_char = '>';
                    break;
                }
                if(player_location.x < SCREEN_WIDTH) {
                    if(map[player_location.x + 1][player_location.y] == ' ')
                        ++player_location.x;
                    if(map[player_location.x + 1][player_location.y] == 'V'){
                        return 1;
                    }
                    else if(player_location.x + 1 == portal_blue_location.x && player_location.y == portal_blue_location.y) {
                        if(portal_orange_in_place) {
                            player_location.x = portal_orange_location.x - portal_orange_dir;
                            player_location.y = portal_orange_location.y;
                            if(portal_orange_dir > 0)
                                player_char = '<';
                            else
                                player_char = '>';
                        }
                    } else if(player_location.x + 1 == portal_orange_location.x && player_location.y == portal_orange_location.y) {
                        if(portal_blue_in_place) {
                            player_location.x = portal_blue_location.x - portal_blue_dir;
                            player_location.y = portal_blue_location.y;
                            if(portal_blue_dir > 0)
                                player_char = '<';
                            else
                                player_char = '>';
                        }
                    }
                }
                break;
            case 'q':
                if(player_char != '<') {
                    player_char = '<';
                    break;
                }
                if(player_location.x > 0) {
                    if(map[player_location.x - 1][player_location.y] == ' ')
                        --player_location.x;
                    else if(player_location.x - 1 == portal_blue_location.x && player_location.y == portal_blue_location.y) {
                        if(portal_orange_in_place) {
                            player_location.x = portal_orange_location.x - portal_orange_dir;
                            player_location.y = portal_orange_location.y;
                            if(portal_orange_dir > 0)
                                player_char = '<';
                            else
                                player_char = '>';
                        }
                    } else if(player_location.x - 1 == portal_orange_location.x && player_location.y == portal_orange_location.y) {
                        if(portal_blue_in_place) {
                            player_location.x = portal_blue_location.x - portal_blue_dir;
                            player_location.y = portal_blue_location.y;
                            if(portal_blue_dir > 0)
                                player_char = '<';
                            else
                                player_char = '>';
                        }
                    }
                }
                break;
            case 'e':
                if(player_char == '<') {
                    portal_blue_dir = -1;
                } else if(player_char == '>') {
                    portal_blue_dir = 1;
                }
                portal_blue_exist = true;
                portal_blue_in_place = false;
                portal_blue_location.x = player_location.x;
                portal_blue_location.y = player_location.y;
                break;
            case 'a':
                if(player_char == '<') {
                    portal_orange_dir = -1;
                } else if(player_char == '>') {
                    portal_orange_dir = 1;
                }
                portal_orange_exist = true;
                portal_orange_in_place = false;
                portal_orange_location.x = player_location.x;
                portal_orange_location.y = player_location.y;
                break;
            case 'r':
                portal_blue_exist = false;
                portal_blue_in_place = false;
                portal_orange_exist = false;
                portal_orange_in_place = false;
                break;
            case ' ': {
                if(player_jump)
                    break;

                player_jump = true;
                unsigned i = 0;
                while (i < 2) {
                    if (player_location.y + i +1 >= SCREEN_HEIGHT || map[player_location.x][player_location.y + i + 1] != ' ') {

                        break;
                    }
                    ++i;
                }
                player_location.y = player_location.y + i;
                break;
            }
            case '\3': //Ctrl + C
                return 0;
        }
    }
}

unsigned long start_clock = 0;


int display() {
    unsigned tick = 0;
    start_clock = current_clock();
    while(1) {

        int count;
        pcount(keyboard_wait_queue, &count);
        if(count <= 0)
            psend(keyboard_wait_queue, 0);
        ++tick;
        clear();
        for(unsigned y = 0; y < 10; ++y)
            printf("\n");
        printf("Press Ctrl+C to quit.\n");
        printf("Q: Move to left | D: Move to right | Space: Jump\n");
        printf("A: Throw a orange portal | E: Throw a blue portal | R: Reset portal\n");
        printf("\n");
        printf("Time: %lds\n", (current_clock() - start_clock) / 100);
        for(unsigned y = SCREEN_HEIGHT; y != 0; --y) {
            for(unsigned x = 0; x < SCREEN_WIDTH; ++x) {
                char map_char = map[x][y];
                if(turret_location.y == y) {
                    bool in_range = false;
                    if(turret_dir < 0 && x < turret_location.x) {
                        in_range = true;
                        for(unsigned tmp = x; tmp < turret_location.x; ++tmp) {
                            if(map[tmp][y] != ' '){
                                in_range = false;
                                break;
                            }
                        }
                    } else if(turret_dir > 0 && x > turret_location.x) {
                        in_range = true;
                        for(unsigned tmp = turret_location.x+1; tmp <= x; ++tmp) {
                            if(map[tmp][y] != ' '){
                                in_range = false;
                                break;
                            }
                        }
                    }
                    if(in_range) {
                        font_color(0xc);
                        printf("-");
                        font_color(0xf);
                        if(player_location.x == x && player_location.y == y) {
                            return 2; //lose
                        }
                        continue;
                    }
                }

                if(player_location.x == x && player_location.y == y) {
                    printf("%c", player_char);
                } else if(portal_blue_exist && portal_blue_location.x == x && portal_blue_location.y == y) {
                    if(portal_blue_in_place) {
                        font_background_color(0x9);
                        printf(" ");
                        font_background_color(0x0);
                    } else {
                        font_color(0x9);
                        printf("-");
                        font_color(0xf);
                    }
                } else if(portal_orange_exist && portal_orange_location.x == x && portal_orange_location.y == y) {
                    if(portal_orange_in_place) {
                        font_background_color(0x6);
                        printf(" ");
                        font_background_color(0x0);
                    } else {
                        font_color(0x6);
                        printf("-");
                        font_color(0xf);
                    }
                } else if(map_char == 'X') {
                    font_background_color(0x7);
                    font_color(0x8);
                    printf("#");
                    font_color(0xf);
                    font_background_color(0x0);
                } else if(map_char == 'Y') {
                    font_background_color(0x7);
                    printf(" ");
                    font_background_color(0x0);
                }else if(map_char == 'V') {
                    font_background_color(0xa);
                    printf(" ");
                    font_background_color(0x0);
                } else{
                    printf("%c", map_char);
                }
            }
            printf("\n");
        }

        if(tick % TURRET_TICK == 0) {
            turret_dir = -turret_dir;
        }
        if(tick % GRAVITY_TICK == 0) {
            if(player_location.y > 0) {
                if(map[player_location.x][player_location.y-1] == ' ') {
                    --player_location.y;
                    if(map[player_location.x][player_location.y-1] != ' ') {
                        player_jump = false;
                    }
                } else {
                    player_jump = false;
                }
            }
        }
        if(portal_blue_exist && !portal_blue_in_place) {
            unsigned next_x = portal_blue_location.x + portal_blue_dir;
            if(next_x + 1 == 0 || next_x > SCREEN_WIDTH || map[next_x][portal_blue_location.y] == 'X') {
                portal_blue_exist = false;
            } else {
                portal_blue_location.x = next_x;

                if(map[next_x][portal_blue_location.y] == 'Y') {
                    portal_blue_in_place = true;
                    if(portal_blue_location.x == portal_orange_location.x && portal_blue_location.y == portal_orange_location.y ) {
                        portal_orange_in_place = false;
                        portal_orange_exist = false;
                    }
                }
            }
        }
        if(portal_orange_exist && !portal_orange_in_place) {
            unsigned next_x = portal_orange_location.x + portal_orange_dir;
            if(next_x + 1 == 0 || next_x > SCREEN_WIDTH || map[next_x][portal_orange_location.y] == 'X') {
                portal_orange_exist = false;
            } else {
                portal_orange_location.x = next_x;
                if(map[next_x][portal_orange_location.y] == 'Y') {
                    portal_orange_in_place = true;
                    if(portal_blue_location.x == portal_orange_location.x && portal_blue_location.y == portal_orange_location.y ) {
                        portal_blue_in_place = false;
                        portal_blue_exist = false;
                    }
                }
            }
        }

        wait_clock(current_clock() + 10);
    }
}

int play_portal() {
    cons_echo(0);

    // Player
    player_jump = false;
    player_char = '>';
    // Portals

    portal_blue_exist = false;
    portal_blue_in_place = false;
    portal_blue_dir = 0;

    portal_orange_exist = false;
    portal_orange_in_place = false;
    portal_orange_dir = 0;

    //Turret
    turret_dir = -1;

    init_map();
    keyboard_wait_queue = pcreate(1);
    int keyboard_pid = start(keyboard_detection, 512, 67, "portal_keyboard_detection", 0);
    int display_pid = start(display, 512, 66, "portal_display", 0);
    int retval = 0;
    while(1) {
        if(waitpid_nohang(keyboard_pid, &retval) == keyboard_pid) {
            kill(display_pid);
            if(retval == 1) { //WIN
                clear();
                font_color(0xa);
                printf("\n");
                printf("XX     XX XX XXX    XX\n");
                printf("XX     XX XX XXXX   XX\n");
                printf("XX  X  XX XX XX XX  XX\n");
                printf("XX XXX XX XX XX  XX XX\n");
                printf(" XXX XXX  XX XX   XXXX\n");
                printf("\n");
                font_color(0xf);
                printf("In %ld seconds\n", (current_clock() - start_clock) / 100);
                printf("\n");
                printf("\n");
                printf("Press r to restart. Press q to quit\n");
                char c[1];
                while(1) {
                    cons_read(c, 1);
                    if (c[0] == 'r')
                        return 1;
                    else if(c[0] == 'q')
                        return 0;
                }
            }
            break;
        } else if(waitpid_nohang(display_pid, &retval) == display_pid) {
            kill(keyboard_pid);
            if(retval == 2) { //LOSE
                clear();
                font_color(0xc);
                printf("\n");
                printf("XXXXXX  XXXXXXX XXXXXX  XXXXXX  XX    XX\n");
                printf("XX   XX XX      XX   XX XX   XX XX    XX\n");
                printf("XXXXXX  XXXXX   XXXXXX  XX   XX XX    XX\n");
                printf("XX      XX      XX   XX XX   XX XX    XX\n");
                printf("XX      XXXXXXX XX   XX XXXXXX   XXXXXX \n");
                printf("\n");
                font_color(0xf);
                printf("In %ld seconds\n", (current_clock() - start_clock) / 100);
                printf("\n");
                printf("\n");
                printf("Press r to restart. Press q to quit\n");
                char c[1];
                while(1) {
                    cons_read(c, 1);
                    if (c[0] == 'r')
                        return 1;
                    else if(c[0] == 'q')
                        return 0;
                }
            }
            break;
        }
    }
    return 0;
}



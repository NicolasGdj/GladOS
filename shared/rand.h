//
// Created by guerroun on 22/06/2022.
//

#ifndef PSYS_BASE_RAND_H
#define PSYS_BASE_RAND_H

/*******************************************************************************
 * Pseudo random number generator
 ******************************************************************************/
unsigned long long mul64(unsigned long long x, unsigned long long y);
typedef unsigned long long uint_fast64_t;
typedef unsigned long uint_fast32_t;


// Assume that 1 <= _bits <= 32
uint_fast32_t randBits(int _bits);

void setSeed(uint_fast64_t _s);

unsigned long rand();
#endif //PSYS_BASE_RAND_H

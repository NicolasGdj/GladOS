//
// Created by guerroun on 22/06/2022.
//

#include "rand.h"

/*******************************************************************************
 * Pseudo random number generator
 ******************************************************************************/
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

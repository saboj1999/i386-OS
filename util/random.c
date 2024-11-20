#include "../include/random.h"

// move function to own file random.c
unsigned int RandomInt(unsigned int max) {
    // +++++++++++ChatGPT-Function+++++++++++ //
    unsigned int hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return lo % max;
    // +++++++++++ChatGPT-Function+++++++++++ //
}
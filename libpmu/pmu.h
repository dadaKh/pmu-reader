#ifndef _PMU_H_
#define _PMU_H_

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/syscall.h>
#define AMD 0
#define INTEL 1
#define NUMBER_OF_PROGRAMMABLE_PMCs 1

#define READ_PMC(pmuvalue, lo, hi) \
    __asm__ volatile(                           \
        "mfence\n\t"                            \
        "rdpmc\n\t"                             \
        "mfence\n\t"                            \
        : "=a"(lo), "=d"(hi)                    \
        : "c"(0x0));                       \
    pmuvalue = ((uint64_t)hi << 32 | lo);

typedef struct
{
  uint64_t hexcode;
  const char *name;
} counter;

// Bind PMU events to the MSR(IA32_PERFEVTSELi) register of the core
/*
* core : logical core number
* event_code: 
*/
void setup_pmc(int core, uint64_t event_code);

// Clean the PMC value
void zero_pmc();

// Start PMC
void start_pmc();

// Stop PMC
void stop_pmc();

// rdpmc
// faster than get_stats
uint64_t readPMC();

// get PMC stats
void get_stats(int64_t *ctrs);


#endif
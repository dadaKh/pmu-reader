#ifndef _PMU_H_
#define _PMU_H_

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/syscall.h>

#if defined(__x86_64__)
#define INTEL 1
#define AMD 0

#define NUMBER_OF_PROGRAMMABLE_PMCs 4

// macro definition for rdpmc
#define READ_x86_PMC(pmuvalue, lo, hi, c) \
    __asm__ volatile(                           \
        "mfence\n\t"                            \
        "rdpmc\n\t"                             \
        "mfence\n\t"                            \
        : "=a"(lo), "=d"(hi)                    \
        : "c"(c));                       \
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
* n: number of events
*/
void pmu_x86_setup_pmc(int core, uint64_t *event_code, int n);

// Clean the PMC value
/*
* n: The index of target counter
*/
void pmu_x86_zero_pmc(int c);

// Start PMC
void pmu_x86_start_pmc();

// Stop PMC
void pmu_x86_stop_pmc();

// rdpmc
// faster than get_stats

uint64_t pmu_x86_readPMC(int c);

// get PMC stats
/*
* ctrs: The array of PMC values
* n: The number of counters
*/
void pmu_x86_get_stats(int64_t *ctrs, int c);

#elif defined(__aarch64__)
void pmu_arm_init(uint64_t event_code, int counter);

void pmu_arm_exit();

uint64_t pmu_arm_read_pmc(int counter);

#endif
#endif
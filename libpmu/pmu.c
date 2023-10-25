#include "pmu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(__x86_64__)
int msr_fd;
// Programmable PMCs 
counter programmables[4];
size_t n_programmables = NUMBER_OF_PROGRAMMABLE_PMCs;

void write_to_IA32_PERFEVTSELi(int i, uint64_t val)
{
	size_t ret;
#ifdef INTEL
	ret = pwrite(msr_fd, &val, sizeof(val), 0x186 + i);
#endif
#ifdef AMD
	ret = pwrite(msr_fd, &val, sizeof(val), 0xC0010000 + i);
#endif
	if (!ret)
		printf("Pwrite error!\n");
}

void write_to_IA32_PMCi(int i, uint64_t val)
{
	size_t ret;
#if INTEL
	ret = pwrite(msr_fd, &val, sizeof(val), 0xC1 + i);
#endif
#if AMD
	ret = pwrite(msr_fd, &val, sizeof(val), 0xC0010004 + i);
#endif
	if (!ret)
		printf("Pwrite error!\n");
}

void write_to_IA32_PERF_GLOBAL_CTRL(uint64_t val)
{
	size_t ret;
	ret = pwrite(msr_fd, &val, sizeof(val), 0x38F);
	if (!ret)
		printf("Pwrite error!\n");
}


// setup pmu_hexcode to msr
void pmu_x86_setup_pmc(int core, uint64_t event_code) {
	char *msr_path;
	msr_path = (char *)malloc(sizeof(char)*16);
	sprintf(msr_path,"/dev/cpu/%d/msr",core);     
	msr_fd = open(msr_path, O_RDWR);

	/* DISABLE ALL COUNTERS */                                         
	write_to_IA32_PERF_GLOBAL_CTRL(0ull);
  
	/* PROGRAM ALL PMCs */                                                  
	unsigned i;
	for (i = 0; i < n_programmables; i++)  {  
		programmables[i].hexcode = event_code;
		write_to_IA32_PERFEVTSELi(i, programmables[i].hexcode); 
	}
	lseek(msr_fd, 0x38F, SEEK_SET);
}

// read pmu counter
uint64_t read_IA32_PMCi(int i){
	uint64_t toret = -1;
	size_t ret;
#if INTEL
	ret = pread(msr_fd, &toret, sizeof(toret), 0xC1 + i);
#endif
#if AMD
	ret = pread(msr_fd, &toret, sizeof(toret), 0xC0010004 + i);
#endif
	if (!ret)
		printf("Pread error!\n");

	return toret;
}


void pmu_x86_get_stats(int64_t *ctrs){
	unsigned i;
	for(i = 0; i < n_programmables; i++)
		ctrs[i] = read_IA32_PMCi(i);
}


void pmu_x86_zero_pmc();

inline void pmu_x86_zero_pmc(){
	unsigned i;
	for(i = 0; i < n_programmables; i++)   
		write_to_IA32_PMCi(i, 0ull);   
}

uint64_t pmu_x86_readPMC(){
	uint64_t lo,hi;
	uint64_t pmc_value;
	asm volatile ("rdpmc" 
		: "=a" (lo), "=d" (hi) 
		: "c"(0x0)
		: );
	pmc_value = ((uint64_t)hi << 32 | lo);
	return pmc_value;
}

void pmu_x86_start_pmc();
void pmu_x86_stop_pmc();

inline void pmu_x86_start_pmc() {
	uint64_t val = 15ull | (7ull << 32);
	asm("mov %[write],     %%eax;"
		"mov %[fd],        %%edi;"
		"mov %[val],       %%rsi;"
		"mov $8,           %%edx;"
		"syscall;"
		:
		: [write] "i" (SYS_write),
          [val]   "r" (&val),
          [fd]    "m" (msr_fd)
		: "eax", "edi", "rsi", "edx");
}


inline void pmu_x86_stop_pmc() {
	uint64_t val = 0;
	asm("mov %[write],     %%eax;"
		"mov %[fd],        %%edi;"
		"mov %[val],       %%rsi;"
		"mov $8,           %%edx;"
		"syscall;"
		:
		: [write] "i" (SYS_write),
		  [val]   "r" (&val),
		  [fd]    "m" (msr_fd)
		: "eax", "edi", "rsi", "edx");
}
#elif defined(__aarch64__)
void pmu_arm_init(uint64_t event_code, int counter){
	switch (counter)
    {
    case 0:
        asm volatile("msr PMEVTYPER0_EL0, %[event_code]\n\t"::[event_code]"r"(event_code));
        break;
    case 1:
        asm volatile("msr PMEVTYPER1_EL0, %[event_code]\n\t"::[event_code]"r"(event_code));
        break;
    case 2:
        asm volatile("msr PMEVTYPER2_EL0, %[event_code]\n\t"::[event_code]"r"(event_code));
        break;
    case 3:
        asm volatile("msr PMEVTYPER3_EL0, %[event_code]\n\t"::[event_code]"r"(event_code));
        break;
    case 4:
        asm volatile("msr PMEVTYPER4_EL0, %[event_code]\n\t"::[event_code]"r"(event_code));
        break;
    case 5:
        asm volatile("msr PMEVTYPER5_EL0, %[event_code]\n\t"::[event_code]"r"(event_code));
        break;
    default:
        break;
    }
    uint32_t r = 0;
    asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
    asm volatile("msr pmcntenset_el0, %0" : : "r" (r|(1<<counter)));
}

void pmu_arm_exit(int counter){
	uint32_t r = 0;
	asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
	asm volatile("msr pmcntenset_el0, %0" : : "r" (r&0x80000000));
}

uint64_t pmu_arm_read_pmc(int counter){
	uint64_t pmu_value;
	switch (counter)
    {
    case 0:
        asm volatile(
			"isb \n"
			"dsb sy \n"
			"mrs %0, pmevcntr0_el0 \n"
			"isb \n"
			"dsb sy \n"
			: "=r" (pmu_value)
			:
			:
    	);
        break;
    case 1:
        asm volatile(
			"isb \n"
			"dsb sy \n"
			"mrs %0, pmevcntr1_el0 \n"
			"isb \n"
			"dsb sy \n"
			: "=r" (pmu_value)
			:
			:
    	);
        break;
    case 2:
        asm volatile(
			"isb \n"
			"dsb sy \n"
			"mrs %0, pmevcntr2_el0 \n"
			"isb \n"
			"dsb sy \n"
			: "=r" (pmu_value)
			:
			:
    	);
        break;
    case 3:
        asm volatile(
			"isb \n"
			"dsb sy \n"
			"mrs %0, pmevcntr3_el0 \n"
			"isb \n"
			"dsb sy \n"
			: "=r" (pmu_value)
			:
			:
    	);
        break;
    case 4:
        asm volatile(
			"isb \n"
			"dsb sy \n"
			"mrs %0, pmevcntr4_el0 \n"
			"isb \n"
			"dsb sy \n"
			: "=r" (pmu_value)
			:
			:
    	);
        break;
    case 5:
        asm volatile(
			"isb \n"
			"dsb sy \n"
			"mrs %0, pmevcntr5_el0 \n"
			"isb \n"
			"dsb sy \n"
			: "=r" (pmu_value)
			:
			:
    	);
        break;
    default:
		return 0;
        break;
    }
    
    return pmu_value;
}
#endif
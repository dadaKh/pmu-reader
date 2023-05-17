#include "pmu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>

int msr_fd;
// Programmable PMCs 
counter programmables[4];
size_t n_programmables = NUMBER_OF_PROGRAMMABLE_PMCs;

void write_to_IA32_PERFEVTSELi(int i, uint64_t val)
{
	size_t ret;
#if INTEL
	ret = pwrite(msr_fd, &val, sizeof(val), 0x186 + i);
#endif
#if AMD
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
void setup_pmc(int core, uint64_t event_code) {
	char msr_path[] = "/dev/cpu/*/msr";                                     
	msr_path[9] = '0' + core;
	//printf("MSR_PATH:%s\n",msr_path);
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


void get_stats(int64_t *ctrs){
	unsigned i;
	for(i = 0; i < n_programmables; i++)
		ctrs[i] = read_IA32_PMCi(i);
}


void zero_pmc();

inline void zero_pmc(){
	unsigned i;
	for(i = 0; i < n_programmables; i++)   
		write_to_IA32_PMCi(i, 0ull);   
}

uint64_t readPMC(){
	uint64_t lo,hi;
	uint64_t pmc_value;
	asm volatile ("rdpmc" 
		: "=a" (lo), "=d" (hi) 
		: "c"(0x0)
		: );
	pmc_value = ((uint64_t)hi << 32 | lo);
	return pmc_value;
}

void start_pmc();
void stop_pmc();

inline void start_pmc() {
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


inline void stop_pmc() {
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
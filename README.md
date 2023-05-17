### Build
```
make
```
At this point, the Linux file pointer can be used to set the MSR register, but due to the use of system calls, the speed is slower

`pmu.h`
```c
// macro definition for rdpmc
#define READ_PMC(pmuvalue, lo, hi) \
    __asm__ volatile(                           \
        "mfence\n\t"                            \
        "rdpmc\n\t"                             \
        : "=a"(lo), "=d"(hi)                    \
        : "c"(0x0));                       \
    pmuvalue = ((uint64_t)hi << 32 | lo);

// Bind PMU events to the MSR(IA32_PERFEVTSELi) register of the core
/*
* core : logical core number
* event_code: Event Code 
*/
void setup_pmc(int core, uint64_t event_code);

// set zero to pmc
void zero_pmc();

// Start Counter
void start_pmc();

// Stop Counter
void stop_pmc();

// rdpmc
// faster than get_stats
uint64_t readPMC();

// get PMC stats
void get_stats(int64_t *ctrs);
```


### ENABLE RDPMC
```shell
make enable
```
Test:
```shell
sudo taskset -c 1 ./demo 1 0xff89
```
+ arg[1] is the logical core number, same as the `taskset -c`
+ arg[2] is the event code you want read


### EXIT
```shell
make unable

make clean
```

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "./libpmu/pmu.h"

int main(int argc,char *argv[]){
#if defined(__x86_64__)
    int core;
    uint64_t event_hexcode;


    if(argc >= 2){
        core = strtoul(argv[1],NULL,0);
    }else{
        printf("Target Core Error!\n");
        exit(0);
    }
    if(argc >= 3){
        event_hexcode = strtoull(argv[2], NULL, 0);
    }else{
        printf("Target Event Code Error!\n");
        exit(0);
    }
     
    // USR-bit and EN-bit enable
    uint64_t hexcode = 0x410000|event_hexcode;
	printf("Hexcode: 0x%lx\n", hexcode);
    int64_t pmuvalue;
    uint32_t lo,hi;
    uint32_t pmu_value_s, pmu_value_e;

	pmu_x86_setup_pmc(core,hexcode);
    pmu_x86_zero_pmc();
    pmu_x86_start_pmc();
    // readPMC
    // pmu_value_s = readPMC();
    READ_x86_PMC(pmu_value_s,lo,hi);

    // make some differences
    int test = 0;
    test++;
    printf("do something!\n");

    READ_x86_PMC(pmu_value_e,lo,hi);
    pmuvalue = pmu_value_e - pmu_value_s;
    
    printf("PMU Value: %ld\n",pmuvalue);
    return 0;
#elif defined(__aarch64__)

    // default event code
    uint64_t event_code0 = 0x01, event_code1 = 0x14;
    pmu_arm_init(event_code0, 0);
    pmu_arm_init(event_code1, 1);

        
    // get event code from argv
    int event_num = argc-1;
    if (event_num > 6)
    {
        printf("Too many events!\n");
        exit(0);
    }
    uint64_t event_code[event_num];
    if(argc >= 2){
        for(int i = 0; i < event_num; i++){
            event_code[i] = strtoull(argv[i+1], NULL, 0);
            pmu_arm_init(event_code[i], i);
            printf("Event Code 0x%lx\t was Initialed\n", event_code[i]);
        }
    }

    printf("**********  start  **********\n\n");
    uint64_t c1, c2, c3, c4;
    c1 = pmu_arm_read_pmc(0);
    c3 = pmu_arm_read_pmc(1);
    printf("do something\n\n");
    c2 = pmu_arm_read_pmc(0);
    c4 = pmu_arm_read_pmc(1);
   
    printf("**********   end   **********\n\n");
    printf("start: %ld, end: %ld\n", c1, c2);
    printf("start: %ld, end: %ld\n", c3, c4);
#endif
}




#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "./libpmu/pmu.h"

int main(int argc,char *argv[]){
#if defined(__x86_64__)
    int core;
    if(argc >= 2){
        core = strtoul(argv[1],NULL,0);
    }else{
        printf("Target Core Error!\n");
        exit(0);
    }

    int n = 0;

    if(argc >= 3){
        n =  argc - 2;
    }else{
        printf("Please input your events!\n");
        exit(0);
    }

    printf(" %d\n",n);
    if (n > NUMBER_OF_PROGRAMMABLE_PMCs) {
        printf("Too many event!\n");
        exit(0);
    }else if(n == 0){
        printf("No event!\n");
        exit(0);
    }
    uint64_t event_hexcode[n];

    for(int i = 0; i < n; i++){
        event_hexcode[i] = strtoull(argv[i+2], NULL, 0);
        printf("Event Code 0x%lx\t was Initialed\n", event_hexcode[i]);
        // USR-bit and EN-bit enable
        event_hexcode[i] = event_hexcode[i] | 0x410000;
    }
    uint64_t pmuvalue[n];
    uint32_t lo,hi;
    uint32_t pmu_value_s[n], pmu_value_e[n];

	pmu_x86_setup_pmc(core, event_hexcode, n);
    for (int i = 0 ; i < n; i++){
        pmu_x86_zero_pmc(i);
    }

    pmu_x86_start_pmc();
    // readPMC
    for (int i = 0 ; i < n; i++){
        pmu_value_s[i] = pmu_x86_readPMC(i);
        // READ_x86_PMC(pmu_value_s[i],lo,hi,i);
    }

    // make some differences
    int test = 0;
    test++;
    printf("do something!\n");


    // readPMC
    for (int i = 0 ; i < n; i++){
        pmu_value_e[i] = pmu_x86_readPMC(i);
        // READ_x86_PMC(pmu_value_e[i],lo,hi,i);
    }
    pmu_x86_stop_pmc();
    for(int i = 0; i < n; i++){
        pmuvalue[i] = pmu_value_e[i] - pmu_value_s[i];
        printf("Event: 0x%lx        PMU Value: %ld\n",event_hexcode[i],pmuvalue[i]);

    }

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




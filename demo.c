#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "./libpmu/pmu.h"

int main(int argc,char *argv[]){
    int core = 1;
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

	setup_pmc(core,hexcode);
    zero_pmc();
    start_pmc();
    // readPMC
    // pmu_value_s = readPMC();
    READ_PMC(pmu_value_s,lo,hi);

    // make some differences
    int test = 0;
    test++;
    printf("Test++ Maybe Trigger some PMUs \n");

    READ_PMC(pmu_value_e,lo,hi);
    pmuvalue = pmu_value_e - pmu_value_s;
    
    printf("PMU Value: %ld\n",pmuvalue);
    return 0;
}




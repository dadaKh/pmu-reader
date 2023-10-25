pmu = $(wildcard ./libpmu/pmu.c)
CC = gcc $(pmu)
SOURCES := $(wildcard *.c)
BINARIES := $(SOURCES:%.c=%)

all: $(BINARIES)

enable_x86:
	cd x86-pmu-enable && make && sudo insmod pmu_enable.ko

enable_arm:
	cd arm-pmu-enable && make && sudo insmod pmu_enable.ko

clean:
	rm -f $(BINARIES)
	cd arm-pmu-enable && make clean
	cd x86-pmu-enable && make clean
	sudo rmmod pmu_enable

	
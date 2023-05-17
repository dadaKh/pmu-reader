pmu = $(wildcard ./libpmu/pmu.c)
CC = gcc -O0  $(pmu)
SOURCES := $(wildcard *.c)
BINARIES := $(SOURCES:%.c=%)

all:$(BINARIES)
	cd pmc-enable-cr4 && make

enable:
	cd pmc-enable-cr4 && sudo insmod enable_pmc.ko

unable:
	cd pmc-enable-cr4 && sudo rmmod enable_pmc

clean:
	rm -f $(BINARIES)
	cd pmc-enable-cr4 && make clean
	
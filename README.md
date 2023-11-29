## PMU reader for linux

### x86
#### How to use it？
Load MSR mod
```shell
sudo modprobe msr
```
Make
```shell
make enable_x86
```
Test
```
sudo taskset -c [core] ./demo [core] [event1] [event2] [event3] ... 
```

Example:
```shell
> sudo taskset -c 1 ./demo 1 0x10e6 0xc189 0xff89
Event Code 0x10e6 was Initialed
Event Code 0xc189 was Initialed
Event Code 0xff89 was Initialed


do something!

Event: 0x4110e6        PMU Value: 0
Event: 0x41c189        PMU Value: 19
Event: 0x41ff89        PMU Value: 26
```

### ARM
```shell
make enable_arm
```
Test
```
make
./demo [event1] [event2] [event3] [event4] [event5] [event6]
```
Example:
```shell
**********  start  **********

do something

**********   end   **********

start: 631, end: 642
start: 10469, end: 10475
```

####

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
sudo taskset -c [core] ./demo [core] [event] 
```

Example:
```shell
> sudo taskset -c 1 ./demo 1 0x10a3
Hexcode: 0x4110a3
do something!
PMU Value: 7216
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

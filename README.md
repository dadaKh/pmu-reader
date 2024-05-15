## PMU reader for linux

### x86
#### How to use it？
Load MSR mod
```shell
sudo modprobe msr
```
Make
```shell
make && make enable_x86
```
Test
```
sudo taskset -c [core] ./demo [core] [event1] [event2] [event3] ...
```

Example:
```shell
> sudo taskset -c 1 ./demo 1 0xc189 0x2079
Event Code 0xc189 was Initialed
Event Code 0x2079 was Initialed


do something!

Event: 0x41c189        PMU Value: 20
Event: 0x412079        PMU Value: 311
```

### ARM
Make
```shell
make && make enable_arm
```
Test
```
./demo [event1] [event2] [event3] [event4]
``` 

Example:
```shell
> taskset -c 1 ./demo 0x12 0x12 0x13 0x14 0x15
Event Code 0x12  was Initialed
Event Code 0x12  was Initialed
Event Code 0x13  was Initialed
Event Code 0x14  was Initialed
Event Code 0x15  was Initialed
**********  start  **********

do something

**********   end   **********

Event Code 0x12 :246
Event Code 0x12 :250
Event Code 0x13 :303
Event Code 0x14 :646
Event Code 0x15 :2
```

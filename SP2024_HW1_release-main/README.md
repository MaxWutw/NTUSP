## SP2024_HW1_release
#### public testcases judge
###### Usage
```
python3 checker.py
```
###### Argument

- `-t TASK [TASK ...]`, `--task TASK [TASK ...]`, Specify which tasks you want to run. If you didn't set this argument, `checker.py` will run all tasks by default.
    - Valid TASK are ["1-1", "1-2", "1-3", "1-4", "2-1", "2-2", "3", "4"].
    - for example `python3 checker.py --task 1-1 1-2` will run both `testcase1_1` and `testcase1_2`


### Report
1. (1) Busy waiting refers to a situation in concurrent programming where a thread (or process) continuously checks a condition
    in a loop without relinquishing control of the CPU.
    During this time, the thread does not perform any useful work and consumes CPU resources while waiting for the condition to change.
    This can lead to inefficient use of CPU time, as the CPU remains occupied by the busy-waiting thread instead of processing other tasks.
   (2) In this assignment I use select with 5 seconds timeout to avoid busy waiting.
   (3) Yes, it is possible to have a busy waiting with select() and poll().
     Assume no timeout is specified, or it's set to 0. 
     If you continuously call select() in a tight loop without any conditions that cause the loop to pause or wait, 
     this would create busy waiting.

2. (1) Starvation is a condition in concurrent computing where a thread (or process) is perpetually denied the 
     resources it needs to proceed with its execution. 
     This often occurs when higher-priority threads or processes continuously consume available resources, 
     preventing lower-priority ones from executing. 
     Starvation can lead to performance degradation and unresponsiveness in a system, as certain tasks may never get a chance to run.
   (2) Yes, it is possible. If multiple clients are connecting to your server simultaneously, 
     and if your server's handling logic prioritizes certain requests over others (e.g., based on arrival order or specific criteria), 
     lower-priority requests may be starved.


### Conclusion
In this homework, I have almost completed all the requirements.
However, I am just an audience student in this class, so I didn’t spend too much time handling error input and the details.
This homework can manage multiplexing I/O with select() and can close the client connection if the client is connected for over 5 seconds.
Additionally, this homework can support multiple clients connecting to the server and can detect whether the seat has been booked or locked.

Thread Manager
=========
Overview
--------
The thread manager is in charge of maintaining the different threads. Once it is called to create a thread it allocates some memory on the heap for the thread structure. It then initialises the thread structure and the kernel stack for that thread. Finnaly when the thread is started it calls the scheduler with the lowest load to add this thread to one of its queue. The code for the thread manager resides in `thread.c` and `thread.h`.
Problems
--------
When the stack of a thread is overrun and creates a paging fault, the entire system will crash at a timer interrupt. In order to fix this we could use a tss with a kernel stack for interrupts. However we cant just use 1 kernel stack, as our GPR/system state after a context switch will be stored on this stack. So a solution to this could be to give each thread it's own small kernelstack, and change tss.rsp0 before a task switch. However this means that every kernel stack should be mapped in every address space. Or we change the page directory and the stack inside the scheduler C function.

We could also use some virtual memory magic: Each kernel stack has its own physical frame but is always mapped to the same virtual address. When swapping task all we have to do is change the mappings of that virtual address.

```C
vmm_map_frame(KERNEL_STACK, current_thread->kstack, 0x3);
```
```C
vmm_flush_page(KERNEL_STACK);
```

Note how in this solution e do not have to change the rsp, nor the tss to do a context switch. Here however we run in the following problem: Imagine thread1 one core0 and thread2 on core1 sharing the same address space... But before we can implement a particuler system we must have some kind of virtual memory space allocater.



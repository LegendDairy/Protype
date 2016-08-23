Scheduler
=========
Algorithm
---------
The scheduling algorithm is a multilevel queue and each queue uses Round Robin to select the next thread. There are multiple 'ready' queues with different priorities. Queues with higher priority are used more often than lower ones by RR scheduler to select a new thread. For example imagine 3 threads A,B and C where A has the highest priority,  B has a normal priority and C the lowest. Then these threads will be executed in the following order:
      `A-B-A-B-A-C`
So every odd quantum a highest priority thread will be executed and every second and 4th quantum a medium priority and finally every 6th quantum a lowest priority thread.

* current_thread:				`A-C-A-B-A-B-A-C-A-B-A-B-A-C-A-B-A-B`
* current_tick	:				`1-2-3-4-5-6...`

This is what I have implemented right now, however it scales horribly on smp because there are only 3 locks (so a high chance of lock contention on big systems), and there is no 'pinning' of threads to a specific cpu. In order to optimize this for SMP systems, I was thinking about using the 1 scheduler per logical cpu method, with each scheduler having its own set of queues and locks. There would then be one thread in charge of load balancing the threads across the different schedulers.The schedulers can only have lock contention when a thread is being added to or removed from one of its queues that he is trying to access, because of load balancing or because a thread was started, blocked... When a thread is blocked for some reason, it would be put on the blocked list of the scheduler it was using. When it's time to start the thread again, it will be awoken on the same cpu. The load balancing thread might later try to reappoint it to an other scheduler.

I could take it a step further and remove locks completely. When a thread is to be added or removed from a scheduler, an IPI is send to the logical cpu that holds that thread. It then disables interrupts to make sure its queues wont be accessed and then adds or removes the threads. However IPIs have quiet a big overhead (e.g. pipeline flush), but locking the bus every time the scheduler is called isn't great for performance either especially on large systems. Also note that if a thread that is being executed on a cpu x, tries to stop/start a thread that is also on cpu x, it wont have to send an IPI, just temporarily disable interrupts. So a thread blocking itself won't cause a performance hit.

Context switch
--------------
Context switches are preformed by changing the stack pointer (rsp) to the value of the new thread. When creating a new thread, it must have its own proper stack that contains all the values as if it was just interrupted.

When a timer interrupt fires, all the GPRs are pushed on the current stack. The function `tm_schedule` get's called by the interupt routine of the APIC timer, it requires the rsp of the current task (passed through rdi in the System V ABI). This function finds a new thread to be executed and returns the rsp of that thread (passed through rax in the System V ABI). The schedule function also sets the kernelstack field in the current tss to this new rsp, so that when the next interrupt fires this stack is used. The timer handler than continues with this new stack and pops all the GPRs, cr3 and the data selectors of this stack. The iretq instruction then pops rsp, ss, and cs.

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

Note how in this solution e do not have to change the rsp, nor the tss to do a context switch. Here however we run in the following problem: Imagine thread1 one core0 and thread2 on core1 sharing the same address space...


Pseudo Code
-----------

```asm
timer_handler_asm:
	(Push all regs)
	(Push data selectors)
	mov eax, cr3
	push eax
	mov rdi, rsp ; system V ABI: rdi=first arg
	call tm_schedule
	mov rsp, rax ; system V ABI: rax=return value
	pop eax
	mov cr3, eax
	(pop data selectors)
	(pop all regs)
	send_eoi
	iretq
```
```C
uint64_t tm_schedule(uint64_t rsp)
{
current_tick++;
current_thread->rsp = rsp;
spinlock_lock();
add_thread_tot_end_of_que();
sched_find_next_thread();
spinlock_unlock();
tss_set_esp0(current_thread->rsp);
return current_thread->rsp;
}
```
```C
void sched_find_next_thread()
{
If(current_tick%2 && ready_que_high)
{
      current_thread = ready_que_highest;
      ready_que_highest = ready_que_highest->next;
}
else if(current_tick%6 && ready_que_lowest)
{
...
}
else if(ready_que_medium)
{
...
}
}
```

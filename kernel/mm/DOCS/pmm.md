Memory managment
================
Overview
--------
When the IPL loads the kernel the cpu is in Long Mode and thus paging is already enabled.
To initialize the pmm the kernel needs some physical memory to setup a linked list and stack of free physical frames
However because paging is enabled to write to this memory it needs to be mapped
but in order to map pages the vmm needs some physical pages for pagetables, page directories but theres no pmm yet...
This creates a chicken-and-egg type of problem.

The solution is to use a pre-pmm that uses the bios memorymap as some sorth of "heap" for the physical memory
The pre-pmm first deletes the entries in the map from 0x00 to the end of the kernel and page-alligns all the entries
It then determines how much memory is needed for the pmm and allocates a region in the map big enough and erases those entries.
The pre-pmm then maps those pages by using a pre-vmm-mapper that can ask the pre-pmm for pages to be used as pagetables etc.

Now that there is some dynamically mapped memory for the pmm, the initialisation process can start.
The pmm parses the modified memory-map and marks those pages as unallocated.
Now that there is a physical memory manager that can allocated and free physical pages dynamically
The vmm can now ask the pmm for a physical page dynamically and map and unmap physical pages to a given virtual address

Furture improvements:
Stability could be improved by giving the pmm it's own pagedirectory, so that no mall-written kernelcode could harm the stack,
however this seems like overcomplication and could lead to a buggy pmm if the directories aren't switched properly...
We shall assume that the rest of the kernel is written w/o any major bugs and wont touch the pmm's data.
This should be possible seeing how this is a microkernel and most things will operate in usermode w/o access to this data.

Also note that this is amd64, thus our addresses need to be canonical (caused some annoying GP with recursivemapped PLM4T)
More on this latter
I also had to add "-mcmodel=large" to the linker and gcc flags, but this may not be very efficient, more research is needed...


Pseudo Code
-----------
Here's some "Pseudo" code i wrote before writing the real code:

```C
plm4t->pdpt->pdt-pt
#define PLM4T_INDEX(a) ((u32int)a/(512*1024*1024*1024))
#define PDPT_INDEX(a) ((u32int)a%512*1024*1024*1024)/(1024*1024*1024)
#define PDT_INDEX(a) (((u32int)a%512*1024*1024*1024)%(1024*1024*1024))/(2*1024*1024)
#define PT_INDEX(a) ((((u32int)a%512*1024*1024*1024)%(1024*1024*1024))%(2*1024*1024))
```

```C
void pre_pmm_setup(ipl_info_t *info)
{
  parse_mmap();
  allocate_Kmem();
  uint64_t sz = calculate_pmm_sz();
  get_free_region(sz);
  allocate_region();
  pre_vmm_map_region();
}
```
```C
void pre_vmm_map(u32int paddress, u32int vaddress)
{
  if(!PLM4T[PLM4T_INDEX(vaddress)])
  {
  uint64_t frame = pre_pmm_allocate_frame();
  errase_page(frame);
  PLM4T[x] = frame;
  //PLM4T is mapped in dir0 so PLM4T entries are mapped!
  }
  if(!PDPT[PDPT_INDEX(vaddress)])
  {
  uint64_t frame = pre_pmm_allocate_frame();
  errase_page(frame);
  PDPT[x] = frame;
  map_entry to dir0[510];
  map_dirx_to_dirx[511];
  }
  if(!PDT[PDT_INDEX(vaddress)])
  {
  uint64_t frame = pre_pmm_allocate_frame();
  errase_page(frame);
  PDT[x] = frame;
  }
  PDT[PT_INDEX(vaddress)] = paddress;
}
```

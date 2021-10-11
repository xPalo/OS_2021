#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 start;
  int npages;
  uint64 bitmask_user_address;
  uint64 bitmask_kernel = 0;

  if(argaddr(0, &start) < 0){
    return -1;
  }

  if(argint(1, &npages) < 0){
    return -1;
  }

  if(argaddr(2, &bitmask_user_address) < 0){
    return -1;
  }

  // validity of arguments
  if(npages > 12500){
    return -1;
  }

  //add pta_a and number of system call

  for(int i = 0; i < npages; i++){
    pte_t *pte = walk(myproc()->pagetable, start, 0);
    if(pte == 0){
      return -1;
    }
    if(*pte & PTE_A){
      bitmask_kernel |= 1 << i;
      *pte &= ~PTE_A;
    }
    start += PGSIZE;
  }

  if(copyout(myproc()->pagetable, bitmask_user_address, (char *)&bitmask_kernel, (npages+7)/8) == -1){
    return -1;
  }

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

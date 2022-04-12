#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->vlimit;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/**
 * Change the protection bits of the page range 
 * starting at addr and of len pages to be read only.
 */
int
sys_mprotect(void)
{
  cprintf("called sys_mprotect\n");

  // Retrieving first argument (virtual address)
  int addrInt;
  argint(0, &addrInt);
  // cprintf("addrInt: %d\n", addrInt);
  if(addrInt % PGSIZE != 0) {
    // address is not page aligned
    return -1;
  }

  // Retrieving second argument (length)
  int len;
  argint(1,&len);
  if(len <= 0) {
    // len must be > 0
    return -1;
  }

  // Pointer to Page Directory of my process
  pde_t *pgdir = myproc()->pgdir;
  // Pointer to Page Directory Entry
  pde_t *pde; 
  // Pointer to Page Table
  pte_t *pgtab;

  uint limit = addrInt + len*PGSIZE;
  uint mallocLimit = myproc()->vlimit;
  cprintf("mallocLimit: %d\n", mallocLimit);
  if(mallocLimit < limit) {
    // addr will point a part that is out of the address space
    return -1;
  }
  cprintf("limit: %d\n", limit);
  // Set the page range starting at addr and of len pages to be read only.
  for(int curAddr = addrInt; curAddr < limit; curAddr+=PGSIZE) {
    // Changing the address into a pointer
    const void* vAddr = (void*) curAddr; 

    // PDX(vAddr) => get Page Directory Index from vAddr
    // Get Page Directory Entry Pointer from Page Directory
    pde = &pgdir[PDX(vAddr)];

    // if Page Directory Entry is present (has present flag set)
    if(*pde & PTE_P){
      // PTE_ADDR(*pde) => Address given by Page Directory Entry
      // P2V(PTE_ADDR(*pde)) => Virtual vAddr of it
      // Get pointer to Page Table 
      pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    } else {
      // No Page Table at this Page Directory Entry, so error
      return -1;
    }
    // PTX(vAddr) => get Page Table Index from vAddr
    // Get pointer to Page Table Entry (from the Page Table)
    pte_t* pte = &pgtab[PTX(vAddr)];

    int oldValue = *pte;

    // Set Writable flag of the Page Table Entry to 0
    *pte &= ~(1 << PTE_W_INDEX);

    int newValue = *pte;
    cprintf("old: %d\n", oldValue);
    cprintf("new: %d\n", newValue);

    if ((*pte & (1 << PTE_W_INDEX)) != 0) {
      cprintf("current: %d PTE_W is 1 \n", vAddr);
    }
    if (!((*pte & (1 << PTE_W_INDEX)) != 0)) {
      cprintf("current: %d PTE_W is 0 \n", vAddr);
    }
  }

  lcr3(V2P(pgdir));

  return 0; // success
}

/**
 * Sets the region back to both readable and writeable.
 */
int
sys_munprotect(void)
{
  // Retrieving first argument (virtual address)
  int addrInt;
  argint(0, &addrInt);
  // cprintf("addrInt: %d\n", addrInt);
  if(addrInt % PGSIZE != 0) {
    // address is not page aligned
    return -1;
  }

  // Retrieving second argument (length)
  int len;
  argint(1,&len);
  if(len <= 0) {
    // len must be > 0
    return -1;
  }

  // Pointer to Page Directory of my process
  pde_t *pgdir = myproc()->pgdir;
  // Pointer to Page Directory Entry
  pde_t *pde; 
  // Pointer to Page Table
  pte_t *pgtab;

  uint limit = addrInt + len*PGSIZE;
  uint mallocLimit = myproc()->vlimit;
  cprintf("mallocLimit: %d\n", mallocLimit);
  if(mallocLimit < limit) {
    // addr will point a part that is out of the address space
    return -1;
  }
  cprintf("limit: %d\n", limit);
  // Set the page range starting at addr and of len pages to be read only.
  for(int curAddr = addrInt; curAddr < limit; curAddr+=PGSIZE) {
    // Changing the address into a pointer
    const void* vAddr = (void*) curAddr; 

    // PDX(vAddr) => get Page Directory Index from vAddr
    // Get Page Directory Entry Pointer from Page Directory
    pde = &pgdir[PDX(vAddr)];

    // if Page Directory Entry is present (has present flag set)
    if(*pde & PTE_P){
      // PTE_ADDR(*pde) => Address given by Page Directory Entry
      // P2V(PTE_ADDR(*pde)) => Virtual vAddr of it
      // Get pointer to Page Table 
      pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    } else {
      // No Page Table at this Page Directory Entry, so error
      return -1;
    }
    // PTX(vAddr) => get Page Table Index from vAddr
    // Get pointer to Page Table Entry (from the Page Table)
    pte_t* pte = &pgtab[PTX(vAddr)];

    int oldValue = *pte;

    // Set Writable flag of the Page Table Entry to 0
    *pte |= 1 << PTE_W_INDEX;

    int newValue = *pte;
    cprintf("old: %d\n", oldValue);
    cprintf("new: %d\n", newValue);

    if ((*pte & (1 << PTE_W_INDEX)) != 0) {
      cprintf("current: %d PTE_W is 1 \n", vAddr);
    }
    if (!((*pte & (1 << PTE_W_INDEX)) != 0)) {
      cprintf("current: %d PTE_W is 0 \n", vAddr);
    }
  }

  lcr3(V2P(pgdir));

  return 0; // success
}
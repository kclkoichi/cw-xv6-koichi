#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
 
int 
main(void) {
  void * a = malloc(PGSIZE*4);
  
  printf(1, "Test normal use of mprotect: ");
  int res1 = mprotect(a, 2);
  if (res1 == 0) { printf(1, "SUCCESS\n"); } else { printf(1, "FAILURE\n"); }

  printf(1, "Test write to mprotected area of code: should crash\n");
  int * b = (int*) a;
  *b = 0x0000;

  exit();
}
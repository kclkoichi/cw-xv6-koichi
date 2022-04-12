#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
 
int 
main(void) {
  void * a = malloc(PGSIZE*3);
  printf(1, "return val of mprotect call is %d\n\n", mprotect(a, 2));
  exit();
}
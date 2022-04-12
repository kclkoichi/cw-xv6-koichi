#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
 
int 
main(void) {
  void * a = malloc(PGSIZE);
  void * b = malloc(PGSIZE);
  printf(1, "return val of mprotect call is %d\n\n", mprotect(a, 100));
  printf(1, "return val of munprotect call is %d\n", munprotect(b, 200));
  exit();
}
#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
 
int 
main(void) {
  printf(1, "Test normal use of munprotect: ");
  void * a = malloc(PGSIZE*4);
  int res1 = munprotect(a, 4);
  if (res1 == 0) { printf(1, "SUCCESS\n\n"); } else { printf(1, "FAILURE\n\n"); }

  printf(1, "Test len is less than or equal to zero:\n");
  int res2 = munprotect(a, 0); // len is 0; and 0 <= 0
  if (res2 == -1) { printf(1, "SUCCESS\n\n"); } else { printf(1, "FAILURE\n\n"); }

  printf(1, "Test addr is not page aligned:\n");
  void * b = (void*) ((int*) malloc(PGSIZE*4) + 1); // not page aligned
  int res3 = munprotect(b, 2);
  if (res3 == -1) { printf(1, "SUCCESS\n\n"); } else { printf(1, "FAILURE\n\n"); }

  printf(1, "Test addr will point to a region that is not currently a part of the address space:\n");
  void * c = malloc(PGSIZE*4);
  int res4 = munprotect(c, 5); // 5 > 4
  if (res4 == -1) { printf(1, "SUCCESS\n"); } else { printf(1, "FAILURE\n"); }

  exit();
}
#include "types.h"
#include "stat.h"
#include "user.h"
 
int 
main(void) {
  printf(1, "return val of mprotect call is %d\n", mprotect());
  printf(1, "return val of munprotect call is %d\n", munprotect());
  exit();
}
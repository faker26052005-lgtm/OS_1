#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "usage: trace mask program [args...]\n");
    exit(1);
  }

  int mask = atoi(argv[1]);

  if(trace(mask) < 0){ 
    fprintf(2, "trace: syscall failed\n");
    exit(1);
  }

  if(argc >= 3){
    exec(argv[2], &argv[2]);
    fprintf(2, "trace: exec %s failed\n", argv[2]);
    exit(1);
  }

  return 0;
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(2, "Usage: runpt <command> [args...]\n");
        exit(1);
    }

    //enable print page table mode
    trace_pgtbl();

    //run command that use call
    exec(argv[1], &argv[1]);

    fprintf(2, "exec %s failed\n", argv[1]);
    exit(1);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/sysinfo.h" //include sysinfo struct

void
sysinfotest(void)
{
    struct sysinfo _sysinfo;

    printf("sysinfotest: running \n");

    if(sysinfo(&_sysinfo) < 0)
    {
        fprintf(2, "sysinfotest: sysinfo failed\n");
        exit(1);
    }

    printf("freemem: %lu\n", _sysinfo.freemem);
    printf("nproc: %lu\n", _sysinfo.nproc);
    printf("nopenfiles: %lu\n", _sysinfo.nopenfiles);

    printf("sysinfotest: OK\n");
}

int
main(void)
{
    sysinfotest();
    exit(0);
}
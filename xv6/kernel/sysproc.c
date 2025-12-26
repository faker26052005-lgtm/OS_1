#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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

//implement system call sys_trace
uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()->mask = mask;
  return 0;
}

//declare functions to collect required info that are implemented in 
extern uint64 get_freemem(void); //
extern uint64 get_nproc(void); //proc.c
extern uint64 get_nopenfiles(void); //file.c

//implement system call sys_sysinfo
uint64
sys_sysinfo(void)
{
    uint64 address;
    struct sysinfo _sysinfo;

    //get address of sysinfo struct pointer from user space
    argaddr(0, &address);

    //get requried info
    _sysinfo.freemem = get_freemem();
    _sysinfo.nproc = get_nproc();
    _sysinfo.nopenfiles = get_nopenfiles();

    //copy sysinfo struct that contains required info to User space at address 
    if(copyout(myproc()->pagetable, address, (char*)&_sysinfo, sizeof(_sysinfo)) < 0)
      return (uint64)-1;

    return 0;
}
uint64
sys_pgaccess(void)
{
  uint64 va;      // Địa chỉ ảo bắt đầu
  int num_pages;  // Số lượng trang cần kiểm tra
  uint64 user_abitsaddr; // Địa chỉ buffer bitmask ở user space
  
  // 1. Lấy các đối số từ system call
  argaddr(0, &va);
  argint(1, &num_pages);
  argaddr(2, &user_abitsaddr);

  // Giới hạn số lượng trang để tránh tràn buffer (tối đa 64 trang cho uint64)
  if(num_pages > 64) return -1;

  struct proc *p = myproc();
  uint64 bitmask = 0;

  // 2. Lặp qua từng trang
  for(int i = 0; i < num_pages; i++){
    uint64 current_va = va + i * PGSIZE;
    
    // Sử dụng hàm walk() từ vm.c để tìm PTE
    pte_t *pte = walk(p->pagetable, current_va, 0);
    
    if(pte != 0 && (*pte & PTE_V) && (*pte & PTE_A)){
      // Nếu trang tồn tại, hợp lệ và đã được truy cập
      bitmask |= (1L << i);
      
      // QUAN TRỌNG: Xóa bit A sau khi kiểm tra để lần sau nhận diện chính xác
      *pte &= ~PTE_A;
    }
  }

  // 3. Copy kết quả bitmask về user space
  if(copyout(p->pagetable, user_abitsaddr, (char *)&bitmask, sizeof(bitmask)) < 0)
    return -1;

  return 0;
}
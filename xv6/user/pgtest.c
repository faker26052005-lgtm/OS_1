#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"

void
pgaccess_test()
{
  char *buf;
  uint abits;
  int npage = 32; // Kiểm tra 32 trang

  printf("Pgaccess starting...\n");

  // 1. Cấp phát 32 trang bộ nhớ bằng sbrk
  buf = sbrk(npage * PGSIZE);
  if(buf == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // 2. Truy cập một số trang cụ thể
  // Ví dụ: truy cập trang 0, trang 2 và trang 30
  buf[0 * PGSIZE] = 'a';
  buf[2 * PGSIZE] = 'b';
  buf[30 * PGSIZE] = 'c';

  // 3. Gọi system call pgaccess
  // Đối số: địa chỉ bắt đầu, số trang, và địa chỉ lưu bitmask
  if (pgaccess(buf, npage, &abits) < 0) {
    printf("pgaccess failed\n");
    exit(1);
  }

  // 4. Kiểm tra kết quả
  // Kỳ vọng: bit 0, 2 và 30 phải được bật (bằng 1)
  if (abits == ((1 << 0) | (1 << 2) | (1 << 30))) {
    printf("Successed! Bitmask: 0x%x\n", abits);
  } else {
    printf("Failed. Expected bitmask 0x%x, reality bitmask 0x%x\n", 
       ((1 << 0) | (1 << 2) | (1 << 30)), abits);
  }

  // Gọi lần 2 ngay lập tức: Kỳ vọng bitmask phải bằng 0 
  // (Vì kernel phải xóa bit PTE_A sau lần gọi đầu tiên)
  if (pgaccess(buf, npage, &abits) < 0) {
    printf("pgaccess failed\n");
    exit(1);
  }
  if (abits != 0) {
    printf("Eror: bit mask must be reset to 0, but reality bitmask was 0x%x\n", abits);
    exit(1);
  }

  printf("pgaccess_test: OK\n");
  exit(0);
}

int
main(int argc, char *argv[])
{
  pgaccess_test();
  return 0;
}
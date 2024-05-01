
#include <cstdint>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>

/*
 * This is it. We're going cool mode.
 *
 * GPIO Address Range:
 * 0х0300 B000---0×0300 B3FF
 *
 */

int main() {

    printf("Hello 0х0300 B000---0×0300 B3FF\n");

    uint32_t pd15_offset = 0x007;
    uint32_t base_addr = 0x0300B000;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Unable to open /dev/mem");
        return -1;
    }

    uint32_t* reg_base = (uint32_t*) mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);

    if (reg_base == MAP_FAILED) {
        perror("Error mapping memory");
        return -1;
    }

    printf("%d\n", *reg_base);

    munmap(reg_base, 32);

    return 0;
}

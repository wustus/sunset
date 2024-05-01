
#include <cstdint>
#include <iostream>
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

    uint32_t pd15_addr = 0x0300B000 + 0x007;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf("Unable to open /dev/mem\n");
        return -1;
    }

    uint32_t* reg_base = (uint32_t*) mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pd15_addr);

    printf("%d\n", *reg_base);

    return 0;
}

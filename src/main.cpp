
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <iomanip>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <chrono>
#include <thread>

/*
 * This is it. We're going cool mode.
 *
 * GPIO Address Range:
 * 0х0300 B000---0×0300 B3FF
 *
 */

typedef unsigned char uchar;

void print_binary(uchar c) {

    for (int i=7; i>=0; i--) {
        printf("%d", (c >> i) & 1);
    }
}

void pb(uint8_t c) {

    for (int i=7; i>=0; i--) {
        printf("%d", (c >> i) & 1);
    }
}

int print_register(uint32_t addr, uint16_t bytes) {

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Unable to open /dev/mem");
        return -1;
    }

    uchar* reg_base = (uchar*) mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);

    if (reg_base == MAP_FAILED) {
        perror("Error mapping memory");
        return -1;
    }

    uchar* reg = new uchar(bytes);
    std::memcpy(reg, reg_base, bytes);

    for (int i=0; i!=bytes; i+=4) {
        printf("%p - ", addr + i);
        print_binary(reg[i]);
        printf(" ");
        print_binary(reg[i+1]);
        printf(" ");
        print_binary(reg[i+2]);
        printf(" ");
        print_binary(reg[i+3]);
        printf("\n");
    }

    munmap(reg_base, 1024);
    return 0;
}

void sleep(std::chrono::microseconds duration) {
    std::this_thread::sleep_for(duration);
}

int main() {

    printf("Hello 0х0300 B000---0×0300 B3FF\n");

    print_register(0x0300B000, 1024);
    return 0;

    uint32_t pd15_offset = 0x0070;
    uint32_t base_addr = 0x0300B000;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Unable to open /dev/mem");
        return -1;
    }

    uchar* reg_base = (uchar*) mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);

    if (reg_base == MAP_FAILED) {
        perror("Error mapping memory");
        return -1;
    }

    uchar* gpio_values = new uchar[1024];

    uchar* on = new uchar[1];
    *on = 0b00000000;
    uchar* off = new uchar[1];
    *off = 0b01000000;

    // for (int i=0; i!=128; i++) {
    //     printf("%x - ", base_addr + 0x70 + i);
    //     printf("%x - ", reg_base + 0x70 + i);
    //     pb(*(uchar*)(reg_base + 0x70 + i));
    //     printf("\n");
    // }
    // return 0;

    while (true) {
        *(uchar*)(reg_base + 0x7d) = *off;
        pb(*(uchar*)(reg_base + 0x7d));
        printf("\n");
        sleep(std::chrono::microseconds(10000));
        *(uchar*)(reg_base + 0x7d) = *on;
        pb(*(uchar*)(reg_base + 0x7d));
        printf("\n");
        sleep(std::chrono::microseconds(10000));
    }

    // void* pd_conf_1_base_addr = (void*)((uintptr_t) reg_base + pd15_offset);
    // uint32_t pd_conf_1 = *(uint32_t*) pd_conf_1_base_addr;

    uchar pd_conf_1_addr = gpio_values[0x7d]; // gpio_values + 0x0070;
                                                      // printf("%d\n", (*pd_conf_1_addr & (0b111 << 4)) >> 4);
    print_binary(pd_conf_1_addr);


    munmap(reg_base, 1024);

    return 0;
}

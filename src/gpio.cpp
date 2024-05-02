
#include "gpio.h"

#include <cstdint>
#include <cstdio>
#include <sys/fcntl.h>
#include <sys/mman.h>

GPIO gpio;

int init() {

    gpio.fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (gpio.fd == -1) {
        perror("Unable to open /dev/mem");
        return -1;
    }

    gpio.reg_base = (uchar*) mmap(NULL, GPIO_MEMORY_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, gpio.fd, GPIO_BASE);
    
    if (gpio.reg_base == MAP_FAILED) {
        perror("Error mapping memory");
        return -1;
    }

    return 0;
}

void clean() {
    munmap(gpio.reg_base, 1024);
}


void* get_register(uint32_t reg, uint32_t pin) {

    uchar* reg_base = (uchar*) gpio.reg_base;
    return (void*) (reg_base + pin * REG_N + reg);
}


void set_pin_direction(uint32_t pin, uint32_t pin_id, uchar dir) {

    void* reg = get_register(CFG_1, pin);

    int32_t byte_offset = (pin_id % 8) / 2;

    // pid % 2 == 1 is more significant than pid % 2 == 0
    uchar n_shift = ((pin_id % 2) * 4);
    uchar n_shift_mask = ((1 - pin_id % 2) * 4);
    uchar pin_mask = 0x8 << n_shift_mask;

    // read out pin register of pin
    uchar* pin_reg = (uchar*) reg + byte_offset;
    uchar pin_reg_val = *pin_reg;


    pin_reg_val &= pin_mask;
    pin_reg_val |= dir << n_shift;

    *pin_reg = pin_reg_val;
}


void set_pin_value(uint32_t pin, uint32_t pin_id, uchar val) {

    void* reg = get_register(DAT, pin);
    uint32_t byte_offset = pin_id / 8;

    uchar pin_mask = ~(0x1 << (pin_id % 8));
    uchar pin_val = val << (pin_id % 8);

    uchar* pin_reg = (uchar*) reg + byte_offset;
    uchar pin_reg_val = *pin_reg;

    pin_reg_val &= pin_mask;
    pin_reg_val |= pin_val;

    *pin_reg = pin_reg_val;
}

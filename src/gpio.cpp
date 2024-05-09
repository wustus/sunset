
#include "gpio.h"
#include "helper.h"

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

    gpio.reg_base = (uint32_t*) mmap(NULL, GPIO_MEMORY_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, gpio.fd, GPIO_BASE);
    
    if (gpio.reg_base == MAP_FAILED) {
        perror("Error mapping memory");
        return -1;
    }

    // step pin
    set_pin_direction(PD, 15, CFG_OUT);
    // direction pin
    set_pin_direction(PD, 16, CFG_OUT);

    return 0;
}

void clean() {
    set_pin_value(nullptr, PD, 16, 0);
    print_memory(gpio.reg_base, 32);
    munmap(gpio.reg_base, GPIO_MEMORY_LENGTH);
}

uint32_t get_conf_register(uint32_t pin_id) {

    if (pin_id < 8) {
        return CFG_0;
    } else if (pin_id < 16) {
        return CFG_1;
    } else if (pin_id < 24) {
        return CFG_2;
    }

    return CFG_3;
}


uint32_t* get_register(uint32_t reg, uint32_t pin) {

    uint32_t* reg_base = reinterpret_cast<uint32_t*>(gpio.reg_base);

    // account for uint32_t size
    uint32_t reg_offset = pin * REG_N + reg;
    reg_offset /= 4;

    return reg_base + reg_offset;
}


void set_pin_direction(uint32_t pin, uint32_t pin_id, uint32_t dir) {

    uint32_t conf_reg = get_conf_register(pin_id);
    uint32_t* reg = get_register(conf_reg, pin);

    uint32_t byte_offset = (pin_id % 4);

    // see ordering of pins | / 15 / 14 | / 13 / 12 | ...
    uint32_t n_shift = ((pin_id % 8) * 4);
    uint32_t pin_mask = ~(0x7 << n_shift);

    uint32_t reg_val = *reg;

    reg_val &= pin_mask;
    reg_val |= dir << n_shift;

    *reg = reg_val;
}


uint32_t* set_pin_value(uint32_t* reg, uint32_t pin, uint32_t pin_id, uint32_t val) {

    if (reg == nullptr) {
        reg = get_register(DAT, pin);
    }

    uint32_t pin_mask = ~(0x1 << pin_id);
    uint32_t pin_val = val << pin_id;

    uint32_t reg_val = *reg;

    reg_val &= pin_mask;
    reg_val |= pin_val;

    *reg = reg_val;

    return reg;
}

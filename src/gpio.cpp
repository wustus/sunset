
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
    printf("Base Address: %p\n", gpio.reg_base);
    
    if (gpio.reg_base == MAP_FAILED) {
        perror("Error mapping memory");
        return -1;
    }

    return 0;
}

void clean() {
    munmap(gpio.reg_base, GPIO_MEMORY_LENGTH);
}


uint32_t* get_register(uint32_t reg, uint32_t pin) {

    uint32_t* reg_base = reinterpret_cast<uint32_t*>(gpio.reg_base);
    printf("Memory Address: %x\n", (GPIO_BASE + pin * REG_N + reg));

    uint32_t reg_offset = pin * REG_N + reg;
    // account for uint32_t size
    reg_offset /= 4;

    return reg_base + reg_offset;
}


void set_pin_direction(uint32_t pin, uint32_t pin_id, uint32_t dir) {

    printf("PIN P%c%d\n", (char) (pin + 65), pin_id);
    printf("Direction: "); print_byte(dir); 

    printf("\n");

    uint32_t* reg = get_register(CFG_1, pin);

    uint32_t byte_offset = (pin_id % 4);

    printf("Mapped Register: %p\n", reg);

    printf("Register: \n");
    print_register(reg);
    printf("\n");

    printf("Byte Offset: %x\n", byte_offset);

    // see ordering of pins | / 15 / 14 | / 13 / 12 | ...
    uint32_t n_shift = ((pin_id % 8) * 4);
    uint32_t pin_mask = ~(0x7 << n_shift);

    printf("Pin Mask: "); print_byte(pin_mask);

    uint32_t reg_val = *reg;

    printf("Register Value: ");
    print_register(reg);

    reg_val &= pin_mask;
    reg_val |= dir << n_shift;

    printf("New Register Value: ");
    print_byte(reg_val);

    *reg = reg_val;

    printf("Register: \n");
    print_register(reg);
}


void set_pin_value(uint32_t pin, uint32_t pin_id, uint32_t val) {

    printf("PIN P%c%d\n", (char) (pin + 65), pin_id);
    printf("Value: "); print_byte(val); 

    uint32_t* reg = get_register(DAT, pin);

    printf("Mapped Register: %p\n", reg);

    uint32_t pin_mask = ~(0x1 << pin_id);
    uint32_t pin_val = val << pin_id;

    printf("Pin Mask: "); print_byte(pin_mask); 
    printf("Pin Value: "); print_byte(pin_val); 

    uint32_t reg_val = *reg;

    printf("Register Value: ");
    print_byte(reg_val);

    reg_val &= pin_mask;
    reg_val |= pin_val;

    *reg = reg_val;

    printf("New Register Value: ");
    print_byte(reg_val);
}

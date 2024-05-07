
#ifndef gpio_h
#define gpio_h

#include <chrono>
#include <cstdint>

#include "helper.h"


// gpio base address
#define GPIO_BASE 0x0300B000
#define GPIO_MEMORY_LENGTH 0x3FF

#define PC 0x2
#define PD 0x3
#define PF 0x5
#define PG 0x6
#define PH 0x7

/**
 * register selector - multiply REG_N by addressed pin reg
 *   PC: 2
 *   PD: 3
 *   PE: 4
 *   ...
 **/
#define REG_N 0x0024

/**
 * register offsets
 *   pins are ordered in min-max in reversed order
 *   0x0048: / PC7 / PC6 / PC5 / PC4 / PC3 / PC2 / PC1 / PC0
 *   0x004C: / PC15 / PC14 / PC13 / PC12 / PC10 / PC9 / PC8
 *   ...
 *
 *   |PCn| = 3 bit
 *    /    = 1 bit
 *
 *   &PXn = GPIO_BASE + X * REG_N + (n // 8) 
 *
 *   default = 0x7
 **/
#define CFG_0 0x00
#define CFG_1 0x04
#define CFG_2 0x08
#define CFG_3 0x0C

// rest is reserved
#define CFG_IN          0x0
#define CFG_OUT         0x1
#define CFG_NAND_DQ4    0x2
#define CFG_DC2_D4      0x3
#define CFG_DISABLE     0x7

/**
 * data register
 *
 * 32 bits, each bit corresponds to a pin in the above order
 * value can be 1 or 0,
 **/
#define DAT 0x10

/**
 * hi jt, if you ever need any more pins, sorry but i was lazy
 **/

struct GPIO {
    int fd;
    uint32_t* reg_base;
};

#pragma pack(push, 1)
struct motorcontrol {
    float duration;
    uint8_t hz;
    bool clockwise;
};
#pragma pack(pop)

int init();
void clean();

uint32_t get_conf_register(uint32_t pin_id);
uint32_t* get_register(uint32_t reg, uint32_t pin);
void set_pin_direction(uint32_t pin, uint32_t pin_id, uint32_t dir);
void set_pin_value(uint32_t pin, uint32_t pin_id, uint32_t val);


#endif

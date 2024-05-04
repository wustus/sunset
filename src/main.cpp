
#include "gpio.h"
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <chrono>
#include <csignal>

/*
 * This is it. We're going cool mode.
 *
 * GPIO Address Range:
 * 0х0300 B000---0×0300 B3FF
 *
 */

void handle_signal(int signal) {
    clean();
    exit(signal);
}

int main() {

    std::signal(SIGINT, handle_signal);

    int err = init();

    if (err != 0) {
        printf("exit 1\n");
        exit(1);
    }

    set_pin_direction(PD, 15, CFG_OUT);
    set_pin_direction(PD, 16, CFG_OUT);

    for (int i=0; i!=1600; i++) {
        set_pin_value(PD, 15, 1);
        sleep(std::chrono::microseconds(625));
        set_pin_value(PD, 15, 0);
        sleep(std::chrono::microseconds(625));
    }
    
    set_pin_value(PD, 16, 1);

    for (int i=0; i!=1600; i++) {
        set_pin_value(PD, 15, 1);
        sleep(std::chrono::microseconds(625));
        set_pin_value(PD, 15, 0);
        sleep(std::chrono::microseconds(625));
    }

    clean();

    return 0;
}

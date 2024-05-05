
#include "gpio.h"
#include "net.h"
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

    // int err = init();

    // if (err != 0) {
    //     printf("exit 1\n");
    //     exit(1);
    // }

    create_socket();
    bind_socket();
    listen_on_socket();
    int sckt = accept_connection();

    char* data = read_from_socket(sckt, sizeof(MotorControl));

    MotorControl* controls = reinterpret_cast<MotorControl*>(data);

    // in seconds
    float t_sleep = 1.0f / (controls->hz * 2);

    if (controls->clockwise) {
        set_pin_value(PD, 16, 1);
    }

    for (int i=0; i!=controls->hz * controls->duration; i++) {
        set_pin_value(PD, 15, 1);
        sleep(std::chrono::microseconds((uint32_t) t_sleep * 1'000'000));
        set_pin_value(PD, 15, 0);
        sleep(std::chrono::microseconds((uint32_t) t_sleep * 1'000'000));
    }

    clean();

    return 0;
}

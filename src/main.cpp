
#include "gpio.h"
#include "helper.h"
#include "net.h"
#include <cstdint>
#include <cstdlib>
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

    create_socket();
    bind_socket();
    listen_on_socket();
    int sckt = accept_connection();

    if (sckt == -1) {
        std::cout << "Failed idk" << std::endl;
        return 1;
    }

    while (is_socket_connected(sckt)) {

        MotorControl* controls = new MotorControl;
        std::memset(controls, 0, sizeof(MotorControl));
        read_from_socket(sckt, controls, sizeof(MotorControl));

        if (*((char*) controls) == 0) {
            sleep(std::chrono::microseconds(1'000'000));
            delete controls;
            continue;
        }

        std::cout << "Motor Controls:" << std::endl;
        std::cout << "Hz - " << (unsigned) controls->hz << std::endl;
        std::cout << "Duration - " << (unsigned) controls->duration << std::endl;
        std::cout << "Clockwise - " << controls->clockwise << std::endl;

        int hz = (int) controls->hz;
        int duration = (int) controls->duration;

        // in seconds
        float t_sleep = 1.0f / (1600 * hz * 2);

        if ((bool) controls->clockwise) {
            set_pin_value(PD, 16, 1);
        } else {
            set_pin_value(PD, 16, 0);
        }

        std::cout << "Steps: " << hz * duration * 1600 << std::endl;
        std::cout << "Timeout: " << (uint32_t) (t_sleep * 1'000'000) << std::endl;

        for (int i=0; i!=hz * duration * 1600; i++) {
            set_pin_value(PD, 15, 1);
            sleep(std::chrono::microseconds((uint32_t) (t_sleep * 1'000'000)));
            set_pin_value(PD, 15, 0);
            sleep(std::chrono::microseconds((uint32_t) (t_sleep * 1'000'000)));
        }

        delete controls;
    }

    std::cout << "Socket disconnected." << std::endl;

    clean();

    return 0;
}

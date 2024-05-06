
#include "gpio.h"
#include "helper.h"
#include "net.h"
#include <cstdint>
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


    while (is_socket_connected(sckt)) {

        char* data = read_from_socket(sckt, sizeof(MotorControl));

        if (data == nullptr) {
            sleep(std::chrono::microseconds(10'000));
            continue;
        }

        MotorControl* controls = reinterpret_cast<MotorControl*>(data);

        std::cout << "Motor Controls:" << std::endl;
        std::cout << "Hz - " << (int) controls->hz << std::endl;
        std::cout << "Duration - " << (int) controls->duration << std::endl;
        std::cout << "Clockwise - " << controls->clockwise << std::endl;

        int hz = (int) controls->hz;
        int duration = (int) controls->duration;

        // in seconds
        float t_sleep = 1.0f / (1600 * hz * 2);

        if (controls->clockwise) {
            set_pin_value(PD, 16, 1);
        }

        std::cout << "Steps: " << hz * duration * 1600 << std::endl;
        std::cout << "Timeout: " << (uint32_t) (t_sleep * 1'000'000) << std::endl;

        for (int i=0; i!=hz * duration * 1600; i++) {
            set_pin_value(PD, 15, 1);
            sleep(std::chrono::microseconds((uint32_t) (t_sleep * 1'000'000)));
            set_pin_value(PD, 15, 0);
            sleep(std::chrono::microseconds((uint32_t) (t_sleep * 1'000'000)));
        }
    }

    clean();

    return 0;
}


#include "gpio.h"
#include "helper.h"
#include "net.h"
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

        motorcontrol* mctl = new motorcontrol;
        std::memset(mctl, 0, sizeof(motorcontrol));
        read_from_socket(sckt, mctl, sizeof(motorcontrol));

        if (*((float*) mctl) == 0) {
            sleep(std::chrono::microseconds(1'000));
            delete mctl;
            continue;
        }

        std::cout << "Motor Controls:" << std::endl;
        std::cout << "Hz - " << (float) mctl->hz << std::endl;
        std::cout << "Duration - " << (float) mctl->duration << std::endl;
        std::cout << "Clockwise - " << mctl->clockwise << std::endl;

        float hz = (float) mctl->hz;
        float duration = (float) mctl->duration;

        // in seconds
        float t_sleep = 1.0f / (1600 * hz * 2);

        if ((bool) mctl->clockwise) {
            set_pin_value(nullptr, PD, 16, 1);
        } else {
            set_pin_value(nullptr, PD, 16, 0);
        }

        int timeout = t_sleep * 1'000'000'000;

        std::cout << "Steps: " << hz * duration * 1600 << std::endl;
        std::cout << "Timeout: " << timeout << std::endl;

        auto initial_t = std::chrono::high_resolution_clock::now();
        int n_steps = (int)(hz * duration * 1600.0f);

        uint32_t* step_reg = nullptr;

        for (int i=0; i<(int)(hz * duration * 1600.0f); i++) {
            step_reg = set_pin_value(step_reg, PD, 15, 1);
            sleep_nano(std::chrono::nanoseconds(timeout));
            step_reg = set_pin_value(step_reg, PD, 15, 0);
            sleep_nano(std::chrono::nanoseconds(timeout));
        }

        auto end_t = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - initial_t);

        std::cout << elapsed.count() << " milliseconds passed" << std::endl;

        delete mctl;
    }

    std::cout << "Socket disconnected." << std::endl;

    clean();

    return 0;
}

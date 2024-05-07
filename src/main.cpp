
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
            sleep(std::chrono::microseconds(1'000'000));
            delete mctl;
            continue;
        }

        std::cout << "Motor Controls:" << std::endl;
        std::cout << "Hz - " << (unsigned) mctl->hz << std::endl;
        std::cout << "Duration - " << (float) mctl->duration << std::endl;
        std::cout << "Clockwise - " << mctl->clockwise << std::endl;

        int hz = (int) mctl->hz;
        float duration = (float) mctl->duration;

        // in seconds
        float t_sleep = 1.0f / (1600 * hz * 2);

        if ((bool) mctl->clockwise) {
            set_pin_value(PD, 16, 1);
        } else {
            set_pin_value(PD, 16, 0);
        }

        std::cout << "Steps: " << hz * duration * 1600 << std::endl;
        std::cout << "Timeout: " << (t_sleep * 1'000'000) << std::endl;

        for (int i=0; i!=hz * duration * 1600; i++) {
            set_pin_value(PD, 15, 1);
            sleep(std::chrono::microseconds((int) (t_sleep * 1'000'000)));
            set_pin_value(PD, 15, 0);
            sleep(std::chrono::microseconds((int) (t_sleep * 1'000'000)));
        }

        delete mctl;
    }

    std::cout << "Socket disconnected." << std::endl;

    clean();

    return 0;
}

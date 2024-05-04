
#include "helper.h"

template <typename T>
void print_byte(T t) {

    int n_b = sizeof(t);
    uchar* val = reinterpret_cast<uchar*>(&t);

    for (int b=n_b-1; b>=0; b--) {
        for (int i=7; i>=0; i--) {
            std::cout << ((*(val + b) >> i) & 1);
        }
        std::cout << " ";
    }

    std::cout << std::endl;
}

template <typename T>
void print_register(T* t) {

    int n_b = sizeof(t);

    if (n_b < 3) {
        return;
    }

    std::cout << t << " - ";

    uint32_t reg = *(uint32_t*) t;
    print_byte(reg);
}

void print_memory(uint32_t* addr, uint32_t bytes) {

    for (int i=0; i!=bytes; i++) {
        print_register(addr + i);
    }
}

void sleep(std::chrono::microseconds duration) {
    std::this_thread::sleep_for(duration);
}

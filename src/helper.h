
#ifndef helper_h
#define helper_h

#include <iostream>
#include <fcntl.h>
#include <chrono>
#include <thread>

typedef unsigned char uchar;

template <typename T>
void print_byte(T t);

template <typename T>
void print_register(T* t);

void print_memory(uint32_t* addr, uint32_t bytes);

inline void sleep(std::chrono::microseconds duration) {
    std::this_thread::sleep_for(duration);
}

inline void sleep_nano(std::chrono::nanoseconds duration) {

    auto now = std::chrono::high_resolution_clock::now();
    auto deadline = now + duration;

    while (std::chrono::high_resolution_clock::now() < deadline) {

    }
}

#endif

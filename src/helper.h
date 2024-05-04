
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

void sleep(std::chrono::microseconds duration);

#endif

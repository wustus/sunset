
#ifndef helper_h
#define helper_h

#include <iostream>

template <typename T>

void print_byte(T t) {

    for (int i=0; i!=8; i++) {
        std::cout << ((t >> 1) & 1);
    }

    std::cout << std::endl;
}

#endif

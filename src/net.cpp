
#include "net.h"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>

NetInfo info;

void set_local_address() {

    struct ifaddrs *ifaddr, *ifaddr_iter;

    void* addr;
    char ip_addr[INET_ADDRSTRLEN]; 

    if (getifaddrs(&ifaddr) < 0) {
        std::cerr << "Unable to get network interface address." << std::endl;
        exit(1);
    }

    for (ifaddr_iter=ifaddr; ifaddr_iter != nullptr; ifaddr_iter = ifaddr->ifa_next) {

        if (ifaddr_iter->ifa_addr == nullptr) {
            continue;
        }

        if (ifaddr_iter->ifa_addr->sa_family == AF_INET) {
            addr = &((sockaddr_in*) ifaddr_iter->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addr, ip_addr, INET_ADDRSTRLEN);

            #ifdef __APPLE__
            if (std::strcmp(ifaddr_iter->ifa_name, "en0") == 0 || std::strcmp(ifaddr_iter->ifa_name, "en1") == 0) {
                std::memcpy(info.addr, ip_addr, INET_ADDRSTRLEN);
            }
            #endif

            #ifdef __unix
            if (std::strcmp(ifaddr_iter->ifa_name, "wlan0") == 0) {
                std::memcpy(info.addr, ip_addr, INET_ADDRSTRLEN);
            }
            #endif
        }
    }

    printf("IP Addr: %s", ip_addr);
}

int create_socket() {

    int sckt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sckt == -1) {
        std::cerr << "Unable to create socket" << std::endl;
        exit(1);
    }

    return sckt;
}


void bind_socket() {

    int sckt = info.socket;

    struct sockaddr_in bind_addr;
    std::memset(&bind_addr, 0, sizeof(sockaddr));

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(COMM_PORT);
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sckt, (struct sockaddr*) &bind_addr, sizeof(sockaddr_in)) == -1) {
        close(sckt);
        std::cerr << "Unable to bind socket." << std::endl;
        exit(1);
    }

    info.bind_addr = bind_addr;

    int opt = 1;

    if (setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(sckt);
        std::cerr << "Unable to set Reuse Address option." << std::endl;
        exit(1);
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == 0) {
        close(sckt);
        std::cerr << "Unable to set Timeout socket option." << std::endl;
        exit(1);
    }
}

void listen_on_socket() {

    int sckt = info.socket;

    if (listen(sckt, 3) == -1) {
        std::cerr << "Unable to start listening." << std::endl;
        exit(1);
    }
}

int accept_connection() {

    int sckt = info.socket;
    socklen_t sckt_len = sizeof(info.bind_addr);

    int conn;

    if ((conn = accept(sckt, (sockaddr*) &info.bind_addr, &sckt_len)) == -1) {
        std::cerr << "Unable to accept connection." << std::endl;
        exit(1);
    }

    return conn;
}

char* read_from_socket(int sckt, size_t buff_size) {

    char* buffer = new char[buff_size];
    read(sckt, buffer, buff_size);

    return buffer;
}

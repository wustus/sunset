
#include "net.h"
#include <sys/socket.h>

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

void create_socket() {

    int sckt = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sckt == -1) {
        std::cerr << "Unable to create socket" << std::endl;
        exit(1);
    }

    info.socket = sckt;
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
        std::cerr << "Unable to bind socket: " << strerror(errno) <<  std::endl;
        exit(1);
    }

    info.bind_addr = bind_addr;

    int opt = 1;

    if (setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        close(sckt);
        std::cerr << "Unable to set Reuse Address option: " << strerror(errno) << std::endl;
        exit(1);
    }

    struct timeval timeout;
    std::memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        close(sckt);
        std::cerr << "Unable to set Timeout socket option: " << strerror(errno) << std::endl;
        exit(1);
    }
}

void listen_on_socket() {

    int sckt = info.socket;

    if (listen(sckt, 3) == -1) {
        std::cerr << "Unable to start listening: " << strerror(errno) << std::endl;
        exit(1);
    }
}

int accept_connection() {

    int sckt = info.socket;
    socklen_t sckt_len = sizeof(info.bind_addr);

    int conn;

    if ((conn = accept(sckt, (struct sockaddr*) &info.bind_addr, &sckt_len)) == -1) {
        std::cerr << "Unable to accept connection: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::cout << "Accepted connection." << std::endl;

    return conn;
}

char* read_from_socket(int sckt, size_t buff_size) {

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sckt, &read_fds);

    struct timeval timeout;
    std::memset(&timeout, 0, sizeof(timeval));

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int activity = select(sckt + 1, &read_fds, nullptr, nullptr, &timeout);

    if (activity < 0) {
        perror("Error when executing select");
        exit(1);
    }

    if (activity == 0) {
        std::cout << "Timeout" << std::endl;
        return nullptr;
    }

    if (FD_ISSET(sckt, &read_fds)) {
        char* buffer = new char[buff_size+1];
        ssize_t bytes_read = read(sckt, buffer, buff_size);

        if (bytes_read < buff_size) {
            perror("Read error");
            delete[] buffer;
            return nullptr;
        }

        buffer[buff_size] = '\0';

        return buffer;
    }

    std::cout << "FD not set" << std::endl;

    return nullptr;
}

bool is_socket_connected(int sckt) {

    int err = 0;
    socklen_t size = sizeof(int);

    return getsockopt(sckt, SOL_SOCKET, SO_ERROR, &err, &size) != -1;
}

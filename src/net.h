
#ifndef net_h
#define net_h

#include "helper.h"

#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

/**
 * We need two modes of operation:
 *  (1) - automatic
 *  (2) - manual
 *
 * Automatic:
 *  Three out of four Pi's set up a server and await thei controlling commands.
 *  The other Pi - the master (preconfigured), selects a precalculated route and sends signals to all other devices.
 *  Execution of these commands should be synchronous (NTP).
 *  While sending commands to the other devices, the master itself, executes signals via direct access or server.
 *
 * Manual:
 *  All Pi's run a server and await commands.
 *  These can be manually transmitted from another device.
 **/

#define COMM_PORT 7834

struct NetInfo {
    char addr[INET_ADDRSTRLEN];
    int socket;
    sockaddr_in bind_addr;
};

void set_local_address();

void create_socket();
void bind_socket();
void listen_on_socket();
int accept_connection();
char* read_from_socket(int, size_t);

#endif

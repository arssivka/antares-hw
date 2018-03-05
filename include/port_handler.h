#pragma once

#include <stdint.h>

#include "port_handler_al.h"


#define MAX_DEV_NAME_LENGTH 256

#define PORT_SUCESS 0
#define PORT_ERROR (-1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _port_handler {
    port_handle_t handler;
    char* dev_name;
    int baudrate;
    int is_used;
} port_handler;

port_handler* create_port_handler(const char* dev_name, int baudrate);

int open_port(port_handler* ph);

int set_baudrate(port_handler* ph, int baudrate);

int write_port(port_handler* ph, const uint8_t* data, int size);

int read_port(port_handler* ph, uint8_t* data, int size);

void close_port(port_handler* ph);

void release_port_handler(port_handler* ph);

#ifdef __cplusplus
}
#endif

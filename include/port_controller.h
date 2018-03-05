#pragma once

#include <stdint.h>
#include "port_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PORT_CONTROLLERS 10

typedef struct _port_controller {
    port_handler* ports[MAX_PORT_CONTROLLERS];
    int ports_count;
} port_controller;

typedef int(* data_cb)(int port, uint8_t* data, int read_bytes);

int add_port(port_controller* pc, const char* dev_name, int baudrate);

int open_ports(port_controller* pc);

int write_read_port(port_controller* pc, uint8_t port_id, uint8_t* data_in, int data_in_size, uint8_t* buffer,
                    int data_out_size, data_cb cb);

int write_read_ports(port_controller* pc, int* ids, uint8_t* data_in[], int* data_in_size, uint8_t* buffer,
                     int* data_out_size, int ids_count, data_cb cb);

int close_ports(port_controller* pc);

#ifdef __cplusplus
}
#endif
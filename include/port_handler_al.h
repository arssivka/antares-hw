#pragma once

#ifdef __linux__
typedef int port_handle_t;
#elif _WIN32
#include "Windows.h"
typedef HANDLE port_handle_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

port_handle_t open_port_helper(const char* dev_name);

int set_baudrate_helper(port_handle_t ph, int baudrate);

int write_port_helper(port_handle_t ph, const char* data, int size);

int read_port_helper(port_handle_t ph, char* data, int size);

void close_port_helper(port_handle_t ph);

#ifdef __cplusplus
}
#endif

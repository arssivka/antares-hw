#pragma once

#ifndef PORT_HANDLER_AL_H
#define PORT_HANDLER_AL_H

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#elif _WIN32
#include "Windows.h"
#endif

#ifdef __linux__
typedef int PORT_HANDLE;
#elif _WIN32
typedef HANDLE PORT_HANDLE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

PORT_HANDLE open_port_helper(const char *dev_name);

int set_baudrate_helper(PORT_HANDLE ph, int baudrate);

int write_port_helper(PORT_HANDLE ph, const  char *data, int size);

int read_port_helper(PORT_HANDLE ph, char *data, int size);

void close_port_helper(PORT_HANDLE ph);
	
#ifdef __cplusplus
}
#endif

#endif

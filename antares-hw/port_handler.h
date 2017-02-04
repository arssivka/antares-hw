#pragma once

#ifndef PORT_HANDLER_H
#define PORT_HANDLER_H

#include "port_handler_al.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DEV_NAME_LENGTH 256
	
#define PORT_SUCESS 0
#define PORT_ERROR -1

typedef struct _port_handler {
	PORT_HANDLE handler;
	char *dev_name;
	int baudrate;
	int is_used;
} port_handler;
	
port_handler * create_port_handler(const char *dev_name);

int open_port(port_handler *ph, int baudrate);

int set_baudrate(port_handler *ph, int baudrate);

int write_port(port_handler *ph, const  char *data, int size);

int read_port(port_handler *ph, char *data, int size);

void close_port(port_handler *ph);

void release_port_handler(port_handler *ph);

#ifdef __cplusplus
}
#endif

#endif

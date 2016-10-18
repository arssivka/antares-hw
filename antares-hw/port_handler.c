#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "port_handler.h"


port_handler * create_port_handler(const char *dev_name) {
	port_handler *ph;
	int dev_name_len;
	
	dev_name_len = (int)strlen(dev_name) + 1;
	if (dev_name == NULL || dev_name_len > MAX_DEV_NAME_LENGTH)
		return NULL;
	
	ph = malloc(sizeof(port_handler));
	
	ph->dev_name = malloc(dev_name_len);
	strcpy(ph->dev_name, dev_name);

	ph->handler = 0;
	ph->is_used = 0;
	
	return ph;
}


int open_port(port_handler *ph, int baudrate) {
	if (ph->handler > 0)
		return PORT_ERROR;
	
	if (ph->dev_name == 0)
		return PORT_ERROR;
	
	ph->handler = open_port_helper(ph->dev_name);
	if (ph->handler < 0)
		return PORT_ERROR;
	
	set_baudrate(ph, baudrate);
	
	return PORT_SUCESS;
}


int set_baudrate(port_handler *ph, int baudrate) {

	if (set_baudrate_helper(ph->handler, baudrate) != PORT_SUCESS)	{
		close_port(ph);
		return PORT_ERROR;
	}
	
	ph->baudrate = baudrate;
	
	return PORT_ERROR;
}


int write_port(port_handler *ph, const  char *data, int size) {
	int written = 0;
	ph->is_used = 1;
	written = write_port_helper(ph->handler, data, size);
	ph->is_used = 0;
	return written;
}


int read_port(port_handler *ph, char *data, int size) {
	int read = 0;
	ph->is_used = 1;
	read = read_port_helper(ph->handler, data, size);
	ph->is_used = 0;
	return read;
}


void close_port(port_handler *ph) {
	if (ph->handler > 0) {
		close_port_helper(ph->handler);
		ph->handler = 0;
	}
}


void release_port_handler(port_handler *ph) {
	close_port(ph);
	free(ph->dev_name);
	free(ph);
	ph = NULL;
}

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "port_handler.h"


port_handler * create_port_handler(const char *dev_name, int baudrate) {
	port_handler *ph;
	int dev_name_len;
	if (dev_name == NULL)
		return NULL;
	
	dev_name_len = (int)strlen(dev_name) + 1;
	
	if (dev_name_len < 1 || dev_name_len > MAX_DEV_NAME_LENGTH)
		return NULL;
	ph = (port_handler*)malloc(sizeof(port_handler));
	
	ph->dev_name = (char*)malloc(dev_name_len);
	strcpy(ph->dev_name, dev_name);
	
	ph->baudrate = baudrate;
	ph->handler = -1;
	ph->is_used = 0;
	
	return ph;
}


int open_port(port_handler *ph) {
	if (ph == NULL)
		return PORT_ERROR;
	
	if (ph->handler > 0)
		return PORT_ERROR;
	
	if (ph->dev_name == 0)
		return PORT_ERROR;
	
	ph->handler = open_port_helper(ph->dev_name);
	if ((int)ph->handler < 0)
		return PORT_ERROR;
	
	set_baudrate(ph, ph->baudrate);
	
	return PORT_SUCESS;
}


int set_baudrate(port_handler *ph, int baudrate) {
	if (ph->handler < 0)
		return PORT_ERROR;

	if (set_baudrate_helper(ph->handler, baudrate) != PORT_SUCESS) {
		close_port(ph);
		return PORT_ERROR;
	}
	
	ph->baudrate = baudrate;
	
	return PORT_ERROR;
}


int write_port(port_handler *ph, const uint8_t *data, int size) {
	int written = 0;
	ph->is_used = 1;
	written = write_port_helper(ph->handler, (char *)data, size);
	
	ph->is_used = 0;
	return written;
}


int read_port(port_handler *ph, uint8_t *data, int size) {
	int read = 0;
	ph->is_used = 1;
	read = read_port_helper(ph->handler, (char *)data, size);
	ph->is_used = 0;
	return read;
}


void close_port(port_handler *ph) {
	if (ph == NULL)
		return;
	if (ph->handler > 0) {
		close_port_helper(ph->handler);
		ph->handler = 0;
	}
}


void release_port_handler(port_handler *ph) {
	if (ph == NULL)
		return;
	close_port(ph);
	if (ph->dev_name != NULL)
		free(ph->dev_name);
	free(ph);
	ph = NULL;
}

#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#include "port_controller.h"

void prinths(uint8_t* pckt, int size, const char* name) {
#ifdef PRINT_PACKET
//	int i;
//	if (name)
//		printf("%s: ", name);
//	for (i = 0; i < size; i++) {
//		if (i > 0) 
//			printf(":");
//		printf("%02X", pckt[i]);
//	}
//	printf("\n");
#endif

    //DEBUG
}

/* add a fd to fd_set, and update max_fd */
static int safe_fd_set(int fd, fd_set* fds, int* max_fd) {
    assert(max_fd != NULL);

    FD_SET(fd, fds);
    if (fd > *max_fd) {
        *max_fd = fd;
    }
    return 0;
}

/* clear fd from fds, update max fd if needed */
static int safe_fd_clr(int fd, fd_set* fds, int* max_fd) {
    assert(max_fd != NULL);

    FD_CLR(fd, fds);
    if (fd == *max_fd) {
        (*max_fd)--;
    }
    return 0;
}

int add_port(port_controller* pc, const char* dev_name, int baudrate) {

    if (pc->ports_count < MAX_PORT_CONTROLLERS - 1) {
        pc->ports[pc->ports_count++] = create_port_handler(dev_name, baudrate);
        return PORT_SUCESS;
    } else return PORT_ERROR;
}

int open_ports(port_controller* pc) {

    int res;
    int i = 0;
    for (i = 0; i < pc->ports_count; i++) {
        printf("try open port %s\n", pc->ports[i]->dev_name);
        res = open_port(pc->ports[i]);
        printf("result : %d\n", res);
        if (res != PORT_SUCESS)
            close_ports(pc);
    }
}

int close_ports(port_controller* pc) {

    int res;
    int i = 0;
    for (i = 0; i < pc->ports_count; i++) {
        close_port(pc->ports[i]);
    }
    return PORT_SUCESS;
}

int write_read_ports(port_controller* pc, int* ids, uint8_t* data_in[], int* data_in_size, uint8_t* buffer,
                     int* data_out_size, int ids_count, data_cb cb) {
    fd_set rfds;
    int max_fd = 0;
    int cb_result;
    int written;
    int read_bytes;
    int i;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 5000;

    for (i = 0; i < ids_count; i++) {
        if (ids[i] < pc->ports_count) {
            safe_fd_set(pc->ports[ids[i]]->handler, &rfds, &max_fd);
        }
    }

    for (i = 0; i < ids_count; i++) {
        if (data_in_size[i]) {
            written = write_port(pc->ports[ids[i]], data_in[i], data_in_size[i]);
            prinths(data_in[i], written, "write");
            data_in_size[i] -= written;
            data_in[i] += written;
        }
    }

    int count = ids_count;
    if (1) {
        while (select(max_fd + 1, &rfds, NULL, NULL, NULL) > 0) {
            for (i = 0; i < ids_count; i++) {
                if (FD_ISSET(pc->ports[ids[i]]->handler, &rfds)) {
                    read_bytes = read_port(pc->ports[ids[i]], buffer, data_out_size[i]);

                    prinths(buffer, read_bytes, "read");
                    if (cb) {
                        cb_result = cb(ids[i], buffer, read_bytes);
                        data_out_size[i] -= cb_result;
                    }
                    if (!cb_result) {
                        --count;
                    }
                }
            }
            if (!count) {
#ifdef PRINT_PACKET
                printf("ready\n");
#endif
                return 0;
            }
        }
    }
    //usleep(5000);
}

int write_read_port(port_controller* pc, uint8_t port_id, uint8_t* data_in, int data_in_size, uint8_t* buffer,
                    int data_out_size, data_cb cb) {
    fd_set rfds;
    int max_fd = 0;
    int cb_result;
    int written;
    int read_bytes;
    int i;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    port_handler* ph = pc->ports[port_id];
    safe_fd_set(ph->handler, &rfds, &max_fd);
    //printf("ph = %d ", ph->handler);
    if (data_in_size) {
        written = write_port(ph, data_in, data_in_size);
        prinths(data_in, written, "write");
        data_in_size -= written;
        data_in += written;
    }
    if (!data_out_size)
        return 0;
    if (1)
        while (select(max_fd + 1, &rfds, NULL, NULL, &tv) > 0) {
            if (FD_ISSET(ph->handler, &rfds)) {
                read_bytes = read_port(ph, buffer, data_out_size);
                prinths(buffer, read_bytes, "read");
                if (read_bytes < 0) {
                    printf("failed\n");
                    return -1;
                }
                if (cb) {
                    cb_result = cb(port_id, buffer, read_bytes);
                    data_out_size -= cb_result;
                    if (!data_out_size) {
#ifdef PRINT_PACKET
                        printf("ready\n");
#endif
                        return 0;
                    }
                    if (!cb_result) {
                        printf("failed\n");
                        return 0;
                    }
                }
            }
        }
    else {
        while (data_out_size) {
            read_bytes = read_port(ph, buffer, data_out_size);

            /*if (read_bytes < 0)
            {
                printf("failed\n");
                return -1;
            }*/
            if (read_bytes && cb) {
                prinths(buffer, read_bytes, "read");
                cb_result = cb(port_id, buffer, read_bytes);
                data_out_size -= cb_result;
                if (!data_out_size) {
#ifdef PRINT_PACKET
                    printf("ready\n");
#endif
                    return 0;
                }
                if (!cb_result) {
                    printf("failed\n");
                    return 0;
                }
            }
        }
    }
}
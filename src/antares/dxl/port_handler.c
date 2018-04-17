///////////////////////////////////////////////////////////////////////////////
// Copyright 2018 Arseniy Ivin <arssivka@yandex.ru>                          //
//                                                                           //
// Licensed under the Apache License, Version 2.0 (the "License");           //
// you may not use this file except in compliance with the License.          //
// You may obtain a copy of the License at                                   //
//                                                                           //
//     http://www.apache.org/licenses/LICENSE-2.0                            //
//                                                                           //
// Unless required by applicable law or agreed to in writing, software       //
// distributed under the License is distributed on an "AS IS" BASIS,         //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  //
// See the License for the specific language governing permissions and       //
// limitations under the License.                                            //
///////////////////////////////////////////////////////////////////////////////

/// \file   port_handler.c
/// \author Arseniy Ivin
/// \date   2018/04/17

#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "port_handler.h"


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


port_handle_t port_open(const char* dev_name) {
    return port_open_linux(dev_name);
}


int port_set_baudrate(port_handle_t ph, int baudrate) {
    return port_set_baudrate_linux(ph, baudrate);
}


int port_write(port_handle_t ph, const uint8_t* buff, int buff_len) {
    return port_write_linux(ph, (char*) buff, buff_len);
}


int port_read(port_handle_t ph, uint8_t* buff, int buff_len) {
    return port_read_linux(ph, (char*) buff, buff_len);
}


int port_multiple_read(int ph_count, port_handle_t* phs, uint8_t* buff, int buff_len) {
    fd_set rfds;
    int max_fd;
    int i;
    int bytes_read;

    FD_ZERO(&rfds);

    for (i = 0; i < ph_count; i++)
        safe_fd_set(phs[i], &rfds, &max_fd);

    bytes_read = 0;
    if (select(max_fd + 1, &rfds, NULL, NULL, NULL) > 0) {
        for (i = 0; i < ph_count; i++) {
            if (FD_ISSET(phs[i], &rfds)) {
                bytes_read = port_read(phs[i], buff, buff_len);
                break;
            }
        }
    }
    return bytes_read;
}


void port_close(port_handle_t ph) {
    port_close_linux(ph);
}



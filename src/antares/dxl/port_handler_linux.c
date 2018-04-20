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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <linux/termios.h>
#include <string.h>
#include <termio.h>
#include "port_handler.h"


port_handle_t port_open_linux(const char* dev_name) {
    struct termios tios;

    port_handle_t ph;
    memset(&tios, 0, sizeof(tios));
    ph = open(dev_name, O_RDWR | O_NOCTTY);
    if (ph < 0)
        return 0;
    tios.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
    tios.c_iflag = IGNPAR;
    tios.c_oflag = 0;
    tios.c_lflag = 0;
    tios.c_cc[VTIME] = 0;
    tios.c_cc[VMIN] = 0;
    sleep(1);
    tcflush(ph, TCIFLUSH);
    tcsetattr(ph, TCSANOW, &tios);
    struct serial_rs485 rs485conf;

    if (ioctl(ph, TIOCGRS485, &rs485conf) < 0) {
        printf("Error: TIOCGRS485 ioctl not supported.\n");
    } else {
        rs485conf.flags |= SER_RS485_ENABLED;
        rs485conf.delay_rts_before_send = 0;
        rs485conf.delay_rts_after_send = 0;

        if (ioctl(ph, TIOCSRS485, &rs485conf) < 0) {
            printf("Error: TIOCSRS485 ioctl not supported.\n");
        }

    }

    return ph;
}


int port_set_baudrate_linux(port_handle_t ph, int baudrate) {
    struct termios2 t;
    if (ioctl(ph, TCGETS2, &t) < 0)
        return 0;
    t.c_cflag = (t.c_cflag & ~CBAUD) | BOTHER;
    t.c_ispeed = t.c_ospeed = baudrate;
    if (ioctl(ph, TCSETS2, &t) < 0)
        return 0;
    return 1;
}


int port_write_linux(port_handle_t ph, const char* data, int size) {
    return (int) write(ph, data, size);
}


int port_read_linux(port_handle_t ph, char* data, int size) {
    return (int) read(ph, data, (size_t) size);
}


void port_close_linux(port_handle_t ph) {
    close(ph);
}

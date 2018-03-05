#ifdef __linux__

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <linux/termios.h>
#include <string.h>

#elif _WIN32
#include "Windows.h"
#endif

#include "port_handler.h"


#ifdef __linux__

port_handle_t open_port_helper(const char* dev_name) {
    struct termios tios;
    port_handle_t ph;
    memset(&tios, 0, sizeof(tios));
    ph = open(dev_name, O_RDWR | O_NOCTTY);// | O_NONBLOCK);
    if (ph < 0)
        return PORT_ERROR;
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


int set_baudrate_helper(port_handle_t ph, int baudrate) {
    /*struct serial_struct ser;
    if (ioctl(ph, TIOCGSERIAL, &ser) < 0)
        return PORT_ERROR;
    ser.flags &= ~ASYNC_SPD_MASK;
    ser.flags |= ASYNC_SPD_CUST;
    ser.custom_divisor = ser.baud_base / baudrate;
    if (ioctl(ph, TIOCSSERIAL, &ser) < 0)
        return PORT_ERROR;*/
    struct termios2 t;
    if (ioctl(ph, TCGETS2, &t) < 0)
        return PORT_ERROR;
    t.c_cflag = (t.c_cflag & ~CBAUD) | BOTHER;
    t.c_ispeed = t.c_ospeed = baudrate;
    if (ioctl(ph, TCSETS2, &t) < 0)
        return PORT_ERROR;
    return PORT_SUCESS;
}


int write_port_helper(port_handle_t ph, const char* data, int size) {
    return write(ph, data, size);
}


int read_port_helper(port_handle_t ph, char* data, int size) {
    return read(ph, data, size);
}


void close_port_helper(port_handle_t ph) {
    close(ph);
}


#elif _WIN32


port_handle_t open_port_helper(const char *dev_name) {
    COMMTIMEOUTS timeouts;
    DWORD error;
    port_handle_t ph = CreateFileA(dev_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (ph < 0)
        return PORT_ERROR;

    if (SetCommMask(ph, 0) == FALSE)
        return PORT_ERROR;

    if (SetupComm(ph, 4096, 4096) == FALSE)
        return PORT_ERROR;

    if (PurgeComm(ph, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE)
        return PORT_ERROR;

    if (ClearCommError(ph, &error, NULL) == FALSE)
        return PORT_ERROR;

    if (GetCommTimeouts(ph, &timeouts) == FALSE)
        return PORT_ERROR;

    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 10000;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (SetCommTimeouts(ph, &timeouts) == FALSE)
        return PORT_ERROR;
    return ph;
}


int set_baudrate_helper(port_handle_t ph, int baudrate) {
    DCB dcb;
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(ph, &dcb) == FALSE)
        return PORT_ERROR;
    dcb.BaudRate = (DWORD)baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fParity = NOPARITY;
    dcb.fBinary = 1;
    dcb.fNull = 0;
    dcb.fAbortOnError = 0;
    dcb.fErrorChar = 0;
    dcb.fOutX = 0;
    dcb.fInX = 0;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fOutxCtsFlow = 0;

    if (SetCommState(ph, &dcb) == FALSE)
        return PORT_ERROR;
    return PORT_SUCESS;
}


int write_port_helper(port_handle_t ph, const  char *data, int size) {
    DWORD write = 0;
    if (WriteFile(ph, data, (DWORD)size, &write, NULL) == FALSE)
        return -1;
    return (int)write;
}


int read_port_helper(port_handle_t ph, char *data, int size) {
    DWORD read = 0;
    if (ReadFile(ph, data, (DWORD)size, &read, NULL) == FALSE)
        return -1;
    return (int)read;
}


void close_port_helper(port_handle_t ph) {
    CloseHandle(ph);
}


#endif

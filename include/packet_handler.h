#pragma once

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _packet {
    uint8_t buf[255];
    uint8_t* pos;
    uint8_t* data;
    uint8_t error;
    uint8_t id;
    uint8_t state;
    uint8_t checksum;
    uint8_t length;
    uint8_t data_size;
    uint8_t data_length;
    uint8_t left_bytes;
} dxl_packet;

uint8_t make_ping(uint8_t* buf, int buf_len, uint8_t id);

uint8_t make_write(uint8_t* buf, int buf_len, uint8_t id, uint8_t data_address, uint8_t* data, uint8_t data_size);

uint8_t make_read(uint8_t* buf, int buf_len, uint8_t id, uint8_t data_address, uint8_t data_length);

uint8_t make_same_multiple_read(uint8_t* buf, int buf_len, uint8_t* id, uint8_t data_address, uint8_t data_length,
                                uint8_t id_count, int* answer_length);

uint8_t make_same_addr_write(uint8_t* buf, int buf_len, uint8_t* id, uint8_t data_address, uint8_t data_length,
                             uint8_t* data, uint8_t id_count, int* answer_length);

void add_header(uint8_t* buf);

uint8_t calculate_checksum(uint8_t* buf);

uint8_t* validate_packet(dxl_packet* packet, uint8_t* buf, int buf_len);

void init_packet(dxl_packet* packet);

void add_checksum(uint8_t* buf);

int print_packet(dxl_packet* packet);

#ifdef __cplusplus
}
#endif
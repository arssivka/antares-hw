#include <memory.h>
#include <stdio.h>
#include "packet_handler.h"

enum {
    HEADER_1 = 0,
    HEADER_2 = 1,
    ID_POS = 2,
    PACKET_LENGTH_POS = 3,
    INSTRUCTION_POS = 4,
    PACKET_PARAM0_POS = 5,
};
enum {
    PING = 0x01,
    WRITE_DATA = 0x03,
    READ_DATA = 0x02,
    MULTIPLE_READ_DATA = 0X92,
    MULTIPLE_WRITE_DATA = 0X83
};
#define MAIN_HEADER_LENGTH 0x02
#define STATUS_PACKET_LENGTH 0x06
static const int BROADCAST_ID = 0xFE;

uint8_t calculate_checksum(uint8_t* buf) {
    int length = buf[PACKET_LENGTH_POS] + 1;
    int pos;
    uint8_t* buff_ptr;
    uint8_t res = 0;
    for (buff_ptr = &buf[ID_POS], pos = 0; pos < length; ++pos, ++buff_ptr) {
        res += *buff_ptr;
    }
    return ~res;
}

uint8_t make_ping(uint8_t* buf, int buf_len, uint8_t id) {// = { 0xFF, 0xFF, 0x01, 0x02, 0x01, 0xFB };
    add_header(buf);
    buf[ID_POS] = id;
    buf[INSTRUCTION_POS] = PING;
    buf[PACKET_LENGTH_POS] = MAIN_HEADER_LENGTH;
    add_checksum(buf);
    return buf[PACKET_LENGTH_POS] + 4;
}

uint8_t make_write(uint8_t* buf, int buf_len, uint8_t id, uint8_t data_address, uint8_t* data, uint8_t data_size) {
    add_header(buf);
    buf[ID_POS] = id;
    buf[INSTRUCTION_POS] = WRITE_DATA;
    buf[PACKET_LENGTH_POS] = MAIN_HEADER_LENGTH + 0x01 + data_size;
    buf[PACKET_PARAM0_POS] = data_address;
    memcpy(&buf[PACKET_PARAM0_POS + 1], data, data_size);
    add_checksum(buf);
    return buf[PACKET_LENGTH_POS] + 4;
}

uint8_t make_read(uint8_t* buf, int buf_len, uint8_t id, uint8_t data_address, uint8_t data_length) {
    add_header(buf);
    buf[ID_POS] = id;
    buf[INSTRUCTION_POS] = READ_DATA;
    buf[PACKET_LENGTH_POS] = MAIN_HEADER_LENGTH + 0x02;
    buf[PACKET_PARAM0_POS] = data_address;
    buf[PACKET_PARAM0_POS + 1] = data_length;
    add_checksum(buf);
    return buf[PACKET_LENGTH_POS] + 4;
}

uint8_t make_multiple_read(uint8_t* buf, int buf_len, uint8_t* id, uint8_t* data_address, uint8_t* data_length,
                           uint8_t id_count) {
    int i;
    add_header(buf);
    buf[ID_POS] = BROADCAST_ID;
    buf[INSTRUCTION_POS] = MULTIPLE_READ_DATA;
    buf[PACKET_LENGTH_POS] = MAIN_HEADER_LENGTH + 0x01 + 3 * id_count;
    buf[PACKET_PARAM0_POS] = 0x00;
    for (i = 0; i < id_count; ++i) {
        buf[PACKET_PARAM0_POS + 1] = data_length[i];
        buf[PACKET_PARAM0_POS + 1 + 1] = id[i];
        buf[PACKET_PARAM0_POS + i + 2] = data_address[i];
    }
    add_checksum(buf);
    return buf[PACKET_LENGTH_POS] + 4;
}

uint8_t make_same_multiple_read(uint8_t* buf, int buf_len, uint8_t* id, uint8_t data_address, uint8_t data_length,
                                uint8_t id_count, int* answer_length) {
    int i;
    add_header(buf);
    buf[ID_POS] = BROADCAST_ID;
    buf[INSTRUCTION_POS] = MULTIPLE_READ_DATA;
    buf[PACKET_LENGTH_POS] = MAIN_HEADER_LENGTH + 0x01 + 3 * id_count;
    buf[PACKET_PARAM0_POS] = 0x00;
    int start_pos = PACKET_PARAM0_POS + 1;
    for (i = 0; i < id_count; ++i) {
        buf[start_pos++] = data_length;
        buf[start_pos++] = id[i];
        buf[start_pos++] = data_address;
    }
    add_checksum(buf);
    *answer_length = (data_length + STATUS_PACKET_LENGTH) * id_count;
    return buf[PACKET_LENGTH_POS] + 4;
}

uint8_t
make_same_addr_write(uint8_t* buf, int buf_len, uint8_t* id, uint8_t data_address, uint8_t data_length, uint8_t* data,
                     uint8_t id_count, int* answer_length) {
    int i;
    add_header(buf);
    buf[ID_POS] = BROADCAST_ID;
    buf[INSTRUCTION_POS] = MULTIPLE_WRITE_DATA;
    buf[PACKET_LENGTH_POS] = MAIN_HEADER_LENGTH + 0x02 + (data_length + 1) * id_count;
    buf[PACKET_PARAM0_POS] = data_address;
    buf[PACKET_PARAM0_POS + 1] = data_length;
    int start_pos = PACKET_PARAM0_POS + 2;
    int data_pos = 0;
    int k;
    for (i = 0; i < id_count; ++i) {
        buf[start_pos++] = id[i];
        for (k = 0; k < data_length; ++k)
            buf[start_pos++] = data[data_pos++];
    }
    add_checksum(buf);
    *answer_length = 0;
    return buf[PACKET_LENGTH_POS] + 4;
}

void add_checksum(uint8_t* buf) {
    buf[PACKET_PARAM0_POS + buf[PACKET_LENGTH_POS] - 2] = calculate_checksum(buf);
}

void add_header(uint8_t* buf) {
    buf[HEADER_1] = 0xFF;
    buf[HEADER_2] = 0xFF;
}

uint8_t* validate_packet(dxl_packet* packet, uint8_t* buf, int buf_len) {
    uint8_t* end = buf + buf_len;
    uint8_t max_len;
    uint8_t checksum = 0;
    int cnt;

    while (buf < end && packet->state != 8) {
        switch (packet->state) {
            case 0:/*init state*/
                if (*(buf++) == 0xFF) {
                    packet->state = 1;
                    packet->left_bytes -= 1;
                }
                break;
            case 2:/*BASE_HEADER*/
                if (*(buf++) != 0xFF) {
                    packet->state = 0xFF;
                    packet->left_bytes = 0;
                    return buf;
                }
                packet->left_bytes -= 1;
                break;
            case 3:/*ID_HEADER*/
                packet->id = *(buf++);
                packet->left_bytes -= 1;
                break;
            case 4:/*LENGTH_HEADER*/
                packet->length = *(buf++);
                packet->left_bytes = packet->length;
                packet->data_length = packet->length - 2;
                break;
            case 5:/*ERROR_HEADER*/
                packet->error = *(buf++);
                packet->pos = packet->buf;
                packet->left_bytes -= 1;
                break;
            case 6:/*DATA*/
                max_len = packet->data_length < (end - buf) ? packet->data_length : (end - buf);
                memcpy(packet->buf, buf, max_len);
                buf += max_len;
                packet->data_size += max_len;
                packet->data_length -= max_len;
                if (packet->data_length)
                    --packet->state;
                packet->left_bytes -= max_len;
                break;
            case 7:/*CHECKSUM*/
                packet->checksum = *(buf++);
                packet->left_bytes -= 1;
                break;
            case 8:/*VALIDATE_CHECKSUM*/
                checksum += packet->id;
                checksum += packet->length;
                checksum += packet->error;
                for (cnt = 0; cnt < packet->length - 2; ++cnt) {
                    checksum += packet->buf[cnt];
                }
                checksum = ~checksum;
                if (checksum != packet->checksum) {
                    printf("packet error\n", packet->left_bytes);
                    packet->state = 0xFF;
                    return buf;
                }
                break;
            case 0xFF:
                return buf;
        }
        if (packet->state)
            ++packet->state;
    }
    return buf;
}

void init_packet(dxl_packet* packet) {
    memset(packet, 0, sizeof(dxl_packet));
    packet->left_bytes = 4;
}

int print_packet(dxl_packet* packet) {
#ifdef DEBUG

    printf("state :%d last bytes: %d id: %d length: %d error: %d\n", packet->state, packet->left_bytes, packet->id, packet->length, packet->error);
#endif // DEBUG


}
#include <memory.h>
#include <stdio.h>
#include "dxl_controller.h"


port_controller controller;
dxl_packet packets[10];
uint8_t input_buffer[10][255];

typedef struct _dxl_device {
    uint8_t id;
    uint8_t error;
    uint8_t buf[255];
} dxl_device;
dxl_device device_list[10][255];

int single_status(int port, uint8_t* data, int read_bytes) {
    uint8_t* buf = data;
    dxl_packet* packet = &packets[port];
    buf = validate_packet(packet, buf, read_bytes);
    if (packet->state == 0)
        return 0;
    if (packet->state == 8) {
        device_list[port][packet->id].id = packet->id;
        device_list[port][packet->id].error = packet->error;
    }
    return (int) (buf - data);
}

int multiple_read(int port, uint8_t* data, int read_bytes) {

    return 0;
}

int base_cb(int port, uint8_t* data, int read_bytes) {
    uint8_t* buf = data;
    int left_bytes;
    dxl_packet* packet = &packets[port];
    while (buf != data + read_bytes) {
        validate_packet(packet, buf, read_bytes);
        left_bytes += packet->left_bytes;
        if (packet->state == 8) {
            print_packet(packet);
            init_packet(packet);
        }
    }
    return left_bytes;
}

int register_port(const char* dev_name, int baudrate) {
    return add_port(&controller, dev_name, baudrate);
}

int init_dxl_controller() {
    return open_ports(&controller);
}

int finalize_dxl_controller() {
    return close_ports(&controller);
}

int bulk_write(int port_id, uint8_t addr, uint8_t data_length, uint8_t* device_ids, uint8_t* data, int devices) {
    int packet_length;
    int answer_length;
    init_packet(&packets[port_id]);
    packet_length = make_same_addr_write(input_buffer[port_id], 255, device_ids, addr, data_length, data, devices,
                                         &answer_length);
    write_read_port(&controller,
                    port_id,
                    input_buffer[port_id],
                    packet_length,
                    input_buffer[port_id],
                    0,
                    multiple_read);
}

int add_dxl_device(int port_id, int id) {
    int packet_length;
    init_packet(&packets[port_id]);
    packet_length = make_ping(input_buffer[port_id], 255, id);
    write_read_port(&controller, port_id, input_buffer[port_id],
                    packet_length, input_buffer[port_id], 6, single_status);
    if (device_list[port_id][id].id == id && !device_list[port_id][id].error) {
        return PORT_SUCESS;
    }
    return PORT_ERROR;
}

int ping_dxl_device(int port_id, int id) {
    int packet_length;
    init_packet(&packets[port_id]);
    packet_length = make_ping(input_buffer[port_id], 255, id);
    write_read_port(&controller,
                    port_id,
                    input_buffer[port_id],
                    packet_length,
                    input_buffer[port_id],
                    6,
                    single_status);
    if (device_list[port_id][id].id == id && !device_list[port_id][id].error) {
        return PORT_SUCESS;
    }
    return PORT_ERROR;
}

int read_dxl_device(int port_id, uint8_t* device_ids, int devices) {
    int packet_length;
    int answer_length;
    init_packet(&packets[port_id]);
    packet_length = make_same_multiple_read(input_buffer[port_id], 255, device_ids, 0, 2, devices, &answer_length);

    write_read_port(&controller,
                    port_id,
                    input_buffer[port_id],
                    packet_length,
                    input_buffer[port_id],
                    answer_length,
                    multiple_read);
    return PORT_ERROR;
}

int set_dxl_position(int broadcast, int port_id, uint8_t* device_ids, uint16_t* pos_list, int devices) {

    dxl_data dev[255];
    int cnt;
    for (cnt = 0; cnt < devices; ++cnt) {
        dev[device_ids[cnt]].addr = 0X1E;
        dev[device_ids[cnt]].length = 0x02;
        memcpy(dev[device_ids[cnt]].data,
               &pos_list[cnt],
               0x02);
    }
    write_multiple_device(0, 0, device_ids, dev, devices);
}

int set_dxl_torque(int broadcast, int port_id, uint8_t* device_ids, uint16_t* torque_list, int devices) {
    dxl_data dev[255];
    int cnt;
    for (cnt = 0; cnt < devices; ++cnt) {
        dev[device_ids[cnt]].addr = 0X22;
        dev[device_ids[cnt]].length = 0x02;
        memcpy(dev[device_ids[cnt]].data,
               &torque_list[cnt],
               0x02);
    }
    write_multiple_device(0, 0, device_ids, dev, devices);
}

int turn_dxl_torque(int broadcast, int port_id, uint8_t* device_ids, int state, int devices) {

    dxl_data dev[255];
    int cnt;
    for (cnt = 0; cnt < devices; ++cnt) {
        dev[device_ids[cnt]].addr = 0X18;
        dev[device_ids[cnt]].length = 0x01;
        dev[device_ids[cnt]].data[0] = state;
    }
    write_multiple_device(0, 0, device_ids, dev, devices);
}

int set_dxl_angle(int broadcast, int port_id, uint8_t* device_ids, float* pos_list, int devices) {

    if (!broadcast) {
        dxl_data dev[255];
        uint16_t pos;
        uint16_t bin_pos_list[255];
        int cnt;
        for (cnt = 0; cnt < devices; ++cnt) {
            bin_pos_list[cnt] = pos_list[cnt] * 4096 / 360;
            dev[device_ids[cnt]].addr = 0X1E;
            dev[device_ids[cnt]].length = 0x02;
            pos = pos_list[cnt] * 4096 / 360;
            memcpy(dev[device_ids[cnt]].data, &pos, 0x02);

        }
        write_multiple_device(0, port_id, device_ids, dev, devices);
    } else {

        uint16_t pos;
        uint16_t bin_pos_list[255];
        int cnt;
        for (cnt = 0; cnt < devices; ++cnt) {
            bin_pos_list[cnt] = pos_list[cnt] * 4096 / 360;
        }
        bulk_write(port_id, 0X1E, 0x02, device_ids, (uint8_t*) bin_pos_list, devices);
        //printf("port_id =%d data = %d\n", port_id, bin_pos_list[0]);
    }
}

int set_dxl_bin_angle(int broadcast, int port_id, uint8_t* device_ids, uint16_t* pos_list, int devices) {

    if (!broadcast) {
        dxl_data dev[255];
        uint16_t pos;
        uint16_t bin_pos_list[255];
        int cnt;
        for (cnt = 0; cnt < devices; ++cnt) {
            bin_pos_list[cnt] = pos_list[cnt] * 4096 / 360;
            dev[device_ids[cnt]].addr = 0X1E;
            dev[device_ids[cnt]].length = 0x02;
            pos = pos_list[cnt] * 4096 / 360;
            memcpy(dev[device_ids[cnt]].data, &pos, 0x02);

        }
        write_multiple_device(0, port_id, device_ids, dev, devices);
    } else {
        bulk_write(port_id, 0X1E, 0x02, device_ids, (uint8_t*) pos_list, devices);
        //printf("bin port_id =%d data = %d\n", port_id, pos_list[0]);
    }
}

int set_dxl_speed(int broadcast, int port_id, uint8_t* device_ids, uint16_t* speed_list, int devices) {
    dxl_data dev[255];
    int cnt;
    for (cnt = 0; cnt < devices; ++cnt) {
        dev[device_ids[cnt]].addr = 0X20;
        dev[device_ids[cnt]].length = 0x02;
        memcpy(dev[device_ids[cnt]].data,
               &speed_list[cnt],
               0x02);
    }
    if (!broadcast)
        write_multiple_device(0, port_id, device_ids, dev, devices);
    else
        bulk_write(port_id, 0X20, 0x02, device_ids, (uint8_t*) speed_list, devices);
}

int write_multiple_device(int broadcast, int port_id, uint8_t* device_ids, dxl_data* data, int devices) {
    if (!broadcast) {
        int packet_length;
        int answer_length = 6;
        int i;
        for (i = 0; i < devices; ++i) {
            init_packet(&packets[port_id]);
            packet_length = make_write(input_buffer[port_id], 255, device_ids[i], data[device_ids[i]].addr,
                                       data[device_ids[i]].data, data[device_ids[i]].length);
            write_read_port(&controller,
                            port_id,
                            input_buffer[port_id],
                            packet_length,
                            input_buffer[port_id],
                            answer_length,
                            single_status);
        }
    } else {
        int packet_length;
        int answer_length;
        init_packet(&packets[port_id]);
        //packet_length = make_same_addr_write(input_buffer[port_id], 255, device_ids, 0, 2, devices, &answer_length);
        write_read_port(&controller,
                        port_id,
                        input_buffer[port_id],
                        packet_length,
                        input_buffer[port_id],
                        answer_length,
                        multiple_read);

    }
    return PORT_ERROR;
}

int scan(int port_id, uint8_t** device_ids) {
    uint8_t i = 1;
    uint8_t found = 0;
    for (; i < 255; i++) {
        found = ping_dxl_device(port_id, i);
        printf("Dev id=%d is %d", i, found ? "founded" : "not founded");
    }
}
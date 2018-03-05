#pragma once

#include "packet_handler.h"
#include "port_controller.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dxl_data {
    uint8_t addr;
    uint8_t length;
    uint8_t data[255];
} dxl_data;

int register_port(const char* dev_name, int baudrate);

int init_dxl_controller();

int finalize_dxl_controller();

int add_dxl_device(int port_id, int id);

int read_dxl_device(int port_id, uint8_t* device_ids, int devices);

int write_multiple_device(int broadcast, int port_id, uint8_t* device_ids, dxl_data* data, int devices);

int set_dxl_position(int broadcast, int port_id, uint8_t* device_ids, uint16_t* pos_list, int devices);

int set_dxl_speed(int broadcast, int port_id, uint8_t* device_ids, uint16_t* speed_list, int devices);

int set_dxl_angle(int broadcast, int port_id, uint8_t* device_ids, float* pos_list, int devices);

int set_dxl_bin_angle(int broadcast, int port_id, uint8_t* device_ids, uint16_t* pos_list, int devices);

int turn_dxl_torque(int broadcast, int port_id, uint8_t* device_ids, int state, int devices);

int set_dxl_torque(int broadcast, int port_id, uint8_t* device_ids, uint16_t* torque_list, int devices);

#ifdef __cplusplus
}
#endif
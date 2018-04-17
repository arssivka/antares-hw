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

/// \file   packet_handler.h
/// \author Arseniy Ivin
/// \date   2018/04/13
///
/// \brief   Functions and constants for manipulating packages by using Dynamixel protocol.
/// \details This file contains functions for creating requests to Dynamixel
///          devices and parsing answers from it. It uses the Dynamixel v1 protocol. This
///          functions only work with buffers and can't read and write to port (\see port_handler.h)

#pragma once

#include <stdint.h>

#define DXL_MAX_DEVICE_ID                 (0xFD) /**< Max available device ID */
#define DXL_FAIL                          (0xFF) /**< Function call error code */

#define DXL_HEADER                        (0xFF) /**< First two bytes of every package is 0xFF */
#define DXL_PACKAGE_HEADER_LENGTH         (0x04) /**< HEADER_1, HEADER_2, ID, LENGTH */
#define DXL_DATA_DESKRIPTION_LENGTH       (0x02) /**< INSTRUCTION, checksum */
#define DXL_STATUS_PACKET_LENGTH          (0x06) //TODO
#define DXL_BROADCAST_ID                  (0xFE) /**< Device ID for broadcasting package */

#define DXL_POS_REQ_HEADER_1              (0x00) /**< Required by protocol (always 0xFF) */
#define DXL_POS_REQ_HEADER_2              (0x01) /**< Required by protocol (always 0xFF) */
#define DXL_POS_REQ_ID                    (0x02) /**< Device ID (or broadcast) */
#define DXL_POS_REQ_LENGTH                (0x03) /**< Data length */
#define DXL_POS_REQ_CMD                   (0x04) /**< Instruction byte (ping, reset, read, write, bulk read, bulk write) */
#define DXL_POS_REQ_PARAMS                (0x05) /**< First byte of data */

#define DXL_POS_ANS_HEADER_1              (0x00) /**< Required by protocol (always 0xFF) */
#define DXL_POS_ANS_HEADER_2              (0x01) /**< Required by protocol (always 0xFF) */
#define DXL_POS_ANS_ID                    (0x02) /**< Device ID */
#define DXL_POS_ANS_LENGTH                (0x03) /**< Data length */
#define DXL_POS_ANS_ERROR                 (0x04) /**< Error mask (see error codes below) */
#define DXL_POS_ANS_DATA                  (0x05) /**< First byte of data */

#define DXL_ANS_MASK_INPUT_VOLTAGE_ERROR  (0x01) /**< Applied voltage is out of the range of operating voltage */
#define DXL_ANS_MASK_ANGLE_LIMIT_ERROR    (0x02) /**< Goal position is out of the range from angle limits */
#define DXL_ANS_MASK_OVERHEATING_ERROR    (0x04) /**< Internal temperature of Dynamixel is out of the range operating temperature */
#define DXL_ANS_MASK_RANGE_ERROR          (0x08) /**< Command is out of the range for use */
#define DXL_ANS_MASK_CHECKSUM_ERROR       (0x10) /**< Checksum incorrect */
#define DXL_ANS_MASK_OVERLOAD_ERROR       (0x20) /**< Current load can't be controlled by the torque */
#define DXL_ANS_MASK_INSTRUCTION_ERROR    (0x40) /**< Undefined instruction or delivering the action command without the reg_write command */

#define DXL_CMD_PING                      (0x01) /**< Ping device */
#define DXL_CMD_READ                      (0x02) /**< Read data from one device */
#define DXL_CMD_WRITE                     (0x03) /**< Write data to one device */
#define DXL_CMD_BULK_WRITE                (0x83) /**< One write request to several deices */
#define DXL_CMD_BULK_READ                 (0x92) /**< One read request to several devices */
#define DXL_CMD_RESET                     (0x06) /**< Read all parameters from default table */

#define DXL_CHECKSUM_BYTE                 (0x01) /**< Only for code readability */
#define DXL_PARAM_BYTE                    (0x01) /**< Only for code readability */

//////////////////////////////////////////////////
//               CONTROL TABLE                  //
//////////////////////////////////////////////////
// EEPROM
#define DXL_CT_MODEL_NUMBER_L =             (0x00)
#define DXL_CT_MODEL_NUMBER_H =             (0x01)
#define DXL_CT_FIRMWARE_VERSION =           (0x02)
#define DXL_CT_ID =                         (0x03)
#define DXL_CT_BAUD_RATE =                  (0x04)
#define DXL_CT_RETURN_DELAY_TIME =          (0x05)
#define DXL_CT_CW_ANGLE_LIMIT_L =           (0x06)
#define DXL_CT_CW_ANGLE_LIMIT_H =           (0x07)
#define DXL_CT_CCW_ANGLE_LIMIT_L =          (0x08)
#define DXL_CT_CCW_ANGLE_LIMIT_H =          (0x09)
#define DXL_CT_HIGHEST_LIMIT_TEMPERATURE =  (0x0B)
#define DXL_CT_LOWEST_LIMIT_VOLTAGE =       (0x0C)
#define DXL_CT_HIGHEST_LIMIT_VOLTAGE =      (0x0D)
#define DXL_CT_MAX_TORQUE_L =               (0x0E)
#define DXL_CT_MAX_TORQUE_H =               (0x0F)
#define DXL_CT_STATUS_RETURN_LEVEL =        (0x10)
#define DXL_CT_ALARM_LED =                  (0x11)
#define DXL_CT_ALARM_SHUTDOWN =             (0x12)
#define DXL_CT_MULTI_TURN_OFFSET_L =        (0x14)
#define DXL_CT_MULTI_TURN_OFFSET_H =        (0x15)
#define DXL_CT_RESOLUTION_DIVIDER =         (0x16)
// RAM
#define DXL_CT_TORQUE_ENABLE =              (0x18)
#define DXL_CT_LED =                        (0x19)
#define DXL_CT_D_GAIN =                     (0x1A)
#define DXL_CT_I_GAIN =                     (0x1B)
#define DXL_CT_P_GAIN =                     (0x1C)
#define DXL_CT_GOAL_POSITION_L =            (0x1E)
#define DXL_CT_GOAL_POSITION_H =            (0x1F)
#define DXL_CT_MOVING_SPEED_L =             (0x20)
#define DXL_CT_MOVING_SPEED_H =             (0x21)
#define DXL_CT_TORQUE_LIMIT_L =             (0x22)
#define DXL_CT_TORQUE_LIMIT_H =             (0x23)
#define DXL_CT_PRESENT_POSITION_L =         (0x24)
#define DXL_CT_PRESENT_POSITION_H =         (0x25)
#define DXL_CT_PRESENT_SPEED_L =            (0x26)
#define DXL_CT_PRESENT_SPEED_H =            (0x27)
#define DXL_CT_PRESENT_LOAD_L =             (0x28)
#define DXL_CT_PRESENT_LOAD_H =             (0x29)
#define DXL_CT_PRESENT_VOLTAGE =            (0x2A)
#define DXL_CT_PRESENT_TEMPERATURE =        (0x2B)
#define DXL_CT_REGISTERED =                 (0x2C)
#define DXL_CT_MOVING =                     (0x2E)
#define DXL_CT_LOCK =                       (0x2F)
#define DXL_CT_PUNCH_L =                    (0x30)
#define DXL_CT_PUNCH_H =                    (0x31)
#define DXL_CT_CURRENT_L =                  (0x44)
#define DXL_CT_CURRENT_H =                  (0x45)
#define DXL_CT_TORQUE_CONTROL_MODE_ENABLE = (0x46)
#define DXL_CT_GOAL_TORQUE_L =              (0x47)
#define DXL_CT_GOAL_TORQUE_H =              (0x48)
#define DXL_CT_GOAL_ACCELERATION =          (0x49)

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Calculate sum of all bytes and invert them
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Checksum if success or DXL_FAIL if length check error
uint8_t calculate_checksum(uint8_t* buff, int buff_len);


/// \brief Write checksum to last byte of buffer.
///        You must call it after changes of buffer before sending!
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Package length if success or DXL_FAIL if length check error
uint8_t packet_add_checksum(uint8_t* buff, int buff_len);


/// \brief Create ping request package
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_make_ping(uint8_t* buff, int buff_len, uint8_t dev_id);


/// \brief Create resetting request package
/// \param buff Package buffer
/// \param buff_len Length of buffer
/// \param dev_id Requested device ID
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_make_reset(uint8_t* buff, int buff_len, uint8_t dev_id);


///////////////////////////////////////////////////////////
//               Single write functions                  //
///////////////////////////////////////////////////////////

/// \brief Create single write request package
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data Data buffer
/// \param data_len Length of data buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_make_write(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t* data,
                          uint8_t data_len);


/// \brief Change data in single write request package. It must be initialized before (\see packet_make_write)
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data Data buffer
/// \param data_len Length of data buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_update_write(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t* data,
                            uint8_t data_len);


///////////////////////////////////////////////////////////
//                Single read functions                  //
///////////////////////////////////////////////////////////

/// \brief Create single read request package
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data_len Length of requested data
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_make_read(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t data_len);


/// \brief Change data in single read request package. It must be initialized before (\see packet_make_read)
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data_len Length of requested data
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_update_read(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t data_len);


// TODO Implementation and documentation
uint8_t packet_calculate_read_answer_len(const uint8_t* buff, int buff_len);


///////////////////////////////////////////////////////////
//                  Bulk read functions                  //
///////////////////////////////////////////////////////////

/// \brief Create a multiple read request package. It isn't contain device IDs initially
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_init_bulk_read(uint8_t* buff, int buff_len);


/// \brief Add a device ID, a control table address and a data length to the end of multiple read request package.
///        Request must be initialized before (\see packet_init_bulk_read).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data_len Length of requested data from one device
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_add_bulk_read_data(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t data_len);


/// \brief Resize a multiple read request package. It will be able to hold param_count requests.
///        You can set or change data by using update function (\see packet_update_bulk_read_data).
///        Request must be initialized before (\see packet_init_bulk_read).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param param_count Num of requested devices
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_resize_bulk_read_data(uint8_t* buff, int buff_len, int param_count);


/// \brief Set or change data in a multiple read request package.
///        It don't resize the package (\see packet_resize_bulk_read_data).
///        Request must be initialized before (\see packet_init_bulk_read).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param index Data block index in package
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data_len Length of requested data from one device
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_update_bulk_read_data(uint8_t* buff, int buff_len, uint8_t index, uint8_t dev_id, uint8_t ctrl_tab_addr,
                                     uint8_t data_len);


/// \brief Resize multiple read request package to initial state.
///        Request must be initialized before (\see packet_init_bulk_read).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_clean_bulk_read_data(uint8_t* buff, int buff_len);


/// \brief Calculate answer size for multiple read request package.
///        Request must be initialized before (\see packet_init_bulk_read).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_calculate_bulk_read_answer_len(const uint8_t* buff, int buff_len);


///////////////////////////////////////////////////////////
//                   Bulk write functions                //
///////////////////////////////////////////////////////////

/// \brief Create a multiple write request package. It isn't contain device IDs initially
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param ctrl_tab_addr Parameter address byte (see DLX_CT_* macros)
/// \param data_len Length of requested data from one device
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_init_bulk_write(uint8_t* buff, int buff_len, uint8_t ctrl_tab_addr, uint8_t data_len);


/// \brief Add a device ID and data to the end of multiple write request package.
///        Length of data must be same size with initial data_len (\see packet_init_bulk_write).
///        Request must be initialized before (\see packet_init_bulk_write).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param dev_id Requested device ID
/// \param data Data buffer
/// \param data_len Length of data buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_add_bulk_write_data(uint8_t* buff, int buff_len, uint8_t dev_id, const uint8_t* data,
                                   uint8_t data_len);


/// \brief Resize multiple write request package to initial state.
///        You can set or change data by using update function (\see packet_update_bulk_write_data).
///        Request must be initialized before (\see packet_init_bulk_write).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param param_count Num of requested devices
/// \param data_len Single request data length
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_resize_bulk_write_data(uint8_t* buff, int buff_len, int param_count, int data_len);


/// \brief Set or change data in a multiple write request package.
///        It don't resize the package (\see packet_resize_bulk_write_data).
///        Request must be initialized before (\see packet_init_bulk_read).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \param index Data block index in package
/// \param dev_id Requested device ID
/// \param data Data buffer
/// \param data_len Length of data buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_update_bulk_write_data(uint8_t* buff, int buff_len, uint8_t index, uint8_t dev_id,
                                      const uint8_t* data, uint8_t data_len);


/// \brief Resize multiple write request package to initial state.
///        Request must be initialized before (\see packet_init_bulk_write).
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \returns Package length if success or DXL_FAIL if length check error
uint8_t packet_clean_bulk_write_data(uint8_t* buff, int buff_len);


///////////////////////////////////////////////////////////
//                 Answer parsing functions              //
///////////////////////////////////////////////////////////

/// \brief Check first two bytes of buffer
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return true (1) if first two bytes is 0xFF else false (0). DXL_FAIL if length check error
uint8_t packet_answer_check_header(const uint8_t* buff, int buff_len);


/// \brief Get device ID from answer buffer
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Device ID if success or DXL_FAIL if length check error
uint8_t packet_answer_get_device_id(const uint8_t* buff, int buff_len);


/// \brief Get data length from answer buffer
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Data length if success or DXL_FAIL if length check error
uint8_t packet_answer_get_data_length(const uint8_t* buff, int buff_len);


/// \brief Get error byte from answer buffer (see DLX_ANS_MASK_* macros)
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Error byte if success or DXL_FAIL if length check error
uint8_t packet_answer_get_error(const uint8_t* buff, int buff_len);


/// \brief Get pointer to first data byte
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Data begin pointer if success or NULL if length check error
const uint8_t* packet_answer_get_data_begin(const uint8_t* buff, int buff_len);


/// \brief Get pointer to the begining of next package in buffer
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Next package start if success or NULL if length check error
const uint8_t* packet_answer_get_next_packet(const uint8_t* buff, int buff_len);


/// \brief Read checksum from buffer
/// \param buff Package buffer
/// \param buff_len Length of package buffer
/// \return Checksum if success or NULL if length check error
uint8_t packet_answer_get_checksum(const uint8_t* buff, int buff_len);


#ifdef __cplusplus
}
#endif

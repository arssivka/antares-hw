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

/// \file   port_handler.h
/// \author Arseniy Ivin
/// \date   2018/04/17
///
/// \brief   Functions for handling devices.
/// \details This file contains functions for open, close and communicate with devices
///          They use OS specific calls internally.
///          There are no functions for handling packages (\see packet_handler.h)

#pragma once

#include <stdint.h>
#include "port_handler_linux.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Open device by name
/// \param dev_name Path to device
/// \return Port handler
port_handle_t port_open(const char* dev_name);


/// \brief Close device
/// \param ph Port handler
void port_close(port_handle_t ph);


/// \brief Set system baudrate for device
/// \param ph Port handler
/// \param baudrate Baudrate value
/// \return True if success otherwise false
int port_set_baudrate(port_handle_t ph, int baudrate);


/// \brief Write data from buffer to port
/// \param ph Port handler
/// \param buff Data buffer
/// \param buff_len Buffer length (data size)
/// \return Length that have been written
int port_write(port_handle_t ph, const uint8_t* buff, int buff_len);


/// \brief Read data from port to buffer
/// \param ph Port handler
/// \param buff Data buffer
/// \param buff_len Buffer max length
/// \return Length that have been read
int port_read(port_handle_t ph, uint8_t* buff, int buff_len);


/// \brief Read from single port handler which is ready to read (it faster than single reads from multiple ports)
/// \param ph_count Count of port handlers
/// \param phs Array of port handlers
/// \param buff Data buffer
/// \param buff_len Buffer max length
/// \return Length that have been read
int port_multiple_read(int ph_count, port_handle_t* phs, uint8_t* buff, int buff_len);

#ifdef __cplusplus
}
#endif

// Copyright 2018 Arseniy Ivin <arssivka@yandex.ru>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

typedef int port_handle_t;

#define PORT_ERROR   (0)
#define PORT_SUCCESS (1)

#ifdef __cplusplus
extern "C" {
#endif

port_handle_t port_open_linux(const char* dev_name);

int port_set_baudrate_linux(port_handle_t ph, int baudrate);

int port_write_linux(port_handle_t ph, const char* data, int size);

int port_read_linux(port_handle_t ph, char* data, int size);

void port_close_linux(port_handle_t ph);

#ifdef __cplusplus
}
#endif

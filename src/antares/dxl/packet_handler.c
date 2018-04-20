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

/// \file   packet_handler.c
/// \author Arseniy Ivin
/// \date   2018/04/13

#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include "packet_handler.h"


uint8_t calculate_checksum(uint8_t* buff, int buff_len) {
    // Check for buffer length
    if (buff_len <= DXL_POS_REQ_ID) {
        return DXL_FAIL;
    }

    int length = buff[DXL_POS_REQ_LENGTH] + DXL_CHECKSUM_BYTE;
    uint8_t* begin = &buff[DXL_POS_REQ_ID];
    uint8_t* end =   &buff[DXL_POS_REQ_ID] + length;
    uint8_t* it =    NULL;
    uint8_t  res =   0;
    for (it = begin; it < end; ++it) {
        res += *it;
    }
    return ~res;
}



uint8_t packet_add_checksum(uint8_t* buff, int buff_len) {
    // Check for buffer length
    if (buff_len <= DXL_POS_REQ_LENGTH || buff_len <= buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }
    int checksum_pos = DXL_POS_REQ_PARAMS + buff[DXL_POS_REQ_LENGTH] - DXL_CMD_BYTE - DXL_CHECKSUM_BYTE;
    buff[checksum_pos] = calculate_checksum(buff, buff_len);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_make_ping(uint8_t* buff, int buff_len, uint8_t dev_id) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_HEADER_1] = (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_HEADER_2] = (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_ID] =       (uint8_t) dev_id;
    buff[DXL_POS_REQ_CMD] =      (uint8_t) DXL_CMD_PING;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] =   (uint8_t) DXL_DATA_DESCRIPTION_LENGTH;
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_make_reset(uint8_t* buff, int buff_len, uint8_t dev_id) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_HEADER_1] = (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_HEADER_2] = (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_ID] =       (uint8_t) dev_id;
    buff[DXL_POS_REQ_CMD] =      (uint8_t) DXL_CMD_RESET;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] =   (uint8_t) DXL_DATA_DESCRIPTION_LENGTH;
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_make_write(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, const uint8_t* data,
                          uint8_t data_len) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH + DXL_PARAM_BYTE + data_len) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_HEADER_1] = (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_HEADER_2] = (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_ID] =       (uint8_t) dev_id;
    buff[DXL_POS_REQ_CMD] =      (uint8_t) DXL_CMD_WRITE;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] =   (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + DXL_PARAM_BYTE + data_len);
    buff[DXL_POS_REQ_PARAMS] =   (uint8_t) ctrl_tab_addr;
    memcpy(&buff[DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE], data, data_len);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_update_write(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, const uint8_t* data,
                            uint8_t data_len) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH + 0x01 + data_len) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_ID] =     (uint8_t) dev_id;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + DXL_PARAM_BYTE + data_len);
    buff[DXL_POS_REQ_PARAMS] = (uint8_t) ctrl_tab_addr;
    memcpy(&buff[DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE], data, data_len);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_make_read(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t data_len) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH + 2 * DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_HEADER_1] =                (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_HEADER_2] =                (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_ID] =                      (uint8_t) dev_id;
    buff[DXL_POS_REQ_CMD] =                     (uint8_t) DXL_CMD_READ;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] =                  (uint8_t) DXL_DATA_DESCRIPTION_LENGTH + 2 * DXL_PARAM_BYTE;
    buff[DXL_POS_REQ_PARAMS] =                  (uint8_t) ctrl_tab_addr;
    buff[DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE] = (uint8_t) data_len;
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_update_read(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t data_len) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH + 2 * DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_ID] =                      (uint8_t) dev_id;
    // Prepare package data
    buff[DXL_POS_REQ_PARAMS] =                  (uint8_t) ctrl_tab_addr;
    buff[DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE] = (uint8_t) data_len;
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_calculate_read_answer_len(const uint8_t* buff, int buff_len) {
    if (buff_len <= DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }

    int answer_len;
    int data_len;
    data_len = buff[DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE];
    answer_len = data_len + DXL_STATUS_PACKET_LENGTH;
    return (uint8_t) answer_len;
}


uint8_t packet_init_bulk_read(uint8_t* buff, int buff_len) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH + DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_HEADER_1] =      (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_HEADER_2] =      (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_ID] =            (uint8_t) DXL_BROADCAST_ID;
    buff[DXL_POS_REQ_CMD] =           (uint8_t) DXL_CMD_BULK_READ;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] =        (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + DXL_PARAM_BYTE);
    buff[DXL_POS_REQ_PARAMS] =        (uint8_t) 0x00; // Always zero
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_add_bulk_read_data(uint8_t* buff, int buff_len, uint8_t dev_id, uint8_t ctrl_tab_addr, uint8_t data_len) {
    // Buffer length check
    if (buff_len <= DXL_POS_REQ_LENGTH || buff_len <= buff[DXL_POS_REQ_LENGTH] - DXL_CHECKSUM_BYTE + 3 * DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }
    // Get last parameter position
    int pos = buff[DXL_POS_REQ_LENGTH] - DXL_CHECKSUM_BYTE;
    // Set data
    buff[pos++] = data_len;
    buff[pos++] = dev_id;
    buff[pos++] = ctrl_tab_addr;
    // Update length
    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (pos + DXL_CHECKSUM_BYTE);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_resize_bulk_read_data(uint8_t* buff, int buff_len, int param_count) {
    // Buffer length check
    if (buff_len <= DXL_POS_REQ_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }
    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + (1 + 3 * param_count) * DXL_PARAM_BYTE);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_update_bulk_read_data(uint8_t* buff, int buff_len, uint8_t index, uint8_t dev_id, uint8_t ctrl_tab_addr,
                                     uint8_t data_len) {
    // Parameter position
    int pos = DXL_DATA_DESCRIPTION_LENGTH + (1 +  3 * index) * DXL_PARAM_BYTE;
    // Buffer length check
    if (buff_len <= pos + 3) {
        return (uint8_t) DXL_FAIL;
    }
    // Update data
    buff[pos++] = data_len;
    buff[pos++] = dev_id;
    buff[pos++] = ctrl_tab_addr;
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_clean_bulk_read_data(uint8_t* buff, int buff_len) {
    if (buff_len <= DXL_POS_REQ_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }
    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + DXL_PARAM_BYTE);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_calculate_bulk_read_answer_len(const uint8_t* buff, int buff_len) {
    if (buff_len <= DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }

    int answer_len;
    int data_len;
    int dev_id_count;

    dev_id_count = (buff[DXL_POS_REQ_LENGTH] - DXL_DATA_DESCRIPTION_LENGTH - DXL_PARAM_BYTE) / (3 * DXL_PARAM_BYTE);
    answer_len = 0;
    if (dev_id_count > 0) {
        data_len = buff[DXL_POS_REQ_PARAMS + DXL_PARAM_BYTE];
        answer_len = (data_len + DXL_STATUS_PACKET_LENGTH) * dev_id_count;
    }
    return (uint8_t) answer_len;
}


uint8_t packet_init_bulk_write(uint8_t* buff, int buff_len, uint8_t ctrl_tab_addr, uint8_t data_len) {
    // Check for buffer length
    if (buff_len <= DXL_PACKAGE_HEADER_LENGTH + DXL_DATA_DESCRIPTION_LENGTH + 2 * DXL_PARAM_BYTE) {
        return (uint8_t) DXL_FAIL;
    }
    // Prepare package header
    buff[DXL_POS_REQ_HEADER_1] =      (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_HEADER_2] =      (uint8_t) DXL_HEADER;
    buff[DXL_POS_REQ_ID] =            (uint8_t) DXL_BROADCAST_ID;
    buff[DXL_POS_REQ_CMD] =           (uint8_t) DXL_CMD_BULK_WRITE;
    // Prepare package data
    buff[DXL_POS_REQ_LENGTH] =        (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + 2 * DXL_PARAM_BYTE);
    buff[DXL_POS_REQ_PARAMS] =        (uint8_t) ctrl_tab_addr;
    buff[DXL_POS_REQ_PARAMS + 1] = data_len;
    buff[DXL_POS_REQ_PARAMS + buff[DXL_POS_REQ_LENGTH] - 2] = calculate_checksum(buff, 0);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_add_bulk_write_data(uint8_t* buff, int buff_len, uint8_t dev_id, const uint8_t* data,
                                   uint8_t data_len) {
    // Check for buffer length
    if (buff_len <= DXL_POS_REQ_PARAMS + 1 ||
            buff_len <= buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH ||
            buff[DXL_POS_REQ_PARAMS + 1] != data_len) {
        return (uint8_t) DXL_FAIL;
    }
    // Get last parameter position
    int pos = buff[DXL_POS_REQ_LENGTH] - DXL_CHECKSUM_BYTE;
    // Set data
    buff[pos++] = dev_id;
    memcpy(&buff[pos], data, data_len);
    pos += data_len;
    // Update length
    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (pos + DXL_CHECKSUM_BYTE);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_resize_bulk_write_data(uint8_t* buff, int buff_len, int param_count, int data_len) {
    // Check for buffer length
    if (data_len <= DXL_POS_REQ_PARAMS + 1 ||
            data_len <= buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH ||
            buff[DXL_POS_REQ_PARAMS + 1] != data_len) {
        return (uint8_t) DXL_FAIL;
    }
    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + (2 + (data_len + 1) * param_count) * DXL_PARAM_BYTE);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_update_bulk_write_data(uint8_t* buff, int buff_len, uint8_t index, uint8_t dev_id,
                                      const uint8_t* data,
                                      uint8_t data_len) {
    // Length check
    if (data_len <= DXL_POS_REQ_PARAMS + 1 ||
            data_len <= buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH ||
            buff[DXL_POS_REQ_PARAMS + 1] != data_len) {
        return (uint8_t) DXL_FAIL;
    }
    // Parameter position
    int pos = DXL_DATA_DESCRIPTION_LENGTH + (2 + (data_len + 1) * index) * DXL_PARAM_BYTE;
    // Update data
    buff[pos++] = dev_id;
    memcpy(&buff[pos], data, data_len);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_clean_bulk_write_data(uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }

    buff[DXL_POS_REQ_LENGTH] = (uint8_t) (DXL_DATA_DESCRIPTION_LENGTH + 2 * DXL_PARAM_BYTE);
    return (uint8_t) (buff[DXL_POS_REQ_LENGTH] + DXL_PACKAGE_HEADER_LENGTH);
}


uint8_t packet_answer_check_header(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_HEADER_2) {
        return (uint8_t) DXL_FAIL;
    }
    return (uint8_t) (buff[DXL_POS_ANS_HEADER_1] == DXL_HEADER &&
                      buff[DXL_POS_ANS_HEADER_2] == DXL_HEADER);
}


uint8_t packet_answer_get_device_id(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_ID) {
        return (uint8_t) DXL_FAIL;
    }

    return (uint8_t) (buff[DXL_POS_ANS_ID]);
}


uint8_t packet_answer_get_data_length(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_LENGTH) {
        return (uint8_t) DXL_FAIL;
    }
    return (uint8_t) (buff[DXL_POS_ANS_LENGTH] - DXL_ERROR_BYTE - DXL_CHECKSUM_BYTE);
}


uint8_t packet_answer_get_error(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_ERROR) {
        return (uint8_t) DXL_FAIL;
    }

    return (uint8_t) (buff[DXL_POS_ANS_ERROR]);
}


uint8_t packet_answer_get_checksum(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_LENGTH || buff_len <= DXL_POS_ANS_DATA + buff[DXL_POS_ANS_LENGTH]) {
        return (uint8_t) DXL_FAIL;
    }

    int len = buff[DXL_POS_ANS_LENGTH];
    return (uint8_t) (DXL_POS_ANS_DATA + len);
}


const uint8_t* packet_answer_get_data_begin(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_DATA) {
        return NULL;
    }
    return &buff[DXL_POS_ANS_DATA];
}


const uint8_t* packet_answer_get_next_packet(const uint8_t* buff, int buff_len) {
    // Length check
    if (buff_len <= DXL_POS_ANS_LENGTH || buff_len <= DXL_POS_ANS_DATA + buff[DXL_POS_ANS_LENGTH] + 1) {
        return NULL;
    }

    int len = buff[DXL_POS_ANS_LENGTH];
    return &buff[DXL_POS_ANS_DATA + len + 1];
}

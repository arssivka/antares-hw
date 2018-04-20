# Antares HW

This repo contains set of C functions for manipulate
Antares hardware. Antares is a anthropomorphic robot developed
at [SPIIRAS](http://www.spiiras.nw.ru/). Robot hardware used
[Dynamixel v1](http://support.robotis.com/en/product/actuator/dynamixel/dxl_communication.htm)
protocol for communicating with sensors and actuators. This
function simplify constructing of packets for Dynamixel
protocol and provide wrapper for communicating with OS and devices.

## Sources tree structure

 - *port_handler* - function for communicating with OS and
   devices
 - *port_handler_linux* - implementation of functions in
   *port_handler* for Linux operation system.
   Implementation is separated for future purposes
 - *packet_handler* - function for system independed
   manipulating of packets for communicating  with
   Dynamixel devices

## Communicate with devices

Open and close port. You should use the *port_handle_t* type for
port handler. We support only Linux systems at nowadays but we
want to impement this functions for Windows in the future.

```cpp
port_handle_t ph = port_open("/dev/ttyUSB0");
if (ph == PORT_ERROR) {
    // Handle the error
    // ...
} else {
    // Your code
    // ...
    port_close(ph);
}
```

Write to single port. If you want to write at multiple
ports you can use loop. Some write packages returns data
and you sould read it

```cpp
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];

// Prepare write request
// ...

port_write(ph, buff, packet_length); // Your data may be shorter
                                     // than buffer length
```

Read from single port. For reading from multimple ports
you should use another method.

```cpp
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int responce_len = 0;

// Prepare read request
// ...

responce_len = port_read(ph, buff, packet_length);
// Your data may be shorter than buffer length
```

Read from multiple ports. This method reads from one available port.

```cpp
const char* dev[] = {"/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2"}; // Some devices
port_handle_t ph[3] = {};
const int ph_count = NUM_OF_PORTS; // In this case it is 3
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int responce_len = 0;
int ph_index = -1;

// Open ports
for (int i = 0; i < ph_count; ++i) {
    ph = port_open(dev[i]);
    // Error check
    // ...
}

// Multiple write
for (int i = 0; i < ph_count; ++i) {
    // Prepare request
    // ...
    port_write(ph[i], buff, packet_length);
}

// Multiple read
for (int i = 0; i < ph_count; ++i) {
    responce_len = port_multiple_read(ph_count, ph, buff, DXL_MAX_PACKAGE_LENGTH, &ph_index);
    // Work with answer
    // ...
}

// Close ports
for (int i = 0; i < ph_count; ++i) {
    port_close(ph[i]);
}
```

## Creating packets

Examples below provide explanation not for all functions and cases! See
documentation in header files!

*Warning! If error occurred all methods can return DXL_FAIL!
Be carefully with buffer length!*

### Ping device

```cpp
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int dev_id = 16;
int len = 0;
int err = 0;

packet_make_ping(buff, DXL_MAX_PACKAGE_LENGTH, dev_id);
len = packet_add_checksum(buff, DXL_MAX_PACKAGE_LENGTH); // Don't forget it!

if (len != DXL_FAIL) {
    port_write(ph, buff, len);

    len = port_read(ph, buff, DXL_MAX_PACKAGE_LENGTH);
    if (len > 0) {
        err = packet_answer_get_error(buff, len);
        if (err == 0) {
            // Success!
        } else {
            // See DXL_ANS_MASK_*
        }
    }
}
```

### Reset device

```cpp
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int dev_id = SOME_DEVICE_ID;
int len = 0;
int err = 0;

packet_make_reset(buff, DXL_MAX_PACKAGE_LENGTH, dev_id);
len = packet_add_checksum(buff, DXL_MAX_PACKAGE_LENGTH); // Don't forget it!

if (len != DXL_FAIL) {
    port_write(ph, buff, DXL_MAX_PACKAGE_LENGTH);

    len = port_read(ph, buff, DXL_MAX_PACKAGE_LENGTH);
    if (len > 0) {
        err = packet_answer_get_error(buff, len);
        if (err == 0) {
            // Success!
        } else {
            // See DXL_ANS_MASK_*
        }
    }
}
```

### Single write

Write position to single servo. Don't use DXL_BROADCAST_ID
as device ID (see DXL_MAX_DEVICE_ID). Returns package without data
(see error byte)

```cpp
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int dev_id = SOME_DEVICE_ID;
int len = 0;
int err = 0;
uint16_t val = 30.0f / 360.0f  * 4096; // 30 degrees to bin val

packet_make_write(buff, DXL_MAX_PACKAGE_LENGTH, dev_id, DXL_CT_GOAL_POSITION_L, &val, sizeof(val));
len = packet_add_checksum(buff, DXL_MAX_PACKAGE_LENGTH); // Don't forget it!

if (len != DXL_FAIL) {
    port_write(ph, buff, DXL_MAX_PACKAGE_LENGTH);

    len = port_read(ph, buff, DXL_MAX_PACKAGE_LENGTH);
    if (len > 0) {
        err = packet_answer_get_error(buff, len);
        if (err == 0) {
            // Success!
        } else {
            // See DXL_ANS_MASK_*
        }
    }
}
```

### Single read

Read position from single servo. Don't use DXL_BROADCAST_ID as device ID (see DXL_MAX_DEVICE_ID).

```cpp
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int dev_id = SOME_DEVICE_ID;
int len = 0;
int err = 0;

len = packet_make_read(buff, DXL_MAX_PACKAGE_LENGTH, dev_id, DXL_CT_GOAL_POSITION_L, sizeof(uint16_t));
packet_add_checksum(buff, DXL_MAX_PACKAGE_LENGTH); // Don't forget it!

if (len != DXL_FAIL) {
    port_write(ph, buff, len);

    len = port_read(ph, buff, DXL_MAX_PACKAGE_LENGTH);
    if (len > 0) {
        err = packet_answer_get_error(buff, len);
        if (err == 0) {
            // Success!
        } else {
            // See DXL_ANS_MASK_*
        }
    }
}
```

### Bulk read

Ask multiple devices from one port (bulk read). This appoach allows
to make single request for multiple devices. It faster than multiple
reads.

```cpp
const uint8_t dev[] = {0, 1, 2, 3, 4, 5, 6, 7} // Some device IDs
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int len = 0;
int err = 0;

packet_init_bulk_read(buff, DXL_MAX_PACKAGE_LENGTH);

for (int i = 0; i < NUM_OF_DEVICES) {
    packet_add_bulk_read_data(buff, buff_len, dev[i], DXL_CT_GOAL_POSITION_L, sizeof(uint8_t);
}

len = packet_add_checksum(buff, DXL_MAX_PACKAGE_LENGTH); // Don't forget it!

if (len != DXL_FAIL) {
    port_write(ph, buff, len);

    len = port_read(ph, buff, DXL_MAX_PACKAGE_LENGTH);
    if (len > 0) {
        err = packet_answer_get_error(buff, len);
        if (err == 0) {
            // Success!
        } else {
            // See DXL_ANS_MASK_*
        }
    }
}
```

Also you can resize buffer and set data directly

```cpp
const uint8_t dev[] = {0, 1, 2, 3, 4, 5, 6, 7} // Some device IDs
uint8_t buff[DXL_MAX_PACKAGE_LENGTH];
int dev_id = SOME_DEVICE_ID; // In this case it is 8
int len = 0;
int err = 0;

packet_init_bulk_read(buff, DXL_MAX_PACKAGE_LENGTH);
packet_resize_bulk_read_data(buff, DXL_MAX_PACKAGE_LENGTH, 8)

for (int i = 0; i < NUM_OF_DEVICES) {
    packet_update_bulk_read_data(buff, buff_len, i, dev[i], DXL_CT_GOAL_POSITION_L, sizeof(uint8_t);
}

// ...
```

In bulk read request you may set different control table addresses but
in bulk write you can't do it.

### Bulk write

Bulk write request is made in a similar
manner (see the documentation in the comments).

*Attention! Bulk write request doesn't return anything!*

## Parsing answer

In this examples

```cpp
// buff contains answer
// len is length of answer
```

### Validate packet

```cpp
uint8_t ret_checksum = packet_answer_get_checksum(buff, len)
uint8_t calc_checksum = calculate_checksum(buff, len)
int header_ok = packet_answer_check_header(buff, len);
int checksum_ok = ret_checksum == calc_checksum
if (header_ok == 1 && checksum_ok == 1) {
    // Success!
    // ...
}
```

### Get data from packet

Data pointer contains mapped data from sensor or actuator
```cpp
uint8_t err = packet_answer_get_error(buff, len);
uint8_t id = packet_answer_get_device_id(buff, len);
uint8_t data_len = packet_answer_get_data_length(buff, len);
const uint8_t* data = packet_answer_get_data_begin(buff, len);
```

### Read multiple packets from one buffer

```cpp
const uint8_t* pkt = buff;
int pkt_len = 0;
do {
    pkt_len = len - (buff - pkt)
    // Parse single packet (pkt)
    // ...
} while((pkt = packet_answer_get_next_packet(pkt, pkt_len)) != NULL);
```
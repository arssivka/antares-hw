#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "port_handler.h"
#include "port_controller.h"
#include "packet_handler.h"
#include "dxl_controller.h"

#include <memory>
#include <chrono>
#include <vector>
#include <thread>
#include <iostream>
#include <map>
extern dxl_packet packets[10];
int cb(int port, uint8_t *data, int read_bytes) {
	uint8_t * buf = data;
	int left_bytes;
	dxl_packet *packet = &packets[port];
	while (buf != data + read_bytes)
	{
		buf = validate_packet(packet, buf, read_bytes);
		
		left_bytes += packet->left_bytes;
		if (packet->state == 8)
		{
			print_packet(packet);
			init_packet(packet);
		}
	}
	return left_bytes;
}/*
void test_port_handler()
{
	port_handler *ph;
	port_handler *ph2; 
	int ret; 
	int pack_len; 
	uint8_t data[1];
	data[0] = 1;
	uint8_t in[255];
	uint8_t out[255];
	ret = make_ping(in, 255, 1);
	
	ph = create_port_handler("/dev/ttyACM0", 57600);
	ret = open_port(ph);
	
	ph2 = create_port_handler("/dev/ttyACM1", 57600);
	ret = open_port(ph2);
	struct timespec start, end;
	while (1)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
		pack_len = make_write(in, 255, 1, 0x19, data, 1);
		
		data[0] = ~data[0] & 0x01;
		ret = write_port(ph, in, pack_len);
		
		ret = read_port(ph, out, 6);
	
		ret = write_port(ph2, in, pack_len);
		ret = read_port(ph2, out, 6);
	
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us);
	}
	close_port(ph);
	close_port(ph2);
	
}
void test_ping()
{
	port_handler *ph;
	port_handler *ph2; 
	int ret; 
	int pack_len; 
	uint8_t data[1];
	data[0] = 0;
	uint8_t in[255];
	uint8_t out[255];
	pack_len = make_ping(in, 255, 1);
	
	ph = create_port_handler("/dev/ttyACM0", 57600);
	ret = open_port(ph);
	ph2 = create_port_handler("/dev/ttyACM1", 57600);
	ret = open_port(ph2);
	
	struct timespec start, end;
	while (1)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
		pack_len = make_write(in, 255, 1, 0x19, data, 1);
		
		data[0] = ~data[0] & 0x01;
		ret = write_port(ph, in, pack_len);
		ret = read_port(ph, out, 6);
	
		ret = write_port(ph2, in, pack_len);
		ret = read_port(ph2, out, 6);
	
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us);
	}
	close_port(ph);
	close_port(ph2);
	
}
void test_port_controller()
{
	int i;
	int ids[] = { 0, 1 };
	int data_in_size[] = { 8, 8 };
	uint8_t data_in[2][8];
	uint8_t *data_in_ptr[2];
	data_in_ptr[0] = &data_in[0];
	data_in_ptr[1] = &data_in[1];
	uint8_t data[1];
	data[0] = 1;
	int data_out_size[] = { 4, 4 };
	uint8_t data_out[2][6];
	uint8_t *data_out_ptr[2];
	data_out_ptr[0] = &data_out[0];
	data_out_ptr[1] = &data_out[1];
	add_port("/dev/ttyACM1", 57600);
	//add_port("/dev/ttymxc0", 57600);
	add_port("/dev/ttyACM0", 57600);
	open_ports();
	struct timespec start, end;
	uint8_t addr = 1;
	while (1)
	{
		data[0] = ~data[0] & 0x01;
		make_write(data_in[0], 255, 2, 0x19, data, 1);
		make_read(data_in[0], 255, 2, addr, 1);
		for (i = 0; i < sizeof(ids) / sizeof(int); ++i)
		{
			data_in_size[i] = 8;
			data_out_size[i] = 7;
			data_out_ptr[i] = &data_out[i];
			data_in_ptr[i] = &data_in[i];
		}
		
		init_packet(&packets[0]);
		init_packet(&packets[1]);
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		write_read_ports(ids,
			data_in_ptr,
			data_in_size,
			data_out_ptr,
			data_out_size,
			1, cb); 
	     
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);

		++addr;
		if (addr == 0x31)
			addr = 1;
		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us);    
	}
}

void test_multiple_read_controller()
{
	int port_ids[] = {};
	int motor_ids[] = {};
	
}*/
void packet_parser_test()
{
	uint8_t buf[3][3] = {{0x00, 0x00, 0xFF},{0xFF, 0x01, 0x02}, {0x00, 0xFC, 0xFF}};
	packets[0].left_bytes = 4; 
	validate_packet(&packets[0], &buf[0][0], 3);
	validate_packet(&packets[0], &buf[1][0], 3);
	validate_packet(&packets[0], &buf[2][0], 3);
	//return packets[0].left_bytes;
}
extern port_controller controller;
void test_dxl_controller()
{
	uint8_t buf[255];
	uint8_t out_buf[255];
	uint8_t id_list[] = { 1, 2};
	uint8_t data_length;
	int port_speed = 500000;
	register_port("/dev/ttyACM0", port_speed);
	register_port("/dev/ttyACM1", port_speed);
	init_dxl_controller();
	add_dxl_device(0, 1);
	add_dxl_device(0, 2);
	struct timespec start, end;
	while (1)
	{
		//clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		read_dxl_device(0, id_list, 2);
		
		//clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us);    

	}
	deinit_dxl_controller();
}
void test_dxl_controller_write()
{
	uint8_t buf[255];
	uint8_t out_buf[255];
	uint8_t id_list[] = { 1, 2 };
	uint8_t data_length;
	int port_speed = 500000;
	register_port("/dev/ttyACM0", port_speed);
	register_port("/dev/ttyACM1", port_speed);
	init_dxl_controller();
	add_dxl_device(0, 1);
	add_dxl_device(0, 2);
	struct timespec start, end;
	dxl_data dev[2];// = { { .addr = 0x19, .data = { 1 }, .length = 0x01 }, { .addr = 0x19, .data = { 1 }, .length = 0x01 } };
	while (1)
	{
		//clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		write_multiple_device(0, 0, id_list, dev, 2);
		dev[0].data[0] = ~dev[0].data[0] & 0x01;
		dev[1].data[0] = ~dev[1].data[0] & 0x01;
		//clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us);    

	}
	deinit_dxl_controller();
}
void test_dxl_controller_set_pos()
{
	uint8_t buf[255];
	uint8_t out_buf[255];
	uint8_t id_list[] = { 1, 2, 3 };
	uint8_t data_length;
	int port_speed = 500000;
	register_port("/dev/ttyACM0", port_speed);
	register_port("/dev/ttyACM1", port_speed);
	init_dxl_controller();
	add_dxl_device(0, 1);
	add_dxl_device(0, 2);
	add_dxl_device(0, 3);
	struct timespec start, end;
	uint16_t data[3] = { 0x0800, 0x0800, 0x0800 }; 
	int sign = 1; 
	/*data[0] = 200; 
	data[1] = 200;
	data[2] = 200; 
	set_dxl_speed(0, 0, id_list, data, 3);*/
	data[0] = 2048; 
	data[1] = 2048;
	data[2] = 2048; 
	uint8_t diff = 100;
	while (1)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		set_dxl_position(0, 0, id_list, data, 3);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us); 
		usleep(30000);
		
		data[0] += sign * diff; 
		data[1] += sign * diff;
		data[2] += sign * diff; 
		sign = -sign;
	}
	deinit_dxl_controller();
}
int move(float x, float y, float z, float *out)
{
	float angles[3];
	float hl1 = 20;
	float hl2 = 20;
	float lxyz = sqrtf(x*x + y*y + z*z);
	float lxy = hypotf(x, y);
	angles[0] = atan2(y, x) ;
	angles[1] = atan2(z, lxy);
	angles[2] = acosf((hl1*hl1 + hl2*hl2 - lxyz*lxyz) / (2*hl1*hl2));
	int i = 0;
	for (; i < 3; ++i)
		out[i] = angles[i] * 360 / M_PI;
	printf("%f %f %f", out[0], out[1], out[2]);
}

void test_dxl_simple_ik()
{
	float angles[3] = { 180, 180, 180 }; 
	while (1)
	{
		scanf("%f %f %f", &angles[0], &angles[1], &angles[2]);
		move(angles[0], angles[1], angles[2], angles);
		
	}
}
void test_dxl_controller_move()
{
	
	uint8_t buf[255];
	uint8_t out_buf[255];
	uint8_t id_list[] = { 1, 2, 3 };
	uint8_t data_length;
	int port_speed = 500000;
	struct timespec start, end;
	int sign = 1; 
	float angles[3] = { 180, 180, 180 }; 
	register_port("/dev/ttyACM0", port_speed);
	register_port("/dev/ttyACM1", port_speed);
	init_dxl_controller();
	add_dxl_device(0, 1);
	add_dxl_device(0, 2);
	add_dxl_device(0, 3);
	uint16_t data[3];
	data[0] = 200; 
	data[1] = 200;
	data[2] = 200; 
	set_dxl_speed(0, 0, id_list, data, 3);
	while (1)
	{
		scanf("%f %f %f", &angles[0], &angles[1], &angles[2]);
		move(angles[0], angles[1], angles[2], angles);
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		set_dxl_angle(0, 0, id_list, angles, 3);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us); 
	}
	deinit_dxl_controller();
}
void test_dxl_controller_input()
{
	uint8_t buf[255];
	uint8_t out_buf[255];
	uint8_t id_list[] = { 1, 2, 3 };
	uint8_t data_length;
	int port_speed = 500000;
	struct timespec start, end;
	int sign = 1; 
	float angles[3] = { 180, 180, 180 }; 
	register_port("/dev/ttyACM0", port_speed);
	register_port("/dev/ttyACM1", port_speed);
	init_dxl_controller();
	add_dxl_device(0, 1);
	add_dxl_device(0, 2);
	//add_dxl_device(0, 3);
	uint16_t data[3];
	data[0] = 200; 
	data[1] = 200;
	data[2] = 200; 
	int motor_cnt = 2;
	set_dxl_speed(0, 0, id_list, data, motor_cnt);
	while (1)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		set_dxl_angle(0, 0, id_list, angles, motor_cnt);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("took %llu\n", delta_us); 
		scanf("%f %f %f", &angles[0], &angles[1], &angles[2]);
	}
	deinit_dxl_controller();
}
class BaseMotionController
{
protected:
	float min_angle;
	float max_angle;
	float start_angle;
	float current_angle;
public:
	BaseMotionController(float min_angle, float max_angle, float start_angle)
		: min_angle(min_angle)
		, max_angle(max_angle)
		, start_angle(start_angle)
		, current_angle(start_angle)
	{};
	virtual float next(int msec)
	{
	}
	virtual ~BaseMotionController() {};
};
class LinearLoopedMotionController:
	public BaseMotionController
{
	int direction;
	float speed;
	float time;
public:
	LinearLoopedMotionController(float min_angle, float max_angle, float start_angle, float time = 1000, int direction = 1) 
		: BaseMotionController(min_angle, max_angle, start_angle)
		, time(time)
		, direction(direction)
	{	speed = (max_angle - min_angle) / time;};
	float next(int msec)
	{
		float next_angle = current_angle + direction*speed*msec;
		if (next_angle >= max_angle) {
			next_angle = max_angle;
			direction = -1;
		}
		else if (next_angle <= min_angle) {
			next_angle = min_angle;
			direction = 1;
		}
		current_angle = next_angle;
		return current_angle;
	}
};

void test_broadcast()
{
	int i = 0;
	uint8_t id_list[10];
	float angles[10];
	uint16_t data[10];
	register_port("/dev/ttyACM1", 57200);
	register_port("/dev/ttyACM0", 57200);
	init_dxl_controller();
	for (i = 1; i < 3; i++) {
		add_dxl_device(0, i);
		add_dxl_device(1, i);
		id_list[i - 1] = i;
		angles[i - 1] = 180;
		data[i - 1] = 500;
	}
	set_dxl_speed(1, 0, id_list, data, 2);
	set_dxl_angle(1, 0, id_list, angles, 2);
	set_dxl_speed(1, 1, id_list, data, 2);
	set_dxl_angle(1, 1, id_list, angles, 2);
	id_list[0] = 6;
	auto pelvis = std::vector<std::pair<std::shared_ptr<BaseMotionController>, int>> {
		std::make_pair(std::make_shared<LinearLoopedMotionController>(188 - 5, 188 + 5, 188, 200, 1), 5),
		std::make_pair(std::make_shared<LinearLoopedMotionController>(162 - 5, 162 + 5, 162, 200, -1), 5)
	};
	decltype(std::chrono::high_resolution_clock::now()) start, end;
	std::chrono::duration<double, std::micro> elapsed;
	/*while (true)
	{
		start = std::chrono::high_resolution_clock::now();
		angles[0] = 188;//pelvis[0].first->next(8);
		set_dxl_angle(1, 0, id_list, angles, 1);
		angles[0] = 162;//pelvis[1].first->next(8);
		
		set_dxl_angle(1, 1, id_list, angles, 1);
		
		end = std::chrono::high_resolution_clock::now();
		elapsed = end - start;
		int sleep_time = 8000 - elapsed.count();
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}*/
	while (true)
	{
		data[0] = 0;
		start = std::chrono::high_resolution_clock::now();
		//angles[0] = 188; //pelvis[0].first->next(8);
		set_dxl_bin_angle(1, 0, id_list, data, 1);
		//angles[0] = 162; //pelvis[1].first->next(8);
		
		//std::this_thread::sleep_for(std::chrono::microseconds(3000));
		data[0] = 1;
		set_dxl_bin_angle(1, 1, id_list, data, 1);
		
		end = std::chrono::high_resolution_clock::now();
		elapsed = end - start;
		int sleep_time = 8000 - elapsed.count();
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}
}
void test_new()
{
	system("echo '1-1' > /sys/bus/usb/drivers/usb/unbind");
	system("echo '1-1' > /sys/bus/usb/drivers/usb/bind");
	register_port("/dev/ttyACM1", 57200);
	register_port("/dev/ttymxc0", 57200);
	register_port("/dev/ttyACM0", 57200);
	init_dxl_controller();
	size_t i;
	uint8_t id_list[10];
	float angles[10]; 
	uint16_t data[10];
	for (i = 1; i < 11; i++) {
		add_dxl_device(0, i);
		add_dxl_device(1, i);
		if(i < 3)
			add_dxl_device(2, i);
		id_list[i - 1] = i;
		angles[i - 1] = 180;
		data[i - 1] = 50;
		usleep(100);
	}
	angles[3] = 180;
	angles[4] = 90;
	set_dxl_speed(0, 0, id_list, data, 10);
	set_dxl_angle(0, 0, id_list, angles, 10);
	angles[6] = 150;
	set_dxl_speed(0, 1, id_list, data, 10);
	set_dxl_angle(0, 1, id_list, angles, 10);
	
	set_dxl_speed(0, 2, id_list, data, 2);
	set_dxl_angle(0, 2, id_list, angles, 2);
	i = 10000;
	angles[0] = 270;
	angles[1] = 90;
	angles[2] = 180;
	angles[3] = 180;
	std::vector<std::vector<std::shared_ptr< BaseMotionController >>> motors = { 
		{
		std::make_shared < LinearLoopedMotionController>(250, 290, 270), 
		std::make_shared < LinearLoopedMotionController>(75, 115, 90), 
		std::make_shared < LinearLoopedMotionController>(240, 320, 270), 
		std::make_shared < LinearLoopedMotionController>(220, 260, 240)
		},
		{
		},
		{
			std::make_shared < LinearLoopedMotionController>(160, 200, 180), 
			std::make_shared < LinearLoopedMotionController>(230, 290, 260) 
		}
		
	 };
	auto start = std::chrono::high_resolution_clock::now(); 
	auto end = std::chrono::high_resolution_clock::now();
	float new_angles[3][4];
	uint8_t new_id[3][4];
	new_id[0][0] = 3;
	new_id[0][1] = 8;
	new_id[0][2] = 9;
	new_id[0][3] = 10;
	new_id[2][0] = 1;
	new_id[2][1] = 2;
	while(i--)
	{	
		end = std::chrono::high_resolution_clock::now();
		//usleep(10000);
		std::chrono::duration<double> diff = end - start;
		int ctrl_cnt = 0, motor_cnt = 0;
		for (auto it_ctrl = motors.begin(); it_ctrl != motors.end(); ++it_ctrl) {
			for (auto it_motor = (*it_ctrl).begin(); it_motor != (*it_ctrl).end(); ++it_motor) {
				new_angles[ctrl_cnt][motor_cnt] = (*it_motor)->next(diff.count() * 1000);
				
				++motor_cnt;
			}
			motor_cnt = 0;
			++ctrl_cnt;
		}
		printf("\n");
		start = std::chrono::high_resolution_clock::now();
		set_dxl_angle(0, 0, &new_id[0][0], &new_angles[0][0], 4);
		set_dxl_angle(0, 2, &new_id[2][0], &new_angles[2][0], 2);
		/*set_dxl_angle(0, 1, id_list, angles, 1);
		usleep(1000);*/
	}
	deinit_dxl_controller();
	
}

int main() {
/*	std::map < char, decltype(test_broadcast)> funcs;
	while (true)
	{
		char c;
		while (std::cin.get(c))	{}
		std::thread motor_control(test_broadcast);
		motor_control.detach();
		
	}
	*/
	//test_new();
	//packet_parser_test();
	//test_dxl_controller();/
	//test_dxl_controller_write();
	//test_dxl_controller_set_pos();
	//move(10, 10, 20);
	//test_dxl_simple_ik();
	//test_dxl_controller_move();
	//test_dxl_controller_input();
	/*init_packet(&packets[0]);
	packet_parser_test();
	//test_ping();
	//test_port_handler();
	init_packet(&packets[0]);
	init_packet(&packets[1]);
	test_port_controller();*/
	return 0;
}
/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2019 Guillaume Valadon
 * guillaume@valadon.net
 */

// Declare functions
void check_command(void);
void fan_command(u_int8_t fan_id, u_int8_t* speed);
void log_command(void);
void test_command(char* libuLinux_hal_path);
void temperature_command(u_int8_t sensor_id);
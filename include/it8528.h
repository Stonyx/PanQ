/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2019 Guillaume Valadon
 * guillaume@valadon.net
 */

// Declare functions
int8_t it8528_get_fan_status(u_int8_t fan_id, u_int8_t* status);
int8_t it8528_get_fan_pwm(u_int8_t fan_id, u_int8_t* pwm);
int8_t it8528_get_fan_speed(u_int8_t fan_id, u_int16_t* speed);
int8_t it8528_set_fan_speed(u_int8_t fan_id, u_int8_t speed);
int8_t it8528_get_temperature(u_int8_t sensor_id, double* temperature);
int8_t i8528_get_power_supply_status(u_int8_t power_supply_id, u_int8_t* status);
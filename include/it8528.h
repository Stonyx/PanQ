/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2020 Guillaume Valadon
 * guillaume@valadon.net
 */

int8_t it8528_get_fan_pwm(u_int8_t, u_int8_t*);
int8_t it8528_get_fan_speed(u_int8_t, u_int32_t*);
int8_t it8528_get_fan_status(u_int8_t, u_int8_t*);
int8_t it8528_get_temperature(u_int8_t, double*);
int8_t it8528_set_fan_speed(u_int8_t, u_int8_t);
int8_t it8528_set_front_usb_led(u_int8_t);

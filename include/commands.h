/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2020 Guillaume Valadon
 * guillaume@valadon.net
 */

void command_check(void);
void command_fan(u_int32_t*);
void command_log(void) __attribute__((noreturn));
void command_led(char*);
void command_test(char*);
void command_temperature(void);
void usage(void);

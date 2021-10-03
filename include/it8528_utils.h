/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * This script is free software. You can redistribute it and/or modify it under the terms of the GNU
 * General Public License Version 3 (or at your option any later version) as published by The Free
 * Software Foundation.
 *
 * This script is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * If you did not received a copy of the GNU General Public License along with this script see
 * http://www.gnu.org/copyleft/gpl.html or write to The Free Software Foundation, 675 Mass Ave,
 * Cambridge, MA 02139, USA.
 */

// Define constants
#define IT8528_WAIT_FOR_READY_INPUT 0x02
#define IT8528_WAIT_FOR_READY_OUTPUT 0x01

// Declare functions
int8_t it8528_wait_for_ready(u_int8_t port, u_int8_t direction);
int8_t it8528_clear_buffer();
int8_t it8528_get_byte(u_int8_t command0, u_int8_t command1, u_int8_t* value);
int8_t it8528_set_byte(u_int8_t command0, u_int8_t command1, u_int8_t value);
int8_t it8528_get_double(u_int8_t command0, u_int8_t command1, double* value);
int8_t it8528_send_commands(u_int8_t command0, u_int8_t command1);
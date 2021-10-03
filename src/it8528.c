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

#include <stdlib.h>
#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>
#include "it8528.h"
#include "it8528_utils.h"

// Function called to get the fan status
int8_t it8528_get_fan_status(u_int8_t fan_id, u_int8_t* status_value)
{
  if (fan_id > 5)
  {
    fprintf(stderr, "it8528_get_fan_status: fan ID too big!\n");
    return -1;
  }

  if (ioperm(0x6C, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_fan_status: ioperm(0x6c) failed!\n");
    return -1;
  }

  if (ioperm(0x68, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_fan_status: ioperm(0x68) failed!\n");
    return -1;
  }

  int8_t ret_value;
  ret_value = it8528_get_byte(2, 0x42, status_value);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_get_fan_status: it8528_get_byte() failed!\n");
    return ret_value;
  }

  *status_value = (*status_value &1) ^ 1;

  return 0;
}

// Function called to get the fan PWM value
int8_t it8528_get_fan_pwm(u_int8_t fan_id, u_int8_t* pwm_value)
{
  u_int8_t tmp_pwm_value = 0;
  u_int8_t command = 0;

  if (ioperm(0x6c, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_fan_pwm: ioperm(0x6c) failed!\n");
    return -1;
  }

  if (ioperm(0x68, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_fan_pwm: ioperm(0x68) failed!\n");
    return -1;
  }

  if (fan_id < 0)
  {
    fprintf(stderr, "it8528_get_fan_pwm: invalid fan ID!\n");
    return -1;
  }
  if (fan_id < 5)
  {
    command = 0x2e;
  }
  else
  {
    command = 0x4b;
  }

  int8_t ret_value;

  ret_value = it8528_get_byte(2, command, &tmp_pwm_value);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_get_fan_pwm: it8528_get_byte() failed!\n");
    return ret_value;
  }

  *pwm_value = (tmp_pwm_value *0x100 - tmp_pwm_value) / 100;

  return 0;
}

// Function called to get the fan RPM value
int8_t it8528_get_fan_speed(u_int8_t fan_id, u_int32_t* speed_value)
{
  // Declare needed variables
  u_int16_t command0;
  u_int16_t command1;

  if (ioperm(0x6C, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_fan_speed: ioperm(0x6c) failed!\n");
    return -1;
  }

  if (ioperm(0x68, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_fan_speed: ioperm(0x68) failed!\n");
    return -1;
  }

  // The following switch statement is an exact copy (with the exception of the variable names
  //   and the default case) of the switch statement found in the libuLinux_hal.so library's
  //   ec_sys_get_fan_speed function as decompiled by IDA
  switch (fan_id)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      command0 = 2 * (fan_id + 0x0312);
      command1 = 2 * fan_id + 0x0625;
      break;
    case 6:
    case 7:
      command0 = 2 * (fan_id + 0x030A);
      command1 = 2 * (fan_id - 0x06) + 0x621;
      break;
    // The following fan ID seems to be only valid if in the model.conf file in the System IO
    //   section the REDUNDANT_POWER_INFO value is set to yes
    case 10:
      command0 = 0x065B;
      command1 = 0x065A;
      break;
    // The following fan ID seems to be only valid if in the model.conf file in the System IO
    //   section the REDUNDANT_POWER_INFO value is set to yes
    case 11:
      command0 = 0x065E;
      command1 = 0x065D;
      break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      command0 = 2 * (fan_id + 0x030E);
      command1 = 2 * (fan_id - 0x14) + 0x0645;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      command0 = 2 * (fan_id + 0x02F8);
      command1 = 2 * (fan_id - 0x1E) + 0x062D;
      break;
    default:
      fprintf(stderr, "it8528_get_fan_speed: invalid fan ID!\n");
      return -1;
  }

  int8_t ret_value;
  u_int8_t tmp_value = 0;

  ret_value = it8528_get_byte(command0 & 0xFF, (command0 >> 8) & 0xFF , &tmp_value);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_get_fan_speed: it8528_get_byte() #1 failed!\n");
    return ret_value;
  }
  *speed_value = ((u_int32_t) tmp_value) << 8;

  ret_value = it8528_get_byte(command1 & 0xFF, (command1 >> 8) & 0xFF, &tmp_value);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_get_fan_speed: it8528_get_byte() #2 failed!\n");
    return ret_value;
  }
  *speed_value += tmp_value;

  return 0;
}

// Function called to get the temperature
int8_t it8528_get_temperature(u_int8_t sensor_id, double* temperature_value)
{
  u_int8_t command = 0;

  if (ioperm(0x6c, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_temperature: ioperm(0x6c) failed!\n");
    return -1;
  }

  if (ioperm(0x68, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_get_temperature: ioperm(0x68) failed!\n");
    return -1;
  }

  command = sensor_id;

  switch (sensor_id)
  {
    case 0:
    case 1:
      break;
    case 5:
    case 6:
    case 7:
      command -= 3;
      break;
    case 10:
      command = 0x59;
      break;
    case 11:
      command = 0x5C;
      break;
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
      command = command - 9;
      break;
    default:
      command = 0xD6;	// 0x4D6 in the original binary
      break;
  }

  int8_t ret_value;

  ret_value = it8528_get_double(6, command, temperature_value);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_get_temperature: it8528_get_double() failed!\n");
    return ret_value;
  }

  return 0;
}

// Function called to set the front LED
int8_t it8528_set_front_usb_led(u_int8_t led_mode)
{
  int8_t ret_value;

  if (led_mode > 3)
  {
    fprintf(stderr, "it8528_set_front_usb_led: invalid LED mode!\n");
    return -1;
  }

  if (ioperm(0x6c, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_set_front_usb_led: ioperm(0x6c) failed!\n");
    return -1;
  }

  if (ioperm(0x68, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_set_front_usb_led: ioperm(0x68) failed!\n");
    return -1;
  }

  ret_value = it8528_set_byte(1, 0x54, led_mode);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_set_front_usb_led: it8528_set_byte() failed!\n");
    return ret_value;
  }

  return 0;
}

// Function called to set the fan speed
int8_t it8528_set_fan_speed(u_int8_t fan_id, u_int8_t fan_speed)
{
  u_int8_t command0, command1;

  if (fan_id < 0)
  {
    fprintf(stderr, "it8528_set_fan_speed: invalid fan ID!\n");
    return -1;
  }

  if (ioperm(0x6c, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_set_fan_speed: ioperm(0x6c) failed!\n");
    return -1;
  }

  if (ioperm(0x68, 1, 1) != 0)
  {
    fprintf(stderr, "it8528_set_fan_speed: ioperm(0x68) failed!\n");
    return -1;
  }

  if (fan_id < 5)
  {
    command0 = 0x20;
    command1 = 0x2e;
  }
  else
  {
    command0 = 0x23;
    command1 = 0x4b;
  }

  int8_t ret_value;

  ret_value = it8528_set_byte(2, command0, 0x10);
  if (ret_value != 0)
  {
    return ret_value;
  }

  u_int8_t fan_speed_normalized = (fan_speed * 100) / 0xFF;
  ret_value = it8528_set_byte(2, command1, fan_speed_normalized);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_set_fan_speed: it8528_set_byte() failed!\n");
    return ret_value;
  }

  return 0;
}
/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2019 Guillaume Valadon
 * guillaume@valadon.net
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/io.h>
#include "it8528_utils.h"
#include "it8528.h"

// Define macros
#define BYTE1(x) x & 0xFF
#define BYTE2(x) (x >> 8) & 0xFF

// Function called to get the fan status
int8_t it8528_get_fan_status(u_int8_t fan_id, u_int8_t* status)
{
  // Declare needed variables
  u_int16_t command;

  // The following switch statement is a copy of the switch statement found in the
  //   libuLinux_hal.so library's ec_sys_get_fan_pwm function as decompiled by IDA
  switch (fan_id)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      command = 0x0242;
      break;
    case 6:
    case 7:
      command = 0x0244;
      break;
    case 10:
    case 11:
      fprintf(stderr, "it8528_get_fan_status: invalid fan ID!\n");
      return -1;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      command = 0x0259;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      command = 0x025A;
      break;
    default:
      fprintf(stderr, "it8528_get_fan_status: invalid fan ID!\n");
      return -1;
  }

  // Declare needed variables
  u_int8_t byte;

  // The following if statement is a copy of the if statement found in the libuLinux_hal.so library's
  //   ec_sys_get_fan_status function as decompiled by IDA
  if (fan_id > 5)
  {
    if (fan_id > 7)
    {
      if (fan_id <= 19 || fan_id > 25)
      {
        if (fan_id > 29 && fan_id <= 35)
        {
          // Get a byte
          if (it8528_get_byte(BYTE1(command), BYTE2(command), &byte) != 0)
          {
            fprintf(stderr, "it8528_get_fan_status: it8528_get_byte() failed!\n");
            return -1;
          }

          // Convert the byte to the status value
          *status = (((int32_t)byte >> (fan_id - 30)) & 0x01) == 0;
        }
      }
      else
      {
        // Get a byte
        if (it8528_get_byte(BYTE1(command), BYTE2(command), &byte) != 0)
        {
          fprintf(stderr, "it8528_get_fan_status: it8528_get_byte() failed!\n");
          return -1;
        }

        // Convert the byte to the status value
        *status = (((int32_t)byte >> (fan_id - 20)) & 0x01) == 0;
      }
    }
    else
    {
      // Get a byte
      if (it8528_get_byte(BYTE1(command), BYTE2(command), &byte) != 0)
      {
        fprintf(stderr, "it8528_get_fan_status: it8528_get_byte() failed!\n");
        return -1;
      }

      // Convert the byte to the status value
      *status = (((int32_t)byte >> (fan_id - 6)) & 0x01) == 0;
    }
  }
  else
  {
    // Get a byte
    if (it8528_get_byte(BYTE1(command), BYTE2(command), &byte) != 0)
    {
      fprintf(stderr, "it8528_get_fan_status: it8528_get_byte() failed!\n");
      return -1;
    }

    // Convert the byte to the status value
    *status = (((int32_t)byte >> fan_id) & 0x01) == 0;
  }

  return 0;
}

// Function called to get the fan PWM
int8_t it8528_get_fan_pwm(u_int8_t fan_id, u_int8_t* pwm)
{
  // Declare needed variables
  u_int16_t command;

  // The following switch statement is a copy of the switch statement found in the
  //   libuLinux_hal.so library's ec_sys_get_fan_pwm function as decompiled by IDA
  switch (fan_id)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      command = 0x022E;
      break;
    case 6:
    case 7:
      command = 0x024B;
      break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      command = 0x022F;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      command = 0x023B;
      break;
    default:
      fprintf(stderr, "it8528_get_fan_pwm: invalid fan ID!\n");
      return -1;
  }

  // Declare needed variables
  u_int8_t byte;

  // Get a byte
  if (it8528_get_byte(BYTE1(command), BYTE2(command), &byte) != 0)
  {
    fprintf(stderr, "it8528_get_fan_pwm: it8528_get_byte() failed!\n");
    return -1;
  }

  // Convert the byte to the pwm value
  // The following formula is a copy of the formula in the libuLinux_hal.so library's
  //   ec_sys_get_fan_pwm function as decompiled by IDA
  *pwm = ((int32_t)((u_int64_t)(0x51999999E1 * byte) >> 32) >> 5) - byte / 0x808081;

  return 0;
}

// Function called to get the fan speed in RPM
int8_t it8528_get_fan_speed(u_int8_t fan_id, u_int16_t* speed)
{
  // Declare needed variables
  u_int16_t command1;
  u_int16_t command2;

  // The following switch statement is a copy of the switch statement found in the
  //   libuLinux_hal.so library's ec_sys_get_fan_speed function as decompiled by IDA
  switch (fan_id)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      command1 = 2 * (fan_id + 0x0312);
      command2 = 2 * fan_id + 0x0625;
      break;
    case 6:
    case 7:
      command1 = 2 * (fan_id + 0x030A);
      command2 = 2 * (fan_id - 0x06) + 0x621;
      break;
    // The following fan ID seems to be only valid if in the model.conf file in the System IO
    //   section the REDUNDANT_POWER_INFO value is set to yes
    case 10:
      command1 = 0x065B;
      command2 = 0x065A;
      break;
    // The following fan ID seems to be only valid if in the model.conf file in the System IO
    //   section the REDUNDANT_POWER_INFO value is set to yes
    case 11:
      command1 = 0x065E;
      command2 = 0x065D;
      break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      command1 = 2 * (fan_id + 0x030E);
      command2 = 2 * (fan_id - 0x14) + 0x0645;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      command1 = 2 * (fan_id + 0x02F8);
      command2 = 2 * (fan_id - 0x1E) + 0x062D;
      break;
    default:
      fprintf(stderr, "it8528_get_fan_speed: invalid fan ID!\n");
      return -1;
  }

  // Declare needed variables
  u_int8_t byte = 0;

  // Get a byte
  if (it8528_get_byte(BYTE1(command1), BYTE2(command1), &byte) != 0)
  {
    fprintf(stderr, "it8528_get_fan_speed: it8528_get_byte() failed!\n");
    return -1;
  }

  // Add the first byte to the speed value
  *speed = byte;
  *speed <<= 8;

  // Get a second byte
  if (it8528_get_byte(BYTE1(command2), BYTE2(command2), &byte) != 0)
  {
    fprintf(stderr, "it8528_get_fan_speed: it8528_get_byte() failed!\n");
    return -1;
  }

  // Add the second byte to the speed value
  *speed |= byte;

  return 0;
}

// Function called to set the fan speed in percentage
int8_t it8528_set_fan_speed(u_int8_t fan_id, u_int8_t speed)
{
  // Declare needed variables
  u_int16_t command1;
  u_int16_t command2;

  // The following switch statement is a copy of the switch statement found in the
  //   libuLinux_hal.so library's ec_sys_get_fan_speed function as decompiled by IDA
  switch (fan_id)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      command1 = 0x0220;
      command2 = 0x022E;
      break;
    case 6:
    case 7:
      command1 = 0x0223;
      command2 = 0x024B;
      break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      command1 = 0x0221;
      command2 = 0x022F;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      command1 = 0x0222;
      command2 = 0x023B;
      break;
    default:
      fprintf(stderr, "it8528_set_fan_speed: invalid fan ID!\n");
      return -1;
  }

  // Set a byte
  if (it8528_set_byte(BYTE1(command1), BYTE2(command1), 0x10) != 0)
  {
    fprintf(stderr, "it8528_set_fan_speed: it8528_set_byte() failed!\n");
    return -1;
  }

  // The following formula is a copy of the formula in the libuLinux_hal.so library's
  //   ec_sys_set_fan_speed function as decompiled by IDA
  u_int8_t normalized_speed = 100 * speed / 255;

  // Set a second byte
  if (it8528_set_byte(BYTE1(command2), BYTE2(command2), normalized_speed) != 0)
  {
    fprintf(stderr, "it8528_set_fan_speed: it8528_set_byte() failed!\n");
    return -1;
  }

  return 0;
}

// Function called to get the temperature
int8_t it8528_get_temperature(u_int8_t sensor_id, double* temperature)
{
  // Declare needed variables
  u_int16_t command;

  // The following switch statement is a copy of the switch statement found in the
  //   libuLinux_hal.so library's ec_sys_get_temperature function as decompiled by IDA
  switch (sensor_id)
  {
    case 0:
    case 1:
      command = sensor_id + 0x0600;
      break;
    case 5:
    case 6:
    case 7:
      command = sensor_id + 0x05FD;
      break;
    case 10:
      command = 0x0659;
      break;
    case 11:
      command = 0x065C;
      break;
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
      command = sensor_id + 0x05F7;
      break;
    default:
      fprintf(stderr, "it8528_get_temperature: invalid sensor ID!\n");
      return -1;
  }

  // Declare needed variables
  u_int8_t byte;

  // Get a byte
  if (it8528_get_byte(BYTE1(command), BYTE2(command), &byte) != 0)
  {
    fprintf(stderr, "it8528_get_temperature: it8528_get_byte() failed!\n");
    return -1;
  }

  // Convert the byte to the temperature value
  *temperature = (double)byte;

  return 0;
}

// Function called to get the power supply status
int8_t i8528_get_power_supply_status(u_int8_t power_supply_id, u_int8_t* status)
{
  // Check if the power supply ID is valid
  if (power_supply_id > 0 && power_supply_id <= 2)
  {
    // Declare needed variables
    u_int8_t byte;

    // Get a byte
    if (it8528_get_byte(0x00, 0x45, &byte) != 0)
    {
      fprintf(stderr, "it8528_get_power_supply_status: it8528_get_byte() failed!\n");
      return -1;
    }

    // Convert the byte to the status value
    // The following formula is a copy of the formula in the libuLinux_hal.so library's
    //   ec_sys_get_power_supply_status function as decompiled by IDA
    *status = (((int32_t)byte >> power_supply_id) & 0x01) == 0;
  }
  else
  {
    fprintf(stderr, "it8528_get_power_supply_status: invalid power supply ID!\n");
    return -1;
  }

  return 0;
}
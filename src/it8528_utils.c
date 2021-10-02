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
#include <time.h>
#include <unistd.h>
#include "it8528_utils.h"
#define IT8528_CHECK_RETRIES 400
#define IT8528_INPUT_BUFFER_FULL 2
#define IT8528_OUTPUT_BUFFER_FULL 1

// Function called to check if the IT8528 chip port is ready to be used
int8_t it8528_check_ready(u_int8_t port, u_int8_t bit_value)
{
  int retries = IT8528_CHECK_RETRIES;
  int value = 0;

  // Poll the bit value
  do {
    value = inb(port);
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 0x32 * 1000
    };
    nanosleep(&ts, NULL);

    if ((value & bit_value) == 0)
    {
      return 0;
    }
  }
  while (retries--);

  return -1;
}

// Function called to clear the IT8528 chip buffers
u_int8_t it8528_clear_buffer(u_int8_t port)
{
  // Read byte from port

  int retries = 5000;
  int value;

 	// Poll the bit value
  do {
    value = inb(0x6C);
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 0x32 * 1000
    };
    nanosleep(&ts, NULL);

    if ((value & 1) != 0)
    {
      return 0;
    }
  }
  while (retries--);

  return -1;
}

// Function called to send the IT8528 chip a command
int8_t it8528_send_commands(u_int8_t command0, u_int8_t command1)
{
  int8_t ret_value;

  ret_value = it8528_check_ready(0x6C, IT8528_OUTPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_check_ready() #1 failed!\n");
    return ret_value;
  }
  inb(0x68);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_check_ready() #2 failed!\n");
    return ret_value;
  }
  outb(0x88, 0x6C);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_check_ready() #3 failed!\n");
    return ret_value;
  }
  outb(command0, 0x68);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_check_ready() #4 failed!\n");
    return ret_value;
  }
  outb(command1, 0x68);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_check_ready() #5 failed!\n");
    return ret_value;
  }

  return 0;
}

// Function called to read a double value from the IT8528 chip
int8_t it8528_get_double(u_int8_t command0, u_int8_t command1, double *value)
{
  int8_t ret_value;

  if ((inb(0x6C) &1) != 0)
  {
    it8528_clear_buffer(0x68);
    inb(0x68);
  }

  ret_value = it8528_send_commands(command0, command1);
  if (ret_value != 0)
  {
    return ret_value;
  }
  it8528_clear_buffer(0x68);
  *value = inb(0x68);

  return 0;
}

// Function called to read a byte value from the IT8528 chip
int8_t it8528_get_byte(u_int8_t command0, u_int8_t command1, u_int8_t *value)
{
  int8_t ret_value;

  if ((inb(0x6C) &1) != 0)
  {
    it8528_clear_buffer(0x68);
    inb(0x68);
  }

  ret_value = it8528_send_commands(command0, command1);
  if (ret_value != 0)
  {
    fprintf(stderr, "it8528_get_byte: it8528_send_commands() failed!\n");
    return ret_value;
  }
  it8528_clear_buffer(0x68);
  *value = inb(0x68);

  return 0;
}

// Function called to write a byte value to the IT8528 byte
int8_t it8528_set_byte(u_int8_t command0, u_int8_t command1, u_int8_t value)
{
  int8_t ret_value;

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    return ret_value;
  }
  outb(0x88, 0x6C);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    return ret_value;
  }
  outb(command0 | 0x80, 0x68);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    return ret_value;
  }
  outb(command1, 0x68);

  ret_value = it8528_check_ready(0x6C, IT8528_INPUT_BUFFER_FULL);
  if (ret_value != 0)
  {
    return ret_value;
  }
  outb(value, 0x68);

  return 0;
}
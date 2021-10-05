/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2020 Guillaume Valadon
 * guillaume@valadon.net
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/io.h>
#include <time.h>
#include <unistd.h>
#include "it8528_utils.h"

// Define constants
#define IT8528_WAIT_FOR_READY_RETRIES 400
#define IT8528_CLEAR_BUFFER_RETRIES 5000

// Function called to check if an IT8528 chip is present
int8_t it8528_check_if_present(void)
{
  // Write 0x20 to the first ID port
  outb(0x20, IT8528_ID_PORT_1);

  // Read a byte from the second ID port
  u_int8_t byte_1 = inb(IT8528_ID_PORT_2);

  // Write 0x21 to the first ID port
  outb(0x21, IT8528_ID_PORT_1);

  // Read a byte from the second ID port
  u_int8_t byte_2 = inb(IT8528_ID_PORT_2);

  // Check if the ID matches
  if (byte_1 == 0x85 && byte_2 == 0x28)
  {
    return 0;
  }
  else
  {
    fprintf(stderr, "IT8528 not found!\n");
    return -1;
  }
}

// Function called to read a byte from the IT8528 chip
int8_t it8528_get_byte(u_int8_t command0, u_int8_t command1, u_int8_t* value)
{
  // Read from the second communication port and check if the read byte has the first bit set to 1
  if ((inb(IT8528_COMM_PORT_2) & 0x01) == 0x01)
  {
    // Clear the chip buffer
    it8528_clear_buffer();

    // Read from the first communication port
    inb(IT8528_COMM_PORT_1);
  }

  // Send the commands
  if (it8528_send_commands(command0, command1) != 0)
  {
    fprintf(stderr, "it8528_get_byte: it8528_send_commands() failed!\n");
    return -1;
  }

  // Clear the chip buffer
  it8528_clear_buffer();

  // Read the byte from first communication port
  *value = inb(IT8528_COMM_PORT_1);

  return 0;
}

// Function called to send a byte to the IT8528 chip
int8_t it8528_set_byte(u_int8_t command0, u_int8_t command1, u_int8_t value)
{
  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_byte(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write 0x88 to the second communication port
  outb(0x88, IT8528_COMM_PORT_2);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_byte(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write the first command bitwise ored with 0x80 to the first communication port
  outb(command0 | 0x80, IT8528_COMM_PORT_1);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_byte(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write the second command to the first communication port
  outb(command1, IT8528_COMM_PORT_1);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_byte(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write the byte to the first communication port
  outb(value, IT8528_COMM_PORT_1);

  return 0;
}

// Function called to read a double from the IT8528 chip
// TODO: rename "value" to something better to match variable name "byte" in above functions
int8_t it8528_get_double(u_int8_t command0, u_int8_t command1, double* value)
{
  // Read from second communication port and check if the read byte has the first bit set to 1
  if ((inb(IT8528_COMM_PORT_2) & 0x01) == 0x01)
  {
    // Clear the chip buffer
    it8528_clear_buffer();

    // Read from the first communication port
    inb(IT8528_COMM_PORT_1);
  }

  // Send the commands
  if (it8528_send_commands(command0, command1) != 0)
  {
    return -1;
  }

  // Clear the chip buffer
  it8528_clear_buffer();

  // Read the double from the first communication port
  *value = inb(IT8528_COMM_PORT_1);

  return 0;
}

// Function called to send commands to the IT8528 chip
int8_t it8528_send_commands(u_int8_t command0, u_int8_t command1)
{
  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_OUTPUT) != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Read from the first communication port
  inb(IT8528_COMM_PORT_1);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write 0x88 to the second communication port
  outb(0x88, IT8528_COMM_PORT_2);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write the first command to the first communication port
  outb(command0, IT8528_COMM_PORT_1);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  // Write the second command to the first communication port
  outb(command1, IT8528_COMM_PORT_1);

  // Wait until the chip is ready
  if (it8528_wait_for_ready(IT8528_WAIT_FOR_READY_INPUT) != 0)
  {
    fprintf(stderr, "it8528_send_commands(): it8528_wait_for_ready() failed!\n");
    return -1;
  }

  return 0;
}

// Function called to check if the IT8528 chip port is ready to be used
int8_t it8528_wait_for_ready(u_int8_t direction)
{
  // Declare needed variables
  int retries = IT8528_WAIT_FOR_READY_RETRIES;
  int byte;
  struct timespec ts = {
    .tv_sec = 0,
    .tv_nsec = 50000
  };

  // Loop until we get the byte we are waiting for or we run out of retries
  do {
    // Read a byte from the second communication port
    byte = inb(IT8528_COMM_PORT_2);

    // Sleep for 50 microseconds
    nanosleep(&ts, NULL);

    // Check if the read byte has the bit that corresponds to the passed in direction set to 0
    if ((byte & direction) == 0x00)
    {
      return 0;
    }
  }
  while (retries--);

  return -1;
}

// Function called to clear the IT8528 chip buffers
int8_t it8528_clear_buffer(void)
{
  // Declare needed variables
  int retries = IT8528_CLEAR_BUFFER_RETRIES;
  int byte;
  struct timespec ts = {
    .tv_sec = 0,
    .tv_nsec = 50000
  };

  // Loop until we get the byte we are waiting for or we run out of retires
  do {
    // Read a byte from the second communication port
    byte = inb(IT8528_COMM_PORT_2);

    // Sleep for 50 microseconds
    nanosleep(&ts, NULL);

    // Check if the read byte has the first bit set to 1
    if ((byte & 0x01) == 0x01)
    {
      return 0;
    }
  }
  while (retries--);

  return -1;
}

/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2020 Guillaume Valadon
 * guillaume@valadon.net
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cap-ng.h>
#include <sys/io.h>
#include "it8528_utils.h"
#include "commands.h"

// Declare functions
void usage(void);

// Function called as main entry point
int main(int argc, char** argv)
{
  // Check if there are no command arguments
  if (argc < 2)
  {
    // Print the usage information and exit
    usage();
    exit(EXIT_FAILURE);
  }

  // Check if we don't have the CAP_SYS_RAW_IO capability and are not running as root
  if (capng_have_capability(CAPNG_EFFECTIVE, CAP_SYS_RAWIO) == 0 &&
     (getuid() != 0 || geteuid() != 0))
  {
    fprintf(stderr, "PanQ must have the CAP_SYS_RAWIO capability, or be launched as root!\n");
    exit(EXIT_FAILURE);
  }

  // Get permission to access the various IT8528 chip ports
  if (ioperm(IT8528_ID_PORT_1, 1, 1) != 0)
  {
    fprintf(stderr, "main: ioperm(IT8528_ID_PORT_1) failed!\n");
    exit(EXIT_FAILURE);
  }
  if (ioperm(IT8528_ID_PORT_2, 1, 1) != 0)
  {
    fprintf(stderr, "main: ioperm(IT8528_ID_PORT_2) failed!\n");
    exit(EXIT_FAILURE);
  }
  if (ioperm(IT8528_COMM_PORT_1, 1, 1) != 0)
  {
    fprintf(stderr, "main: ioperm(IT8528_COMM_PORT_1) failed!\n");
    exit(EXIT_FAILURE);
  }
  if (ioperm(IT8528_COMM_PORT_2, 1, 1) != 0)
  {
    fprintf(stderr, "main: ioperm(IT8528_COMM_PORT_2) failed!\n");
    exit(EXIT_FAILURE);
  }

  // Check if the IT8528 chip is not present
  if (it8528_check_if_present() != 0)
  {
    fprintf(stderr, "IT8528 chip not found");
    exit(EXIT_FAILURE);
  }

  // Call the correct command
  if (strcmp("check", argv[1]) == 0)
  {
    check_command();
  }
  else if (strcmp("fan1", argv[1]) == 0)
  {
    // Check if there is no speed argument
    if (argc == 2)
    {
      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(5, NULL);
    }
    else
    {
      // Convert argument 2 to an integer
      u_int8_t speed = strtoul(argv[2], NULL, 10);

      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(5, &speed);
    }
  }
  else if (strcmp("fan2", argv[1]) == 0)
  {
    // Check if there is no speed argument
    if (argc == 2)
    {
      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(7, NULL);
    }
    else
    {
      // Convert argument 2 to an integer
      u_int8_t speed = strtoul(argv[2], NULL, 10);

      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(7, &speed);
    }
  }
  else if (strcmp("fan3", argv[1]) == 0)
  {
    // Check if there is no speed argument
    if (argc == 2)
    {
      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(25, NULL);
    }
    else
    {
      // Convert argument 2 to an integer
      u_int8_t speed = strtoul(argv[2], NULL, 10);

      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(25, &speed);
    }
  }
  else if (strcmp("fan4", argv[1]) == 0)
  {
    // Check if there is no speed argument
    if (argc == 2)
    {
      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(35, NULL);
    }
    else
    {
      // Convert argument 2 to an integer
      u_int8_t speed = strtoul(argv[2], NULL, 10);

      // Call the fan command function with the correct fan ID based on the fan related switch
      //   statements in the it8528.c file
      fan_command(35, &speed);
    }
  }
  else if (strcmp("help", argv[1]) == 0)
  {
    usage();
  }
  else if (strcmp("log", argv[1]) == 0)
  {
    log_command();
  }
  else if (strcmp("test", argv[1]) == 0)
  {
    if (argc == 2)
    {
      test_command("libuLinux_hal.so");
    }
    else
    {
      test_command(argv[2]);
    }
  }
  else if (strcmp("temp1", argv[1]) == 0)
  {
    // Call the temperature command function with the correct sensor ID based on the switch
    //   statement in the it8528_get_temperature function in the it8528.c file
    temperature_command(1);
  }
  else if (strcmp("temp2", argv[1]) == 0)
  {
    // Call the temperature command function with the correct sensor ID based on the switch
    //   statement in the it8528_get_temperature function in the it8528.c file
    temperature_command(7);
  }
  else if (strcmp("temp3", argv[1]) == 0)
  {
    // Call the temperature command function with the correct sensor ID based on the switch
    //   statement in the it8528_get_temperature function in the it8528.c file
    temperature_command(10);
  }
  else if (strcmp("temp4", argv[1]) == 0)
  {
    // Call the temperature command function with the correct sensor ID based on the switch
    //   statement in the it8528_get_temperature function in the it8528.c file
    temperature_command(11);
  }
  else if (strcmp("temp5", argv[1]) == 0)
  {
    // Call the temperature command function with the correct sensor ID based on the switch
    //   statement in the it8528_get_temperature function in the it8528.c file
    temperature_command(38);
  }
  else
  {
    usage();
  }

  exit(EXIT_SUCCESS);
}

// Function called to print the usage information
void usage(void)
{
  // Print the usage information
  printf("Control the I8528 Super I/O controller chip on QNAP NAS units\n\n");
  printf("Usage: panq { COMMAND | help }\n");
  printf("\n");
  printf("Available commands:\n");
  printf("  check                   - detect the Super I/O controller\n");
  printf("  fan1 [speed_percentage] - get or set the fan #1 speed\n");
  printf("  fan2 [speed_percentage] - get or set the fan #2 speed\n");
  printf("  fan3 [speed_percentage] - get or set the fan #3 speed\n");
  printf("  fan4 [speed_percentage] - get or set the fan #4 speed\n");
  printf("  help                    - this help message\n");
  printf("  log                     - display fan speed & temperature\n");
  printf("  test [libuLinux_hal.so] - test functions against libuLinux_hal.so\n");
  printf("  temp1                   - retrieve the temperature of sensor #1\n");
  printf("  temp2                   - retrieve the temperature of sensor #2\n");
  printf("  temp3                   - retrieve the temperature of sensor #3\n");
  printf("  temp4                   - retrieve the temperature of sensor #4\n");
  printf("  temp5                   - retrieve the temperature of sensor #5\n");
  printf("\n");
}
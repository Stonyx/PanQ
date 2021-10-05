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

  // Check if the IT8528 chip is present
  if (it8528_check_if_present() != 0)
  {
    fprintf(stderr, "IT8528 chip not found");
    exit(EXIT_FAILURE);
  }

  // Parse arguments and call sub-commands
  char* command = argv[1];
  if (strcmp("help", command) == 0)
  {
    usage();
  }
  else if (strcmp("check", command) == 0)
  {
    command_check();
  }
  else if (strcmp("fan", command) == 0)
  {
    u_int32_t* speed = NULL;
    if (argv[2])
    {
      speed = (u_int32_t*) malloc(sizeof(u_int32_t));
      *speed = (u_int32_t) strtoul(argv[2], NULL, 10);
    }
    command_fan(speed);
  }
  else if (strcmp("led", command) == 0)
  {
    if (argv[2])
    {
      command_led(argv[2]);
    }
    else
    {
      fprintf(stderr, "a LED mode is needed!\n");
      exit(EXIT_FAILURE);
    }
  }
  else if (strcmp("log", command) == 0)
  {
    command_log();
  }
  else if (strcmp("test", command) == 0)
  {
    if (argv[2])
    {
      command_test(argv[2]);
    }
    else
    {
      command_test("libuLinux_hal.so");
    }
  }
  else if (strcmp("temperature", command) == 0)
  {
    command_temperature();
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
  printf("  check                    - detect the Super I/O controller\n");
  printf("  fan [SPEED_PERCENTAGE]   - get or set the fan speed\n");
  printf("  help                     - this help message\n");
  printf("  led {on | off | blink}   - configure the front USB LED\n");
  printf("  log                      - display fan speed &temperature\n");
  printf("  test [libuLinux_hal.so]  - test functions against libuLinux_hal.so\n");
  printf("  temperature              - retrieve the temperature\n");
  printf("\n");
}

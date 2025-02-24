/*
 * Copyright (C) 2021 Stonyx
 * http://www.stonyx.com
 *
 * Copyright (C) 2020 Guillaume Valadon
 * guillaume@valadon.net
 */

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "it8528_utils.h"
#include "it8528.h"
#include "commands.h"

// Function called to run the check command
void check_command(void)
{
  // Check if the IT8528 chip is present
  if (it8528_check_if_present())
  {
    printf("IT8528 detected.\n");
  }
  else
  {
    printf("IT8528 not detected.\n");
    exit(EXIT_FAILURE);
  }
}

// Function called to run the fan command
void fan_command(u_int8_t fan_id, u_int8_t* speed)
{
  // Declare needed variables
  u_int8_t status;

  // Get the fan status
  if (it8528_get_fan_status(0, &status) != 0)
  {
    fprintf(stderr, "fan_command: it8528_get_fan_status() failed!\n");
    exit(EXIT_FAILURE);
  }

  // Check the fan status
  if (status == 0)
  {
    fprintf(stderr, "Incorrect fan status!\n");
    exit(EXIT_FAILURE);
  }

  // Check if no speed was supplied
  if (speed == NULL)
  {
    // Declare needed variables
    u_int16_t rpm;

    // Get the fan RPM
    if (it8528_get_fan_speed(fan_id, &rpm) != 0)
    {
      fprintf(stderr, "fan_command: it8528_get_fan_speed() failed!\n");
      exit(EXIT_FAILURE);
    }

    // Print the fan RPM
    printf("%u RPM\n", rpm);
  }
  else
  {
    // Make sure the speed is valid
    if (*speed > 100)
    {
      fprintf(stderr, "Invalid percent!\n");
      exit(EXIT_FAILURE);
    }

    // Set the fan speed
    if (it8528_set_fan_speed(fan_id, *speed) != 0)
    {
      fprintf(stderr, "fan_command: it8528_set_fan_speed() failed!\n");
      exit(EXIT_FAILURE);
    }
  }
}

// Function called to run the log command which prints the fan speed and the temperature
void log_command(void)
{
  u_int8_t status_value = 0xFF;
  int8_t status_ret = it8528_get_fan_status(0, &status_value);
  if (status_ret != 0 && status_value != 0)
  {
    fprintf(stderr, "Incorrect fan status!\n");
    exit(EXIT_FAILURE);
  }

  u_int16_t speed_value;
  if (it8528_get_fan_speed(0, &speed_value) != 0)
  {
    fprintf(stderr, "Can't get fan speed!\n");
    exit(EXIT_FAILURE);
  }

  double temperature_value = 0;
  if (it8528_get_temperature(0, &temperature_value) != 0)
  {
    fprintf(stderr, "Can't get the temperature!\n");
    exit(EXIT_FAILURE);
  }
  printf("%ld,%d,%.2f\n", time(NULL), speed_value, temperature_value);

  exit(EXIT_SUCCESS);
}

// Function called to run the test command which compares the PanQ function with the QNAP ones
//   from the libuLinux_hal.so library
void test_command(char* libuLinux_hal_path)
{
  if (getuid() == 0 && geteuid() == 0)
  {
    fprintf(stderr, "The test command cannot be used as root!\n");
    exit(EXIT_FAILURE);
  }

  void* handle;
  char* error;

  handle = dlopen(libuLinux_hal_path, RTLD_LAZY);
  if (!handle)
  {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  // it8528_get_fan_status
  typedef int8_t(*ec_sys_get_fan_status_t)(u_int8_t, u_int8_t *);
  ec_sys_get_fan_status_t ec_sys_get_fan_status = (ec_sys_get_fan_status_t) dlsym(handle, "ec_sys_get_fan_status");
  error = dlerror();
  if (error != NULL)
  {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  int8_t ret_status_ec, ret_status_it;
  u_int8_t value_status_ec, value_status_it = 0;
  ret_status_ec = ec_sys_get_fan_status(0, &value_status_ec);
  ret_status_it = it8528_get_fan_status(0, &value_status_it);
  if (ret_status_ec != ret_status_it || value_status_ec != value_status_it)
  {
    fprintf(stderr, "it8528_get_fan_status() test failed!\n");
    fprintf(stderr, "ret   %d != %d\n", ret_status_ec, ret_status_it);
    fprintf(stderr, "value %d != %d\n", value_status_ec, value_status_it);
    exit(EXIT_FAILURE);
  }

  // it8528_get_fan_pwm
  typedef int8_t(*ec_sys_get_fan_pwm_t)(u_int8_t, u_int8_t *);
  ec_sys_get_fan_pwm_t ec_sys_get_fan_pwm = (ec_sys_get_fan_pwm_t) dlsym(handle, "ec_sys_get_fan_pwm");

  error = dlerror();
  if (error != NULL)
  {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  int8_t ret_pwm_ec, ret_pwm_it;
  u_int8_t value_pwm_ec, value_pwm_it = 0;
  ret_pwm_ec = ec_sys_get_fan_pwm(0, &value_pwm_ec);
  ret_pwm_it = it8528_get_fan_pwm(0, &value_pwm_it);
  if (ret_pwm_ec != ret_pwm_it || value_pwm_ec != value_pwm_it)
  {
    fprintf(stderr, "it8528_get_fan_pwm() test failed!\n");
    fprintf(stderr, "ret   %d != %d\n", ret_pwm_ec, ret_pwm_it);
    fprintf(stderr, "value %d != %d\n", value_pwm_ec, value_pwm_it);
    exit(EXIT_FAILURE);
  }

  // it8528_get_fan_speed
  typedef int8_t(*ec_sys_get_fan_speed_t)(u_int8_t, u_int32_t *);
  ec_sys_get_fan_speed_t ec_sys_get_fan_speed = (ec_sys_get_fan_speed_t) dlsym(handle, "ec_sys_get_fan_speed");
  error = dlerror();
  if (error != NULL)
  {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  int8_t ret_speed_ec, ret_speed_it = 0;
  u_int32_t value_speed_ec = 0;
  u_int16_t value_speed_it = 0;
  ret_speed_ec = ec_sys_get_fan_speed(0, &value_speed_ec);
  ret_speed_it = it8528_get_fan_speed(0, &value_speed_it);
  if (ret_speed_ec != ret_speed_it || value_speed_ec != value_speed_it)
  {
    fprintf(stderr, "it8528_get_fan_speed() test failed!\n");
    fprintf(stderr, "ret   %d != %d\n", ret_speed_ec, ret_speed_it);
    fprintf(stderr, "value %d != %d\n", value_speed_ec, value_speed_it);
    exit(EXIT_FAILURE);
  }

  // it8528_get_temperature
  typedef int8_t(*ec_sys_get_temperature_t)(u_int8_t, double*);
  ec_sys_get_temperature_t ec_sys_get_temperature = (ec_sys_get_temperature_t) dlsym(handle, "ec_sys_get_temperature");
  error = dlerror();
  if (error != NULL)
  {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  int8_t ret_temperature_ec, ret_temperature_it = 0;
  double value_temperature_ec, value_temperature_it = 0;
  ret_temperature_ec = ec_sys_get_temperature(0, &value_temperature_ec);
  ret_temperature_it = it8528_get_temperature(0, &value_temperature_it);
  if (ret_temperature_ec != ret_temperature_it || value_temperature_ec < value_temperature_it)
  {
    fprintf(stderr, "it8528_get_temperature() test failed!\n");
    fprintf(stderr, "ret   %d != %d\n", ret_temperature_ec, ret_temperature_it);
    fprintf(stderr, "value %f != %f\n", value_temperature_ec, value_temperature_it);
    exit(EXIT_FAILURE);
  }

  printf("All tests passed.\n");
  dlclose(handle);
}

// Function called to run the temperature command
void temperature_command(u_int8_t sensor_id)
{
  // Declare needed variables
  double temperature;

  // Get the temperature
  if (it8528_get_temperature(sensor_id, &temperature) != 0)
  {
    fprintf(stderr, "temperature_command: it8528_get_temperature failed!\n");
    exit(EXIT_FAILURE);
  }

  // Print the temperature
  printf("%.2f °C\n", temperature);
}
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

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <unistd.h>
#include <cap-ng.h>
#include <seccomp.h>
#include "utils.h"

// Function called to configure the secure computing mode
scmp_filter_ctx configure_seccomp(void)
{
  // Configure seccomp filters
  scmp_filter_ctx scmp_ctx = NULL;

  scmp_ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
  if (scmp_ctx == NULL)
  {
    fprintf(stderr, "Can't initialize seccomp!\n");
    exit(EXIT_FAILURE);
  }

  int scmp_rules_status = 0;

  // Accept any arguments
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(capget), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(gettid), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(getuid), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(geteuid), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(nanosleep), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);

  // Restrict syscall arguments values
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(iopl), 1,
    SCMP_A0(SCMP_CMP_EQ, 3));
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioperm), 1,
    SCMP_A0(SCMP_CMP_EQ, 0x6C));
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioperm), 1,
    SCMP_A0(SCMP_CMP_EQ, 0x68));
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 1,
    SCMP_A2(SCMP_CMP_MASKED_EQ, O_RDONLY, O_RDONLY));
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1,
    SCMP_A0(SCMP_CMP_EQ, STDOUT_FILENO));
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1,
    SCMP_A0(SCMP_CMP_EQ, STDERR_FILENO));

  if (scmp_rules_status != 0)
  {
    fprintf(stderr, "Can't add seccomp rules!\n");
    exit(EXIT_FAILURE);
  }

  return scmp_ctx;
}

// Function called to update the secure computing mode
scmp_filter_ctx update_seccomp(scmp_filter_ctx scmp_ctx)
{
  int scmp_rules_status = 0;

  // Accept any arguments
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(getcwd), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(lseek), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(prlimit64), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigaction), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigprocmask), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(semctl), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(semget), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(semop), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_robust_list), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_tid_address), 0);
  scmp_rules_status += seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(stat), 0);

  if (scmp_rules_status != 0)
  {
    fprintf(stderr, "Can't add seccomp rules!\n");
    exit(EXIT_FAILURE);
  }

  return scmp_ctx;
}
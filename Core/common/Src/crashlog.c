/*
 * crashlog.c
 *
 *  Created on: Mar 7, 2026
 *      Author: night
 */

#include "crashlog.h"


volatile crashlog_t g_crashlog __attribute__((section(".noinit.crash")));

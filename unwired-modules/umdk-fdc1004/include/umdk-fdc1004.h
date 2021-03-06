/*
 * Copyright (C) 2016-2018 Unwired Devices LLC <info@unwds.com>

 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @defgroup    
 * @ingroup     
 * @brief       
 * @{
 * @file		umdk-fdc1004.h
 * @brief       umdk-fdc1004 driver module definitions
 * @author      Oleg Artamonov
 */
#ifndef UMDK_FDC1004_H
#define UMDK_FDC1004_H

#include "unwds-common.h"

#define UMDK_FDC1004_STACK_SIZE 1024

#define UMDK_FDC1004_I2C 1

#define UMDK_FDC1004_PUBLISH_PERIOD_MIN 1

typedef enum {
	UMDK_FDC1004_CMD_SET_PERIOD = 0,
	UMDK_FDC1004_CMD_POLL = 1,
	UMDK_FDC1004_CMD_SET_I2C = 2,
} umdk_fdc1004_cmd_t;

void umdk_fdc1004_init(uwnds_cb_t *event_callback);
bool umdk_fdc1004_cmd(module_data_t *data, module_data_t *reply);

#endif /* UMDK_FDC1004_H */

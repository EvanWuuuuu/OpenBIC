/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <logging/log.h>
#include "libutil.h"
#include "sensor.h"
#include "sq52205.h"
#include "hal_i2c.h"

LOG_MODULE_REGISTER(dev_sq52205);

#define SQ52205_CALIBRATION_OFFSET 0x05
#define SQ52205_SHUNT_LSB 0.0000025

uint8_t sq52205_read(uint8_t sensor_num, int *reading)
{
	CHECK_NULL_ARG_WITH_RETURN(reading, SENSOR_UNSPECIFIED_ERROR);

	if (sensor_num > SENSOR_NUM_MAX) {
		LOG_ERR("sensor 0x%x input parameter is invalid", sensor_num);
		return SENSOR_UNSPECIFIED_ERROR;
	}

	sq52205_init_arg *init_arg =
		(sq52205_init_arg *)sensor_config[sensor_config_index_map[sensor_num]].init_args;
	CHECK_NULL_ARG_WITH_RETURN(init_arg, SENSOR_UNSPECIFIED_ERROR);

	if (init_arg->is_init != true) {
		LOG_ERR("device isn't initialized");
		return SENSOR_UNSPECIFIED_ERROR;
	}
	uint8_t retry = 5;
	int ret = 0;
	float val = 0;
	I2C_MSG msg = { 0 };

	sensor_cfg cfg = sensor_config[sensor_config_index_map[sensor_num]];

	msg.bus = cfg.port;
	msg.target_addr = cfg.target_addr;
	msg.tx_len = 1;
	msg.rx_len = 2;
	msg.data[0] = cfg.offset;

	ret = i2c_master_read(&msg, retry);
	if (ret != 0) {
		LOG_ERR("i2c read fail ret: %d", ret);
		return SENSOR_FAIL_TO_ACCESS;
	}

	val = (msg.data[0] << 8) | msg.data[1];
	sensor_val *sval = (sensor_val *)reading;
	switch (cfg.offset) {
	case SQ52205_READ_VOL_OFFSET:
		// 1.25 mV/LSB
		val = val * 1.25;
		break;
	case SQ52205_READ_CUR_OFFSET:
		// current_lsb mA/LSB
		val = val * init_arg->current_lsb;
		break;
	case SQ52205_READ_PWR_OFFSET:
		// (25 * current_lsb) mW/LSB
		val = val * (25 * init_arg->current_lsb);
		break;
	default:
		LOG_ERR("Offset not supported: 0x%x", cfg.offset);
		return SENSOR_PARAMETER_NOT_VALID;
		break;
	}

	sval->integer = (int16_t)(val) / 1000;
	sval->fraction = (int16_t)(val) - (sval->integer * 1000);
	return SENSOR_READ_SUCCESS;
}

uint8_t sq52205_init(uint8_t sensor_num)
{
	if (sensor_num > SENSOR_NUM_MAX) {
		LOG_ERR("input sensor number is invalid");
		return SENSOR_INIT_UNSPECIFIED_ERROR;
	}

	sq52205_init_arg *init_arg =
		(sq52205_init_arg *)sensor_config[sensor_config_index_map[sensor_num]].init_args;
	CHECK_NULL_ARG_WITH_RETURN(init_arg, SENSOR_INIT_UNSPECIFIED_ERROR);

	if (init_arg->is_init != true) {
		int ret = 0, retry = 5;
		uint16_t shunt_unit = 0;
		uint16_t calibration = 0;
		I2C_MSG msg = { 0 };

		sensor_cfg cfg = sensor_config[sensor_config_index_map[sensor_num]];

		// Shunt unit = 1 ohm
		shunt_unit = init_arg->r_shunt * 1000;
		msg.bus = cfg.port;
		msg.target_addr = cfg.target_addr;
		msg.tx_len = 3;
		msg.data[0] = SQ52205_CALIBRATION_OFFSET;

		// Calibration formula = ((2048 * shunt_lsb) / (current_lsb * r_shunt)
		calibration = (uint16_t)((2048 * (shunt_unit * SQ52205_SHUNT_LSB)) /
					 (init_arg->current_lsb * init_arg->r_shunt));
		msg.data[1] = (calibration >> 8) & 0xFF;
		msg.data[2] = calibration & 0xFF;

		ret = i2c_master_write(&msg, retry);
		if (ret != 0) {
			LOG_ERR("i2c write fail ret: %d", ret);
			return SENSOR_INIT_UNSPECIFIED_ERROR;
		}
		init_arg->is_init = true;
	}

	sensor_config[sensor_config_index_map[sensor_num]].read = sq52205_read;
	return SENSOR_INIT_SUCCESS;
}

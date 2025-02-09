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

#ifndef PLAT_PWRSEQ_H
#define PLAT_PWRSEQ_H

#include "hal_gpio.h"
#include "plat_gpio.h"

#define DEFAULT_POWER_ON_SEQ 0x00
#define DEFAULT_POWER_OFF_SEQ 0x0B
#define NUMBER_OF_POWER_ON_SEQ 0x0A
#define NUMBER_OF_POWER_OFF_SEQ 0x00
#define CHKPWR_DELAY_MSEC 100
#define DEV_RESET_DELAY_USEC 100

enum CONTROL_POWER_MODE {
	ENABLE_POWER_MODE = 0x00,
	DISABLE_POWER_MODE,
};

enum POWER_ON_STAGE {
	ASIC_POWER_ON_STAGE = 0x00,
	DIMM_POWER_ON_STAGE1,
	DIMM_POWER_ON_STAGE2,
	DIMM_POWER_ON_STAGE3,
	BOARD_POWER_ON_STAGE,
};

enum POWER_OFF_STAGE {
	DIMM_POWER_OFF_STAGE1 = 0x00,
	DIMM_POWER_OFF_STAGE2,
	DIMM_POWER_OFF_STAGE3,
	ASIC_POWER_OFF_STAGE1,
	ASIC_POWER_OFF_STAGE2,
	BOARD_POWER_OFF_STAGE,
};

enum CONTROL_POWER_SEQ_NUM_MAPPING {
	CONTROL_POWER_SEQ_01 = FM_P0V8_ASICA_EN,
	CONTROL_POWER_SEQ_02 = FM_P0V8_ASICD_EN,
	CONTROL_POWER_SEQ_03 = FM_P0V9_ASICA_EN,
	CONTROL_POWER_SEQ_04 = P1V8_ASIC_EN_R,
	CONTROL_POWER_SEQ_05 = PVPP_AB_EN_R,
	CONTROL_POWER_SEQ_06 = PVPP_CD_EN_R,
	CONTROL_POWER_SEQ_07 = FM_PVDDQ_AB_EN,
	CONTROL_POWER_SEQ_08 = FM_PVDDQ_CD_EN,
	CONTROL_POWER_SEQ_09 = PVTT_AB_EN_R,
	CONTROL_POWER_SEQ_10 = PVTT_CD_EN_R,
};

enum CHECK_POWER_SEQ_NUM_MAPPING {
	CHECK_POWER_SEQ_01 = P0V8_ASICA_PWRGD,
	CHECK_POWER_SEQ_02 = P0V8_ASICD_PWRGD,
	CHECK_POWER_SEQ_03 = P0V9_ASICA_PWRGD,
	CHECK_POWER_SEQ_04 = P1V8_ASIC_PG_R,
	CHECK_POWER_SEQ_05 = PVPP_AB_PG_R,
	CHECK_POWER_SEQ_06 = PVPP_CD_PG_R,
	CHECK_POWER_SEQ_07 = PWRGD_PVDDQ_AB,
	CHECK_POWER_SEQ_08 = PWRGD_PVDDQ_CD,
	CHECK_POWER_SEQ_09 = PVTT_AB_PG_R,
	CHECK_POWER_SEQ_10 = PVTT_CD_PG_R,
};

void set_MB_DC_status(uint8_t gpio_num);
void set_power_on_seq(uint8_t seq_num);
void set_power_off_seq(uint8_t seq_num);
void control_power_on_sequence();
void control_power_off_sequence();
void control_power_stage(uint8_t control_mode, uint8_t control_seq);
int check_power_stage(uint8_t check_mode, uint8_t check_seq);
bool power_on_handler(uint8_t initial_stage);
bool power_off_handler(uint8_t initial_stage);
void set_CXL_update_status(uint8_t set_status);
bool get_CXL_update_status();

#endif

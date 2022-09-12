#pragma once
#if !defined(OPM_H)
#	define OPM_H

#include <stdint.h>

//=============================================
//
// YM2151 wrapper around ymfm's API
//
// Copyright (c) 2021, Stephen Horn
// All Rights Reserved. License: 2-clause BSD
//
//---------------------------------------------
/*
#	define MAX_YM2151_VOICES (8)
#	define MAX_YM2151_SLOTS (MAX_YM2151_VOICES * 4)

#	define YM_R_L_FB_CONN_OFFSET 0x20
#	define YM_KC_OFFSET 0x28
#	define YM_KF_OFFSET 0x30
#	define YM_PMS_AMS_OFFSET 0x38

#	define YM_DT1_MUL_OFFSET 0x40
#	define YM_TL_OFFSET 0x60
#	define YM_KS_AR_OFFSET 0x80
#	define YM_A_D1R_OFFSET 0xA0
#	define YM_DT2_D2R_OFFSET 0xC0
#	define YM_D1L_RR_OFFSET 0xE0
*/
extern "C" {
void YM_render(int16_t *stream, uint32_t samples);
void YM_write(uint8_t reg, uint8_t val);
void YM_reset();
}


#endif

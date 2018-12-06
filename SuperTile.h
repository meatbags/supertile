/**
** SuperTile.h
**/

#pragma once
#ifndef SUPERTILE_H
#define SUPERTILE_H
#define PF_DEEP_COLOR_AWARE 1 // 16bpc flag for AE_Effect.h
#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEGP_SuiteHandler.h"
#include <cmath>

#ifdef AE_OS_WIN
#include <Windows.h>
#endif	

// Version
#define	MAJOR_VERSION 1
#define	MINOR_VERSION 0
#define	BUG_VERSION	0
#define	STAGE_VERSION PF_Stage_DEVELOP
#define	BUILD_VERSION 1

// Defaults
#define ST_SIZE_MIN 8
#define ST_SIZE_MAX 128
#define ST_SIZE_DFLT 16
#define ST_OFFSET_X_DFLT 0
#define ST_OFFSET_Y_DFLT 0

// Conversion
#define D2FIX(x) (((long)x)<<16)
#define FIX2D(x) (x / ((double)(1L << 16)))

enum {
	ST_INPUT = 0,
	ST_TARGET_LAYER_ID,
	ST_SIZE_SRC_ID,
	ST_SIZE_DEST_ID,
	ST_OFFSET_ID,
	ST_NUM_PARAMS
};

typedef struct {
	double h, s, l;
} HSL;

//PF_Pixel8 *getPixel8(PF_EffectWorld *inputP, int x,	int y) {
//	return ((PF_Pixel8 *)((char*)inputP->data + (y * inputP->rowbytes) + x * sizeof(PF_Pixel8)));
//}

double getHSLMag (HSL *a, HSL *b) {
	/* Vector Magnitude Between HSL */
	double dh = min(fabs(b->h - a->h), min(b->h + (1.0 - a->h), a->h + (1.0 - b->h)));
	double ds = b->s - a->s;
	double dl = b->l - a->l;
	return sqrt(dh * dh + ds * ds + dl * dl);
}

void getHSL (PF_Pixel8 *pixel, HSL *dest) {
	/* Convert RGB [0, 255] to HSL [0, 1] */
	double r = (double)pixel->red / 255.0;
	double g = (double)pixel->green / 255.0;
	double b = (double)pixel->blue / 255.0;
	double maxRGB = max(r, max(g, b));
	double minRGB = min(r, min(g, b));
	double d = maxRGB - minRGB;
	dest->l = (maxRGB + minRGB) / 2;
	if (d == 0) {
		dest->h = 0;
		dest->s = 0;
	} else {
		dest->s = (dest->l <= 0.5) ? (d / (maxRGB + minRGB)) : (d / (2 - maxRGB - minRGB));
		dest->h = (r == maxRGB) ? (g - b) / d : ((g == maxRGB) ? 2.0 + (b - r) / d : 4.0 + (r - g) / d);
		dest->h /= 6.0;
	}
}

#define PF_ADD_FLOAT_SLIDERXX(NAME, VALID_MIN, VALID_MAX, SLIDER_MIN, SLIDER_MAX, DFLT, PREC, DISP, FLAGS, UI_FLAGS, ID)    \
do {\
	AEFX_CLR_STRUCT(def); \
	PF_Err    priv_err = PF_Err_NONE; \
	def.param_type = PF_Param_FLOAT_SLIDER; \
	PF_STRCPY(def.name, (NAME)); \
	def.flags = (FLAGS); \
	def.ui_flags = (UI_FLAGS); \
	def.u.fs_d.valid_min = (VALID_MIN); \
	def.u.fs_d.slider_min = (SLIDER_MIN); \
	def.u.fs_d.valid_max = (VALID_MAX); \
	def.u.fs_d.slider_max = (SLIDER_MAX); \
	def.u.fs_d.value = (DFLT); \
	def.u.fs_d.dephault = (PF_FpShort)(def.u.fs_d.value); \
	def.u.fs_d.precision = (PREC); \
	def.u.fs_d.display_flags = (DISP); \
	def.u.fs_d.fs_flags = 0; \
	def.u.fs_d.curve_tolerance = AEFX_AUDIO_DEFAULT_CURVE_TOLERANCE; \
	def.uu.id = (ID); \
	if ((priv_err = PF_ADD_PARAM(in_data, -1, &def)) != PF_Err_NONE) return priv_err; \
} while (0)
//(WANT_PHASE) ? PF_FSliderFlag_WANT_PHASE : 0;

extern "C" {
	DllExport
		PF_Err
		EffectMain(
			PF_Cmd cmd,
			PF_InData *in_data,
			PF_OutData *out_data,
			PF_ParamDef *params[],
			PF_LayerDef	*output,
			void *extra
		);
}

#endif // SUPERTILE_H
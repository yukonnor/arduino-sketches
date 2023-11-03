/*
	This file is part of uVGA library.

	uVGA library is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uVGA library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uVGA library.  If not, see <http://www.gnu.org/licenses/>.

	Copyright (C) 2017 Eric PREVOTEAU

	Original Author: Eric PREVOTEAU <digital.or@gmail.com>
*/

#ifndef UVGA_VALID_SETTINGS_192MHZ_H
#define UVGA_VALID_SETTINGS_192MHZ_H

#include <uVGA.h>

#if F_CPU == 192000000
extern "C"
{


#ifdef UVGA_DEFAULT_REZ
#define UVGA_192M_602X300
#endif

// ======================================================
// valid video settings for 192MHz CPU
// * UVGA_192M_602X300  (800x600@56Hz, FB resolution: 602x300), very stable

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef UVGA_192M_602X300
// 192Mhz, 800x600@56Hz, FB resolution: 602x300
// very stable
#pragma message "192Mhz 602x300"

#define UVGA_HREZ 602
#define UVGA_VREZ 600
#define UVGA_RPTL 2
#define UVGA_TOP_MARGIN 0
#define UVGA_BOTTOM_MARGIN 0

uVGAmodeline modeline = {
   .pixel_clock = 36000000, //36MHz
   .hres = UVGA_HREZ,
   .hsync_start = 824,
   .hsync_end = 896,
   .htotal = 1024,
   .vres = UVGA_VREZ,
   .vsync_start = 601,
   .vsync_end = 603,
   .vtotal = 625,
	.top_margin = UVGA_TOP_MARGIN,
	.bottom_margin = UVGA_BOTTOM_MARGIN,
   .h_polarity = UVGA_POSITIVE_POLARITY,
   .v_polarity = UVGA_POSITIVE_POLARITY,
   .img_color_mode = UVGA_RGB332,
   .repeat_line = UVGA_RPTL,
   .horizontal_position_shift = 14,
	.pixel_h_stretch = UVGA_HSTRETCH_WIDE,
	.dma_settings = UVGA_DMA_AUTO,
	};
#endif

}
#endif
#endif

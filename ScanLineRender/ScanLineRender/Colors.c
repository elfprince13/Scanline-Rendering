/*
//  Colors.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 2/8/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Colors.h"
#include <stdbool.h>


#ifdef _EZ80
static Palette activePalette = (Palette)0xE30200;
#else
static HardwareColor activePalette[256];
#endif

Palette getPalette(){	return activePalette; }
void setPalette(const Palette p, size_t entries){
	size_t i;
	for(i = 0; i < entries; i++){
		activePalette[i] = p[i];
	}
}

#ifndef NDEBUG
const char * fmtColor(Color cc){
	/* extra unsafe */
	static char buf[64];
	const PaletteRef i = decodeColor(cc);
	const HardwareColor c = activePalette[i];
	const uint8_t r = (c >> 11) << 3,
	g = ((c >> 5) & 0x3f) << 2,
	b = (c & 0x1f) << 3;
	sprintf(buf,"java.awt.Color[r=%d,g=%d,b=%d]",r,g,b);
	return buf;
}
#endif

static PaletteConfig activeConfig = NULL;
static Color activeColor = 0;

void setPaletteConfig(const PaletteConfig c){	activeConfig = c; }
PaletteConfig getPaletteConfig(){	return activeConfig; }

void setActiveColor(Color c){ activeColor = c; }
Color getActiveColor(){ return activeColor; }

PaletteRef decodeColor(Color c){
	const int nineBitMask = 0x1ff,
	thirdBitMask = 0x8;
	int ofs = c & nineBitMask;
	bool compl;
	PaletteRef ret;
	if(ofs == 24 || ofs == 16){
		bool compl = ofs & thirdBitMask;
		ofs = (c >> 9) & nineBitMask;
		if(ofs == 24 || ofs == 16){
			compl = ofs & thirdBitMask;
			ofs = activeColor;
		}
	} else {
		compl = false;
	}
	ret = activeConfig[ofs][compl];
	return ret;
}
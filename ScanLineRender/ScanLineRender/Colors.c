/*
//  Colors.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 2/8/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Colors.h"
#include <stdbool.h>
#include <string.h>

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

PaletteConfig activeConfig = NULL;
void setPaletteConfig(const PaletteConfig c){	activeConfig = c; }
PaletteConfig getPaletteConfig(){	return activeConfig; }

#ifndef NDEBUG
const char * fmtColor(Color cc){
	/* extra unsafe */
	static char buf[64];
	const PaletteRef i = decodeColor(cc);
	const HardwareColor c = activePalette[i];
	const uint8_t r = (c >> 10) << 3,
	g = ((c >> 5) & 0x1f) << 3,
	b = (c & 0x1f) << 3;
	sprintf(buf,"java.awt.Color[r=%d,g=%d,b=%d]",r,g,b);
	return buf;
}
#endif

void setActiveColor(Color c){
	memcpy(activeConfig[16],activeConfig[c & 0x1ff],sizeof(PaletteConfigEntry));
	activeConfig[24][0] = activeConfig[16][1];
	activeConfig[24][1] = activeConfig[16][0];
}

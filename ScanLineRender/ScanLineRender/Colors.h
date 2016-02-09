/*
//  Colors.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 2/8/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Colors_h
#define Colors_h

#include <stdint.h>
#include <stddef.h>
#include "debugConfig.h"

/* for LDraw-style palette, must be at least 18 bits.
 * this is valid on x86 and z80.
 * We can also use the 6 upper bits to store status flags. */
typedef unsigned int Color;
typedef uint8_t PaletteRef;
typedef uint16_t HardwareColor;

typedef PaletteRef PaletteConfigEntry[2];
typedef PaletteConfigEntry* PaletteConfig;

typedef HardwareColor* Palette;

void setPaletteConfig(const PaletteConfig);
PaletteConfig getPaletteConfig(void);

void setActiveColor(Color);
Color getActiveColor(void);

PaletteRef decodeColor(Color);

void setPalette(const Palette, size_t entries);
Palette getPalette(void);

#ifndef NDEBUG
const char * fmtColor(Color c);
#endif


#endif /* Colors_h */

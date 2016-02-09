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

/* 
 * Lower 9 bits for LDraw index
 * Upper 7 bits for status flags.
 */
typedef uint16_t Color;

typedef uint8_t PaletteRef;
typedef uint16_t HardwareColor;

typedef PaletteRef PaletteConfigEntry[2];
typedef PaletteConfigEntry* PaletteConfig;

typedef HardwareColor* Palette;

extern PaletteConfig activeConfig;
void setPaletteConfig(const PaletteConfig);
PaletteConfig getPaletteConfig(void);
/*PaletteRef decodeColor(Color);*/
#define decodeColor(c) activeConfig[((c) & 0x1ff)][0]

void setActiveColor(Color);
Color getActiveColor(void);


void setPalette(const Palette, size_t entries);
Palette getPalette(void);

#ifndef NDEBUG
const char * fmtColor(Color c);
#endif


#endif /* Colors_h */

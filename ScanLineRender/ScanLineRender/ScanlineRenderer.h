/*
//  ScanlineRenderer.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/27/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef ScanlineRenderer_h
#define ScanlineRenderer_h

#include "Primitive.h"
#include "Projection.h"

void render(Color *raster, int16_t width, int16_t height, const Primitive *geometry, size_t geomCount, const Projection *p);

#endif /* ScanlineRenderer_h */
/*
//  main.cpp
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef _EZ80
#include <netpbm/ppm.h>
#endif

#include "ScanlineRenderer.h"
#include <string.h>


static HardwareColor palette[] = {0x4400,0x7e01,0x35ab,0x6004,0x10d6,0xd06,0x7e81,0x157,0xc,0x530e,0x400f,0x211,0x6377,0x5e87,0x5f15,0x7a16,0x35d9,0x6318,0x1404,0x1f1b,0x431c,0x6b5a,0x4300,0x0,0xd6,0x3821,0x7e23,0x4924,0x7b25,0x7b26,0x5a27,0x5587,0x4a2e,0x5540,0x5531,0x7333,0x7f39,0x739b,0x673c,0x5840,0x842,0x43,0x3d47,0x72b9,0x344a,0x6a4d,0x7e63,0x2154,0x7e55,0x7b56,0x3657,0x5b5a,0x5b5c,0x735d,0x7ee7,0x6665,0x6461,0x1462,0xc63,0x4564,0x5165,0x4611,0x3169,0x316a,0x11e7,0x316c,0x6d92,0x4170,0x6347,0x6593,0x3b74,0x2a76,0x2669,0x422e,0x6f94,0xa7a,0x5f7b,0x296a,0x637e,0x2a95,0x5c80,0x6ec0,0x6982,0x2d6b,0x1084,0xd87,0x2b88,0x6eec,0x318c,0x6d,0x7f92,0x4e93,0x94,0x5295,0x4296,0x5299,0x48ef,0x739c,0x5fa1,0x2ca2,0x259b,0x1c6,0xca6,0x50ab,0x4cac,0x10ad,0xdae,0x5ef7,0x4e6b,0x6fb4,0x7fc7,0x52b6,0x2ab9,0x66c0,0x57bd,0x45c0,0x4ead,0x4cb,0x65c5,0x18c6,0x5ec7,0x1880,0x79cb,0x2ccc,0x1ece,0x2dd0,0x55d1,0x1cd2,0x49d3,0x65d4,0x69ce,0x3ad6,0x29d7,0x7b33,0x2e7a,0x7bde,0x63e0,0x466b,0x2ce4,0x6ea6,0x62e7,0x11e8,0x59ea,0x55eb,0x7eec,0x300c,0x3def,0x4e95,0x7a23,0x5294,0x52f5,0x73f6,0x55f7,0x7ef7,0x35ad,0x2afa,0x6bc0,0x7fff};
static PaletteRef ldConfig[512][2] = {{41,83},{7,119},{64,119},{11,119},{56,119},{129,119},{138,58},{91,119},{2,119},{52,119},{72,119},{79,119},{122,119},{48,119},{29,119},{157,119},{146,119},{12,119},{90,119},{35,119},{38,119},{151,9},{10,119},{4,58},{146,119},{26,119},{96,119},{98,119},{32,119},{43,119},{152,119},{53,119},{23,119},{92,8},{141,85},{86,116},{56,0},{66,103},{1,80},{78,94},{63,40},{71,106},{136,22},{114,131},{128,123},{48,126},{28,115},{157,17},{23,23},{23,23},{23,23},{23,23},{95,18},{23,23},{81,140},{23,23},{23,23},{148,60},{23,23},{23,23},{62,57},{50,105},{124,101},{34,44},{102,83},{28,119},{113,115},{157,17},{133,119},{69,119},{99,83},{93,119},{154,119},{134,119},{70,119},{23,31},{65,61},{36,119},{49,119},{157,17},{111,119},{137,119},{59,119},{102,119},{118,119},{127,58},{42,119},{2,119},{23,23},{100,119},{23,23},{23,23},{45,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{153,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{47,119},{23,23},{16,119},{23,23},{66,104},{68,119},{23,23},{157,17},{51,119},{23,23},{74,119},{23,23},{23,23},{23,23},{23,23},{144,119},{23,23},{23,23},{23,23},{145,18},{23,23},{23,23},{23,61},{23,139},{27,119},{147,119},{23,23},{132,119},{23,23},{23,23},{23,23},{23,23},{87,119},{23,23},{23,23},{23,23},{23,23},{23,23},{77,119},{23,23},{107,119},{97,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{30,119},{61,119},{23,23},{23,23},{23,23},{135,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{54,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{20,119},{23,23},{23,23},{23,23},{39,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{110,119},{23,23},{23,23},{23,23},{23,23},{23,23},{155,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{84,83},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{117,58},{24,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{5,83},{23,23},{23,23},{23,23},{23,23},{23,23},{14,73},{23,23},{23,23},{55,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{121,83},{23,23},{23,23},{23,23},{23,23},{112,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{25,119},{75,119},{19,119},{76,119},{3,119},{23,23},{109,119},{23,23},{23,23},{23,23},{108,119},{23,23},{23,23},{23,23},{13,120},{130,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{82,119},{15,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{46,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{67,119},{23,23},{17,119},{23,23},{23,23},{150,119},{125,119},{23,23},{23,23},{23,23},{97,149},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{89,83},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{10,119},{142,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{6,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{33,119},{23,23},{23,23},{23,23},{23,23},{23,23},{156,119},{23,23},{23,23},{88,83},{21,119},{143,119},{149,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{37,119},{61,119},{23,23},{23,23},{23,23},{23,23},{23,23},{23,23},{157,119}};

typedef enum {
	RED = 4,
	MAGENTA = 26,
	GREEN = 2,
	ORANGE = 25,
	YELLOW = 14,
	PINK = 13,
	BLUE = 1,
	WHITE = 15
} ColorCodesPalette;

typedef struct {
	float x;
	float y;
	float zFrac;
	float scale;
} DrawCoords;

static DrawCoords drawCoords = {170, 40, 0.7, 100};

void viewF(const Point *p, Point *o, const DrawCoords *offset);
void viewF(const Point *p, Point *o, const DrawCoords *offset){
	const float x = offset->scale * p->x,
	 y = offset->scale * p->y,
	z = offset->scale * p->z,
	zOff = z * offset->zFrac;
	INIT_POINT(*o, offset->x + x + zOff, offset->y + y + zOff, z);
}

const Transformation viewProj = {(TransformationF)(&viewF),&drawCoords};



int main(int argc, const char * argv[]) {
	const static Point cubePointsSrc[8] =
	{{0,0,0},{0,0,1},{0,1,0},{0,1,1},
		{1,0,0},{1,0,1},{1,1,0},{1,1,1}};
	static Point cubePoints[8];
	static Point* cubeEdges[3][4][2] = {
		{
			{&cubePoints[0],&cubePoints[1]},
			{&cubePoints[2],&cubePoints[3]},
			{&cubePoints[4],&cubePoints[5]},
			{&cubePoints[6],&cubePoints[7]}},
		{
			{&cubePoints[0],&cubePoints[2]},
			{&cubePoints[1],&cubePoints[3]},
			{&cubePoints[4],&cubePoints[6]},
			{&cubePoints[5],&cubePoints[7]}},
		{
			{&cubePoints[0],&cubePoints[4]},
			{&cubePoints[1],&cubePoints[5]},
			{&cubePoints[2],&cubePoints[6]},
			{&cubePoints[3],&cubePoints[7]}}};
	
	static Point* cubeFaces[6][5] = {
		{&cubePoints[0],&cubePoints[1],&cubePoints[3],&cubePoints[2],&cubePoints[0]},
		{&cubePoints[7],&cubePoints[6],&cubePoints[4],&cubePoints[5],&cubePoints[7]},
		{&cubePoints[5],&cubePoints[1],&cubePoints[3],&cubePoints[7],&cubePoints[5]},
		{&cubePoints[4],&cubePoints[6],&cubePoints[2],&cubePoints[0],&cubePoints[4]},
		{&cubePoints[0],&cubePoints[1],&cubePoints[5],&cubePoints[4],&cubePoints[0]},
		{&cubePoints[2],&cubePoints[3],&cubePoints[7],&cubePoints[6],&cubePoints[2]},
	};
	
	
	static Primitive cube[] = {
		{cubeFaces[0], 4, RED},
		{cubeFaces[1], 4, MAGENTA},
		{cubeFaces[2], 4, GREEN},
		{cubeFaces[3], 4, ORANGE},
		{cubeFaces[4], 4, YELLOW},
		{cubeFaces[5], 4, PINK},
		{cubeEdges[0][0], 1, BLUE},
		{cubeEdges[0][1], 1, BLUE},
		{cubeEdges[0][2], 1, BLUE},
		{cubeEdges[0][3], 1, BLUE},
		{cubeEdges[1][0], 1, BLUE},
		{cubeEdges[1][1], 1, BLUE},
		{cubeEdges[1][2], 1, BLUE},
		{cubeEdges[1][3], 1, BLUE},
		{cubeEdges[2][0], 1, BLUE},
		{cubeEdges[2][1], 1, BLUE},
		{cubeEdges[2][2], 1, BLUE},
		{cubeEdges[2][3], 1, BLUE}
	};
	
	const int numLines = 240;
	const int lineWidth = 320;
	const size_t rasterByteCount = numLines * lineWidth * sizeof(PaletteRef);
	PaletteRef *const raster = (PaletteRef*)
#ifdef _EZ80
	0xD40000
#else
	SafeMalloc(rasterByteCount)

#endif
		;
	rb_red_blk_tree* buckets = NULL;
#ifdef _EZ80
	*(uint8_t*)0xE30018 = 7;
#else
	pm_proginit(&argc, argv);
#endif
	setPaletteConfig(ldConfig);
	setPalette(palette, sizeof(palette) / sizeof(HardwareColor));
	
	transformData(&viewProj, cubePointsSrc, cubePoints, 8);
	buckets = bucketPrims(buckets, numLines, cube, sizeof(cube) / sizeof(Primitive));
	
	memset(raster, decodeColor(WHITE), rasterByteCount);
	render(raster, lineWidth, numLines, buckets);
	buckets = teardownBuckets(buckets, numLines);
	
	waitKey();
	cleanUp();
	
#ifndef _EZ80
	{
		pixel **const ppm_raster = ppm_allocarray(lineWidth, numLines);
		size_t x,y;
		FILE *fp;
		
		for(y = 0; y < numLines; y++){
			for(x = 0; x < lineWidth; x++){
				const PaletteRef i = raster[y * lineWidth + x];
				const HardwareColor rgb = getPalette()[i];
				const uint8_t r = (rgb >> 10) << 3,
				g = ((rgb >> 5) & 0x1f) << 3,
				b = (rgb & 0x1f) << 3;
				ppm_raster[y][x].r = r;
				ppm_raster[y][x].g = g;
				ppm_raster[y][x].b = b;
			}
		}
		
		if((fp = fopen("out.ppm", "wb"))){
			ppm_writeppm(fp, ppm_raster, lineWidth, numLines, 255, 0);
			fclose(fp);
		}
		
		ppm_freearray(ppm_raster, numLines);
	}
	free(raster);
#endif
    return 0;
}

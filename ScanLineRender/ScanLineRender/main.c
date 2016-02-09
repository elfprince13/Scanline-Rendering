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


static HardwareColor palette[] = {0x8800,0xfc01,0xc004,0x844e,0x0,0x1b07,0xc6d7,0xc,0xa60e,0x800f,0x411,0x9d13,0x114,0x9d15,0x8516,0xc618,0x2804,0x3e1b,0xe71c,0xcb85,0xb920,0xfd21,0xd322,0x1a26,0xfe79,0x5728,0xab07,0x632c,0xff32,0xabcb,0x8c31,0xdb32,0xdf34,0xa535,0x8600,0xa534,0xffdf,0x196,0x5535,0xbf41,0x5942,0xfc43,0x9244,0xf436,0x1946,0xb447,0x600c,0xa14b,0x98b,0x9d4d,0x944e,0xb3ca,0xfce3,0x3d8e,0xa556,0xa539,0x5559,0xce5c,0xaf5d,0xfdc7,0xcce5,0x7061,0xf463,0xf665,0xf666,0xe71b,0x6b6b,0x6b6d,0xc687,0xbd87,0xaa71,0xcd80,0xf673,0xdf74,0x214d,0xe579,0xaaa0,0x53b7,0xdd80,0x1082,0x83,0x7596,0x3186,0x7a87,0x55fa,0xddec,0x688a,0xf38b,0xd48d,0x3100,0x5b90,0xab91,0x9393,0xcb94,0x2196,0xcb13,0xbe35,0xb69c,0xd69a,0xf79e,0x8ba0,0x4b1b,0x3a6,0x2104,0x861c,0xc5c7,0x59ac,0xd3ae,0xdd66,0x39b2,0xe673,0x42b4,0xfcb5,0xf6b6,0x2b7,0x994c,0x6bb9,0xb6ba,0xe6bd,0xc8c1,0xd780,0x8ccb,0x59c4,0x23c7,0x23c8,0x62c9,0x52ca,0x5acb,0xfdcc,0x91cf,0x9ccb,0xff87,0x1b4e,0x54d6,0xabd7,0x14da,0x6cb7,0xb080,0xc7e0,0x28e2,0x18e3,0x8ae4,0xa2e5,0xa514,0x4ce9,0x62ea,0xbd27,0x62ec,0xed,0x7bef,0x82f0,0x76f4,0xa5f5,0xe7f6,0xbdf7,0xfdd7,0x5cfa,0xbefb,0xc6fe,0xffff};
static PaletteRef ldConfig[512][2] = {
	{80,127},{114,82},{123,82},{10,82},	{119,82},{93,82},{122,140},{11,82},
	{66,82},{97,82},{144,82},{38,82},	{87,82},{112,82},{64,82},{159,82},
	{149,82},{6,82},{28,82},{110,82},	{57,82},	{153,8},	{9,82},	{94,140},
	{149,82},	{41,82},	{129,82},	{39,82},	{50,82},	{75,82},	{134,82},	{118,82},
	{4,82},	{12,7},	{124,5},	{25,49},	{119,0},	{31,47},	{1,20},	{158,14},
	{145,79},	{133,132},	{138,34},	{58,81},	{92,106},	{112,91},	{63,100},	{159,15},
	{4,4},	{4,4},	{4,4},	{4,4},	{55,16},	{4,4},	{78,105},	{4,4},
	{4,4},	{62,142},	{4,4},	{4,4},	{125,139},	{136,74},	{53,102},	{70,86},
	{44,127},	{63,82},	{71,100},	{159,15},	{72,82},	{95,82},	{40,127},	{33,82},
	{67,82},	{156,82},	{151,82},	{4,26},	{147,30},	{24,82},	{113,82},	{159,15},
	{54,82},	{121,82},	{141,82},	{44,82},	{19,82},	{109,140},	{83,82},	{66,82},
	{4,4},	{101,82},	{4,4},	{4,4},	{88,82},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{155,82},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{111,82},	{4,4},
	{116,82},	{4,4},	{31,115},	{68,82},	{4,4},	{159,15},	{117,82},	{4,4},
	{32,82},	{4,4},	{4,4},	{4,4},	{4,4},	{128,82},	{4,4},	{4,4},
	{4,4},	{46,16},	{4,4},	{4,4},	{4,30},	{4,108},	{42,82},	{13,82},
	{4,4},	{77,82},	{4,4},	{4,4},	{4,4},	{4,4},	{85,82},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{126,82},	{4,4},	{154,82},	{18,82},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{45,82},	{30,82},	{4,4},	{4,4},	{4,4},	{99,82},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{59,82},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{104,82},	{4,4},	{4,4},	{4,4},
	{137,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{131,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{84,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{103,127},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{48,140},	{37,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{23,127},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{96,3},	{4,4},
	{4,4},	{60,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{89,127},	{4,4},	{4,4},	{4,4},
	{4,4},	{56,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{61,82},	{135,82},	{17,82},	{157,82},	{2,82},	{4,4},	{73,82},	{4,4},
	{4,4},	{4,4},	{130,82},	{4,4},	{4,4},	{4,4},	{146,69},	{107,82},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{22,82},	{43,82},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{52,82},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{150,82},	{4,4},	{15,82},
	{4,4},	{4,4},	{152,82},	{90,82},	{4,4},	{4,4},	{4,4},	{18,35},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{148,127},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{9,82},	{51,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{21,82},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{4,4},	{4,4},	{76,82},	{4,4},	{4,4},	{4,4},
	{4,4},	{4,4},	{120,82},	{4,4},	{4,4},	{27,127},	{98,82},	{29,82},
	{143,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{65,82},
	{30,82},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{4,4},	{36,82}};

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
#ifdef _EZ80
	_OS( GetKey() );
	cleanUp();
#else
	{
		pixel **const ppm_raster = ppm_allocarray(lineWidth, numLines);
		size_t x,y;
		FILE *fp;
		
		for(y = 0; y < numLines; y++){
			for(x = 0; x < lineWidth; x++){
				const PaletteRef i = raster[y * lineWidth + x];
				const HardwareColor rgb = getPalette()[i];
				const uint8_t r = (rgb >> 11) << 3,
				g = ((rgb >> 5) & 0x3f) << 2,
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

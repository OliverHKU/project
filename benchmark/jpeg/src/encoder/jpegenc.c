/******************************************************************************
** This file is part of the jpegant project.
**
** Copyright (C) 2009-2013 Vladimir Antonenko
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License,
** or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.
******************************************************************************/

#include "arch.h"
#include "jpegenc.h"


/* tables from JPEG standard
const unsigned char qtable_std_lum[64] =
{
	16, 11, 10, 16, 24, 40, 51, 61,
	12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56,
	14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68,109,103, 77,
	24, 35, 55, 64, 81,104,113, 92,
	49, 64, 78, 87,103,121,120,101,
	72, 92, 95, 98,112,100,103, 99
};

const unsigned char qtable_std_chrom[64] =
{
	17, 18, 24, 47, 99, 99, 99, 99,
	18, 21, 26, 66, 99, 99, 99, 99,
	24, 26, 56, 99, 99, 99, 99, 99,
	47, 66, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99
};

// Windows Paint tables
const unsigned char qtable_paint_lum[64] =
{
	 8,  6,  5,  8, 12, 20, 26, 31, 
	 6,  6,  7, 10, 13, 29, 30, 28,
	 7,  7,  8, 12, 20, 29, 35, 28,
	 7,  9, 11, 15, 26, 44, 40, 31,
	 9, 11, 19, 28, 34, 55, 52, 39,
	12, 18, 28, 32, 41, 52, 57, 46,
	25, 32, 39, 44, 52, 61, 60, 51,
	36, 46, 48, 49, 56, 50, 52, 50
};

const unsigned char qtable_paint_chrom[64] =
{
	 9,  9, 12, 24, 50, 50, 50, 50,
	 9, 11, 13, 33, 50, 50, 50, 50,
	12, 13, 28, 50, 50, 50, 50, 50,
	24, 33, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50
};
*/


#define QTAB_SCALE	10

// as you can see I use Paint tables
static const unsigned char qtable_0_lum[64] =
{
	 8,  6,  5,  8, 12, 20, 26, 31, 
	 6,  6,  7, 10, 13, 29, 30, 28,
	 7,  7,  8, 12, 20, 29, 35, 28,
	 7,  9, 11, 15, 26, 44, 40, 31,
	 9, 11, 19, 28, 34, 55, 52, 39,
	12, 18, 28, 32, 41, 52, 57, 46,
	25, 32, 39, 44, 52, 61, 60, 51,
	36, 46, 48, 49, 56, 50, 52, 50
};

static const unsigned char qtable_0_chrom[64] =
{
	 9,  9, 12, 24, 50, 50, 50, 50,
	 9, 11, 13, 33, 50, 50, 50, 50,
	12, 13, 28, 50, 50, 50, 50, 50,
	24, 33, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50,
	50, 50, 50, 50, 50, 50, 50, 50
};

// (1 << QTAB_SCALE)/qtable_0_lum[][]
static const unsigned char qtable_lum[64] =
{
	128,171,205,128, 85, 51, 39, 33,
	171,171,146,102, 79, 35, 34, 37,
	146,146,128, 85, 51, 35, 29, 37,
	146,114, 93, 68, 39, 23, 26, 33,
	114, 93, 54, 37, 30, 19, 20, 26,
	 85, 57, 37, 32, 25, 20, 18, 22,
	 41, 32, 26, 23, 20, 17, 17, 20,
	 28, 22, 21, 21, 18, 20, 20, 20
};

// (1 << QTAB_SCALE)/qtable_0_chrom[][]
static const unsigned char qtable_chrom[64] =
{
	114,114, 85, 43, 20, 20, 20, 20,
	114, 93, 79, 31, 20, 20, 20, 20,
	 85, 79, 37, 20, 20, 20, 20, 20,
	 43, 31, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20
};

// zig-zag table
const unsigned char zig[64] =
{
	 0,
	 1, 8,
	16, 9, 2, 
	 3,10,17,24,
	32,25,18,11, 4,
	 5,12,19,26,33,40,
	48,41,34,27,20,13, 6,
	 7,14,21,28,35,42,49,56,
	57,50,43,36,29,22,15,
	23,30,37,44,51,58,
	59,52,45,38,31,
	39,46,53,60,
	61,54,47,
	55,62,
	63
};

static const unsigned char std_dc_luminance_nrcodes[16] =
{
	0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0
};
static const unsigned char std_dc_luminance_values[12] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

static const unsigned char std_dc_chrominance_nrcodes[16] =
{
	0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0
};
static const unsigned char std_dc_chrominance_values[12] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

static const unsigned char std_ac_luminance_nrcodes[16] =
{
	0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,0x7d
};

static const unsigned char std_ac_luminance_values[162] =
{
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

static const unsigned char std_ac_chrominance_nrcodes[16] =
{
	0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,0x77
};

static const unsigned char std_ac_chrominance_values[162] =
{
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

// Huffman Y DC code length
static const unsigned char HYDClen[12] =
{
	0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x05,
	0x06, 0x07, 0x08, 0x09
};

// Huffman Cb/Cr DC code length
static const unsigned char HCDClen[12] =
{
	0x02, 0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b
};

// Huffman Y DC code bits
static const unsigned short HYDCbits[12] =
{
	0x0000, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x000e, 0x001e,
	0x003e, 0x007e, 0x00fe, 0x01fe
};

// Huffman Cb/Cr DC code bits
static const unsigned short HCDCbits[12] =
{
	0x0000, 0x0001, 0x0002, 0x0006, 0x000e, 0x001e, 0x003e, 0x007e,
	0x00fe, 0x01fe, 0x03fe, 0x07fe
};


// Huffman Y AC code length
static const unsigned char HYAClen[16][12] =
{
	{0x04, 0x02, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x0a, 0x10, 0x10, 0x00},
	{0x00, 0x04, 0x05, 0x07, 0x09, 0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x05, 0x08, 0x0a, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x06, 0x09, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x06, 0x0a, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x07, 0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x07, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x08, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x0f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x0a, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x0a, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}
};

// Huffman Y AC code bits
static const unsigned short HYACbits[16][12] =
{
	{0xFFFA, 0xFFF0, 0xFFF1, 0xFFF4, 0xFFFB, 0xFFFA, 0xFFF8, 0xFFF8, 0xFFF6, 0xFF82, 0xFF83, 0x0000},
	{0x0000, 0xFFFC, 0xFFFB, 0xFFF9, 0xFFF6, 0xFFF6, 0xFF84, 0xFF85, 0xFF86, 0xFF87, 0xFF88, 0x0000},
	{0x0000, 0xFFFC, 0xFFF9, 0xFFF7, 0xFFF4, 0xFF89, 0xFF8A, 0xFF8B, 0xFF8C, 0xFF8D, 0xFF8E, 0x0000},
	{0x0000, 0xFFFA, 0xFFF7, 0xFFF5, 0xFF8F, 0xFF90, 0xFF91, 0xFF92, 0xFF93, 0xFF94, 0xFF95, 0x0000},
	{0x0000, 0xFFFB, 0xFFF8, 0xFF96, 0xFF97, 0xFF98, 0xFF99, 0xFF9A, 0xFF9B, 0xFF9C, 0xFF9D, 0x0000},
	{0x0000, 0xFFFA, 0xFFF7, 0xFF9E, 0xFF9F, 0xFFA0, 0xFFA1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0x0000},
	{0x0000, 0xFFFB, 0xFFF6, 0xFFA6, 0xFFA7, 0xFFA8, 0xFFA9, 0xFFAA, 0xFFAB, 0xFFAC, 0xFFAD, 0x0000},
	{0x0000, 0xFFFA, 0xFFF7, 0xFFAE, 0xFFAF, 0xFFB0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5, 0x0000},
	{0x0000, 0xFFF8, 0xFFC0, 0xFFB6, 0xFFB7, 0xFFB8, 0xFFB9, 0xFFBA, 0xFFBB, 0xFFBC, 0xFFBD, 0x0000},
	{0x0000, 0xFFF9, 0xFFBE, 0xFFBF, 0xFFC0, 0xFFC1, 0xFFC2, 0xFFC3, 0xFFC4, 0xFFC5, 0xFFC6, 0x0000},
	{0x0000, 0xFFFA, 0xFFC7, 0xFFC8, 0xFFC9, 0xFFCA, 0xFFCB, 0xFFCC, 0xFFCD, 0xFFCE, 0xFFCF, 0x0000},
	{0x0000, 0xFFF9, 0xFFD0, 0xFFD1, 0xFFD2, 0xFFD3, 0xFFD4, 0xFFD5, 0xFFD6, 0xFFD7, 0xFFD8, 0x0000},
	{0x0000, 0xFFFA, 0xFFD9, 0xFFDA, 0xFFDB, 0xFFDC, 0xFFDD, 0xFFDE, 0xFFDF, 0xFFE0, 0xFFE1, 0x0000},
	{0x0000, 0xFFF8, 0xFFE2, 0xFFE3, 0xFFE4, 0xFFE5, 0xFFE6, 0xFFE7, 0xFFE8, 0xFFE9, 0xFFEA, 0x0000},
	{0x0000, 0xFFEB, 0xFFEC, 0xFFED, 0xFFEE, 0xFFEF, 0xFFF0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0x0000},
	{0xFFF9, 0xFFF5, 0xFFF6, 0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE, 0x0000}
};

// Huffman Cb/Cr AC code length
static const unsigned char HCAClen[16][12] =
{
	{0x02, 0x02, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x09, 0x0a, 0x0c, 0x00},
	{0x00, 0x04, 0x06, 0x08, 0x09, 0x0b, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x05, 0x08, 0x0a, 0x0c, 0x0f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x05, 0x08, 0x0a, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x06, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x06, 0x0a, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x07, 0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x07, 0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x0b, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x00, 0x0e, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},
	{0x0a, 0x0f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}
};

// Huffman Cb/Cr AC code bits
static const unsigned short HCACbits[16][12] =
{
	{0x0000, 0x0001, 0x0004, 0x000a, 0x0018, 0x0019, 0x0038, 0x0078, 0x01f4, 0x03f6, 0x0ff4, 0x0000},
	{0x0000, 0x000b, 0x0039, 0x00f6, 0x01f5, 0x07f6, 0x0ff5, 0xff88, 0xff89, 0xff8a, 0xff8b, 0x0000},
	{0x0000, 0x001a, 0x00f7, 0x03f7, 0x0ff6, 0x7fc2, 0xff8c, 0xff8d, 0xff8e, 0xff8f, 0xff90, 0x0000},
	{0x0000, 0x001b, 0x00f8, 0x03f8, 0x0ff7, 0xff91, 0xff92, 0xff93, 0xff94, 0xff95, 0xff96, 0x0000},
	{0x0000, 0x003a, 0x01f6, 0xff97, 0xff98, 0xff99, 0xff9a, 0xff9b, 0xff9c, 0xff9d, 0xff9e, 0x0000},
	{0x0000, 0x003b, 0x03f9, 0xff9f, 0xffa0, 0xffa1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0xFFA6, 0x0000},
	{0x0000, 0x0079, 0x07f7, 0xffa7, 0xffa8, 0xffa9, 0xffaa, 0xffab, 0xFFAc, 0xFFAf, 0xFFAe, 0x0000},
	{0x0000, 0x007a, 0x07f8, 0xffaf, 0xffb0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5, 0xFFB6, 0x0000},
	{0x0000, 0x00f9, 0xffb7, 0xFFB8, 0xFFB9, 0xFFBa, 0xFFBb, 0xFFBc, 0xFFBd, 0xFFBe, 0xFFBf, 0x0000},
	{0x0000, 0x01f7, 0xffc0, 0xffc1, 0xFFC2, 0xFFC3, 0xFFC4, 0xFFC5, 0xFFC6, 0xFFC7, 0xFFC8, 0x0000},
	{0x0000, 0x01f8, 0xffc9, 0xFFCa, 0xFFCb, 0xFFCc, 0xFFCd, 0xFFCe, 0xFFCf, 0xFFd0, 0xFFd1, 0x0000},
	{0x0000, 0x01f9, 0xFFD2, 0xFFD3, 0xFFD4, 0xFFD5, 0xFFD6, 0xFFD7, 0xFFD8, 0xFFD9, 0xFFDa, 0x0000},
	{0x0000, 0x01fa, 0xFFDb, 0xFFDc, 0xFFDd, 0xFFDe, 0xFFDf, 0xFFe0, 0xFFe1, 0xFFe2, 0xFFe3, 0x0000},
	{0x0000, 0x07f9, 0xFFE4, 0xFFE5, 0xFFE6, 0xFFE7, 0xFFE8, 0xFFE9, 0xFFEa, 0xFFEb, 0xFFEc, 0x0000},
	{0x0000, 0x3fe0, 0xffed, 0xFFEe, 0xFFEf, 0xFFf0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0xFFF5, 0x0000},
	{0x03fa, 0x7fc3, 0xFFF6, 0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE, 0x0000}
};

huffman_t huffman_ctx[3] =
{
	{HYAClen, HYACbits, HYDClen, HYDCbits, qtable_lum,   0}, // Y
	{HCAClen, HCACbits, HCDClen, HCDCbits, qtable_chrom, 0}, // Cb
	{HCAClen, HCACbits, HCDClen, HCDCbits, qtable_chrom, 0}, // Cr
};

typedef struct bitbuffer_s
{
	unsigned buf;
	unsigned n;
}
bitbuffer_t;

static bitbuffer_t bitbuf;


/******************************************************************************
**  quantize
**  --------------------------------------------------------------------------
**  DCT coeficient quantization.
**  To avoid division function uses quantization coefs amplified by 2^QTAB_SCALE
**  and then shifts the product by QTAB_SCALE bits to the right.
**  To make this operation a bit faster some tricks are used but it is just
**  returns round(data[i]/qt0[i]).
**  
**  ARGUMENTS:
**      data    - DCT freq value;
**      qt      - quantization value ( (1 << QTAB_SCALE)/qt0 );
**
**  RETURN: quantized value.
******************************************************************************/
static short quantize(const short data, const unsigned char qt)
{
	return (data*qt - (data>>15) + ((1<<(QTAB_SCALE-1))-1)) >> QTAB_SCALE;
}

/******************************************************************************
**  quantization
**  --------------------------------------------------------------------------
**  DCT coeficients quantization to discard weak high frequencies.
**  This function processes 8x8 DCT blocks inplace.
**  
**  ARGUMENTS:
**      data    - 8x8 DCT freq block;
**      qt      - 8x8 quantization table;
**
**  RETURN: -
******************************************************************************/
static void quantization(short data[], const unsigned char qt[])
{
	unsigned i;

	for (i = 0; i < 64; i+=4)
	{
		data[i]   = quantize(data[i],   qt[i]);
		data[i+1] = quantize(data[i+1], qt[i+1]);
		data[i+2] = quantize(data[i+2], qt[i+2]);
		data[i+3] = quantize(data[i+3], qt[i+3]);
	}
}

void quantization_lum(short data[64])
{
	quantization((short*)data, qtable_lum);
}

void quantization_chrom(short data[64])
{
	quantization((short*)data, qtable_chrom);
}

static void iquantization(short data[64], const unsigned char qt[64])
{
	unsigned i;

	// de-quantization
	for (i = 0; i < 64; i++)
	{
		int p = data[i];
		unsigned char q = qt[i];

		data[i] = p*q;
	}
}

void iquantization_lum(short data[64])
{
	iquantization(data, qtable_0_lum);
}

void iquantization_chrom(short data[64])
{
	iquantization(data, qtable_0_chrom);
}

/******************************************************************************
**  writebyte
**  --------------------------------------------------------------------------
**  This function writes byte into output buffer
**  and flushes the buffer if it is full.
**  
**  unsigned char jpgbuff - global output buffer;
**  unsigned      jpgn    - the buffer index;
**  
**  ARGUMENTS: b - byte;
**
**  RETURN: -
******************************************************************************/

// code-stream output counter
static unsigned jpgn = 0;
// code-stream output buffer, adjust its size if you need
static unsigned char jpgbuff[256];

static void writebyte(const unsigned char b)
{
	jpgbuff[jpgn++] = b;

	if (jpgn == sizeof(jpgbuff)) {
		jpgn = 0;
		write_jpeg(jpgbuff, sizeof(jpgbuff));
	}
}

static void writeword(const unsigned short w)
{
	writebyte(w >> 8); writebyte((const unsigned char)w);
}

static void write_APP0info(void)
{
	writeword(0xFFE0); //marker
	writeword(16);     //length
	writebyte('J');
	writebyte('F');
	writebyte('I');
	writebyte('F');
	writebyte(0);
	writebyte(1);//versionhi
	writebyte(1);//versionlo
	writebyte(0);//xyunits
	writeword(1);//xdensity
	writeword(1);//ydensity
	writebyte(0);//thumbnwidth
	writebyte(0);//thumbnheight
}

// should set width and height before writing
static void write_SOF0info(const short height, const short width)
{
	writeword(0xFFC0);	//marker
	writeword(17);		//length
	writebyte(8);		//precision
	writeword(height);	//height
	writeword(width);	//width
	writebyte(3);		//nrofcomponents
	writebyte(1);		//IdY
	writebyte(0x22);	//HVY, 4:2:0 subsampling
	writebyte(0);		//QTY
	writebyte(2);		//IdCb
	writebyte(0x11);	//HVCb
	writebyte(1);		//QTCb
	writebyte(3);		//IdCr
	writebyte(0x11);	//HVCr
	writebyte(1);		//QTCr
}

static void write_SOSinfo(void)
{
	writeword(0xFFDA);	//marker
	writeword(12);		//length
	writebyte(3);		//nrofcomponents
	writebyte(1);		//IdY
	writebyte(0);		//HTY
	writebyte(2);		//IdCb
	writebyte(0x11);	//HTCb
	writebyte(3);		//IdCr
	writebyte(0x11);	//HTCr
	writebyte(0);		//Ss
	writebyte(0x3F);	//Se
	writebyte(0);		//Bf
}

static void write_DQTinfo(void)
{
	unsigned i;
	
	writeword(0xFFDB);
	writeword(132);
	writebyte(0);

	for (i = 0; i < 64; i++) 
		writebyte(qtable_0_lum[zig[i]]); // zig-zag order

	writebyte(1);

	for (i = 0; i < 64; i++) 
		writebyte(qtable_0_chrom[zig[i]]); // zig-zag order
}

static void write_DHTinfo(void)
{
	unsigned i;
	
	writeword(0xFFC4); // marker
	writeword(0x01A2); // length

	writebyte(0); // HTYDCinfo
	for (i = 0; i < 16; i++) 
		writebyte(std_dc_luminance_nrcodes[i]);
	for (i = 0; i < 12; i++) 
		writebyte(std_dc_luminance_values[i]);

	writebyte(0x10); // HTYACinfo
	for (i = 0; i < 16; i++)
		writebyte(std_ac_luminance_nrcodes[i]);
	for (i = 0; i < 162; i++)
		writebyte(std_ac_luminance_values[i]);
	

	writebyte(1); // HTCbDCinfo
	for (i = 0; i < 16; i++)
		writebyte(std_dc_chrominance_nrcodes[i]);
	for (i = 0; i < 12; i++)
		writebyte(std_dc_chrominance_values[i]);
	
	writebyte(0x11); // HTCbACinfo = 0x11;
	for (i = 0; i < 16; i++)
		writebyte(std_ac_chrominance_nrcodes[i]);
	for (i = 0; i < 162; i++)
		writebyte(std_ac_chrominance_values[i]);
}

/******************************************************************************
**  writebits
**  --------------------------------------------------------------------------
**  Write bits into bit-buffer.
**  If the number of bits exceeds 16 the result is unpredictable.
**  
**  ARGUMENTS:
**      pb      - pointer to bit-buffer context;
**      bits    - bits to write;
**      nbits   - number of bits to write, 0-16;
**
**  RETURN: -
******************************************************************************/
static void writebits(bitbuffer_t *const pbb, unsigned bits, unsigned nbits)
{
	// shift old bits to the left, add new to the right
	pbb->buf = (pbb->buf << nbits) | (bits & ((1 << nbits)-1));

	// new number of bits
	nbits += pbb->n;

	// flush whole bytes
	while (nbits >= 8) {
		unsigned char b;

		nbits -= 8;
		b = pbb->buf >> nbits;

		writebyte(b);

		if (b == 0xFF)
			writebyte(0); // add 0x00 after 0xFF
	}

	// remember how many bits is remained
	pbb->n = nbits;
}

/******************************************************************************
**  flushbits
**  --------------------------------------------------------------------------
**  Flush bits into bit-buffer.
**  If there is not an integer number of bytes in bit-buffer - add 1-s
**  and write these bytes.
**  
**  ARGUMENTS:
**      pbb     - pointer to bit-buffer context;
**
**  RETURN: -
******************************************************************************/
static void flushbits(bitbuffer_t *pbb)
{
	if (pbb->n)
		writebits(pbb, 0xFF, 8 - pbb->n);
}

/******************************************************************************
**  huffman_bits
**  --------------------------------------------------------------------------
**  Converst amplitude into the representation suitable for Jpeg encoder -
**  so called "Baseline Entropy Coding Symbol-2" or variable length integer VLI
**  Unsignificant higher bits will be dropped later.
**  
**  ARGUMENTS:
**      value    - DCT amplitude;
**
**  RETURN: huffman bits
******************************************************************************/
static unsigned huffman_bits(const short value)
{
	return value + (value >> 15);
}

/******************************************************************************
**  huffman_magnitude
**  --------------------------------------------------------------------------
**  Calculates magnitude of an VLI integer - the number of bits that are enough
**  to represent given value.
**  
**  ARGUMENTS:
**      value    - DCT amplitude;
**
**  RETURN: magnitude
******************************************************************************/
static unsigned huffman_magnitude(const short value)
{
	unsigned x = (value < 0)? -value: value;
	unsigned m = 0;

	while (x >> m) ++m;

	return m;
}

/******************************************************************************
**  huffman_start
**  --------------------------------------------------------------------------
**  Starts Huffman encoding by writing Start of Image (SOI) and all headers.
**  Sets image size in Start of File (SOF) header before writing it.
**  
**  ARGUMENTS:
**      height  - image height (pixels);
**      width   - image width (pixels);
**
**  RETURN: -
******************************************************************************/
void huffman_start(short height, short width)
{
	writeword(0xFFD8); // SOI
	write_APP0info();
	write_DQTinfo();
	write_SOF0info(height, width);
	write_DHTinfo();
	write_SOSinfo();

	huffman_ctx[2].dc = 
	huffman_ctx[1].dc = 
	huffman_ctx[0].dc = 0;
}

/******************************************************************************
**  huffman_stop
**  --------------------------------------------------------------------------
**  Finalize Huffman encoding by flushing bit-buffer, writing End of Image (EOI)
**  into output buffer and flusing this buffer.
**  
**  ARGUMENTS: -
**
**  RETURN: -
******************************************************************************/
void huffman_stop(void)
{
	flushbits(&bitbuf);
	writeword(0xFFD9); // EOI - End of Image
	write_jpeg(jpgbuff, jpgn);
	jpgn = 0;
}

/******************************************************************************
**  huffman_encode
**  --------------------------------------------------------------------------
**  Quantize and Encode a 8x8 DCT block by JPEG Huffman lossless coding.
**  This function writes encoded bit-stream into bit-buffer.
**  
**  ARGUMENTS:
**      ctx     - pointer to encoder context;
**      data    - pointer to 8x8 DCT block;
**
**  RETURN: -
******************************************************************************/
void huffman_encode(huffman_t *const ctx, const short data[])
{
	unsigned magn, bits;
	unsigned zerorun, i;
	short    diff;

	short    dc = quantize(data[0], ctx->qtable[0]);
	// difference between old and new DC
	diff = dc - ctx->dc;
	ctx->dc = dc;

	bits = huffman_bits(diff); // VLI
	magn = huffman_magnitude(diff); // VLI length

	// encode VLI length
	writebits(&bitbuf, ctx->hdcbit[magn], ctx->hdclen[magn]);
	// encode VLI itself
	writebits(&bitbuf, bits, magn);

	for (zerorun = 0, i = 1; i < 64; i++)
	{
		const short ac = quantize(data[zig[i]], ctx->qtable[zig[i]]);

		if (ac == 0) {
			zerorun++; continue;
		}

		while (zerorun >= 16) {
			zerorun -= 16;
			// ZRL
			writebits(&bitbuf, ctx->hacbit[15][0], ctx->haclen[15][0]);
		}

		bits = huffman_bits(ac);
		magn = huffman_magnitude(ac);

		// encode VLI length + number of preceded zero coefs
		writebits(&bitbuf, ctx->hacbit[zerorun][magn], ctx->haclen[zerorun][magn]);
		// encode VLI itself
		writebits(&bitbuf, bits, magn);

		zerorun = 0;
	}

	if (zerorun) { // EOB - End Of Block
		writebits(&bitbuf, ctx->hacbit[0][0], ctx->haclen[0][0]);
	}
}

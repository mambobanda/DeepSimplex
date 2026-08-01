/*
 * The Mailo Today Solutions Software License
 *
 * Copyright 2020 Mailo Today Solutions.
 *
 * No Permission is granted, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software.
 */
 /**
	* @file ...h
	*
	* @author Mambo Banda
	*
	*/

#pragma once

namespace ds {
	namespace type {
		enum {
			SIZE_T = 0,
			INT  = 1,
			UINT = 2,
			FLOAT  = 3,
			DOUBLE = 4,
			STRING = 5,
			BOOL = 6,
			CHAR = 7,
			VEC2 = 8,
			VEC3 = 9,
			VEC4 = 10,
			MAT3 = 11,
			MAT4 = 12,
			IVEC2 = 13,
			IVEC3 = 14,
			IVEC4 = 15,
			DVEC2 = 16,
			DVEC3 = 17,
			DVEC4 = 18,
			DMAT3 = 19,
			DMAT4 = 20,
			QUAT  = 21,
			ANY   = 22,
		};
	}
}
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

#pragma once

#include <vector>
#include "libtess/tesselator.h"

namespace ds {

	class Tesselator {
	private:
		struct vt {
			float a;
			float b;
			float c;
		};
	private:
		TESStesselator *tess;
		size_t vertexCount;

		vt *vts;
		size_t vt_count;
	public:

		Tesselator();
		virtual ~Tesselator();

		void init();
		void destroy();

		void newPoly(size_t);
		void commitPoly();

		void addVertex(size_t index, float a, float b, float c);

		void tesselate();

		size_t numberOfVertices();
		size_t numberOfTriangles();

		float triVert(size_t tri, size_t vert, int index);
		float vertValue(size_t inx);
		int   faceIndex(size_t inx);
	};
};
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

#include "DsTesselator.h"

namespace ds {

	Tesselator::Tesselator() {
		tess = nullptr;
		vts = nullptr;
		vt_count = 0;
	}

	Tesselator::~Tesselator() {

	}

	void Tesselator::init() {
		tess = tessNewTess(NULL);
	}

	void Tesselator::destroy() {

		if (vts != nullptr) {
			delete[] vts;
			vts = nullptr;
		}

		tessDeleteTess(tess);
	}

	void Tesselator::newPoly(size_t sz) {
		if (vts != nullptr) {
			delete[] vts;
			vts = nullptr;
		}

		vts = new vt[sz];
		vt_count = sz;
	}

	void Tesselator::commitPoly() {
		if (vts != nullptr) {
			tessAddContour(tess, 3, vts, sizeof(float) * 3, (int)vt_count);
		}
	}

	void Tesselator::addVertex(size_t index, float a, float b, float c) {
		vt vtt = { a, b, c };
		vts[index] = vtt;
	}

	size_t Tesselator::numberOfVertices() {
		return tessGetVertexCount(tess);
	}

	size_t Tesselator::numberOfTriangles() {
		return tessGetElementCount(tess);
	}

	float Tesselator::triVert(size_t tri, size_t vert, int index) {

		const float* verts = tessGetVertices(tess);
		const int* elems = tessGetElements(tess);
		const int* p = &elems[tri * 3];

		if ( p[vert] != TESS_UNDEF ) {
			return verts[ p[vert] * 3 + index ];
		}
		else {
			return 0.f;
		}
	}

	float Tesselator::vertValue(size_t inx) {
		const float* verts = tessGetVertices(tess);
		return verts[inx];
	}

	int Tesselator::faceIndex(size_t inx) {
		const int* elems = tessGetElements(tess);
		return elems[inx];
	}

	void Tesselator::tesselate() {
		tessTesselate(tess, TESS_WINDING_POSITIVE, TESS_POLYGONS, 3, 3, NULL);
	}
}
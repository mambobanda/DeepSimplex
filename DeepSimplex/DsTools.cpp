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


#include "DsTools.h"

namespace dstools {

	// Get Face Verts
	ds::Vert** faceVertices(ds::Face *f) {
		return f->vertices();
	}

	int faceVerticesNum(ds::Face *f) {
		return f->verticesCount();
	}

	// Create standard mesh
	ds::Mesh *newStandardMesh() {

		ds::Mesh *msh = new ds::Mesh;

		msh->addProperty("uvs", ds::PropertyType::VERTEX, ds::type::VEC2);
		msh->addProperty("smooth_normals", ds::PropertyType::VERTEX, ds::type::VEC3);
		msh->addProperty("face_normals", ds::PropertyType::FACE, ds::type::VEC3);

		return msh;
	}
}
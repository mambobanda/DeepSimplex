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

#include <queue>
#include "DsMesh.h"
#include "DsType.h"


 // Data Structure based on "Adjacency and incidence framework: a data structure for efficient and fast management of multiresolution meshes.(2009)" paper.

namespace ds {

	class MeshQuery {
	public:

		// Direct Queries
		static EdgeList vertexIncidentEdges(Vert *);
		static VertexList edgeAdjacentVertices(Edge *);
		static FaceList edgeIncidentFaces(Edge *);
		static EdgeList faceAdjacentEdges(Face *);

		// Indirect Queries
		static FaceList vertexIncidentFaces(Vert *);
		static VertexList faceBoundingVertices(Face *);
		static VertexList vertexNeighboringVertices(Vert *);
		static EdgeList edgeNeighboringEdges(Edge *);
		static FaceList faceNeighboringFaces(Face *);

		// Other Queries
		static FaceList faceLoop(FaceList faces);
		static FaceList connectedFaces(Mesh *mesh, FaceList faces);
	};
}
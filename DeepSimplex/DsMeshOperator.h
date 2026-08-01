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

#include <string>
#include <unordered_map>
#include <map>
#include <glm/geometric.hpp>
#include "DsMesh.h"
#include "DsMeshQuery.h"
#include "DsMeshOperatorDependencies.h"
#include "DsUtil.h"
#include "DsMeshBuilder.h"
#include "DsTesselator.h"

namespace ds {

	class MeshOperator {
	private:
		Mesh *_mesh;

	private:
		std::vector<Face*> facesFromIndices(std::vector<size_t> faces);
		std::vector<Edge*> edgesFromIndices(std::vector<size_t> edges);
	public:

		MeshOperator();
		MeshOperator(Mesh *mesh);

		virtual ~MeshOperator();

		void setMesh(Mesh *msh);
		Mesh *mesh();

	public:

		// Calculate smooth normals
		void calcSmoothNormals(std::string propertyName);

		// Calculate face normals
		void calcFaceNormals(std::string propertyName);

		// Delete Vertices
		void deleteVertices(std::vector<size_t> verts);

		// Delete Edges
		void deleteEdges(std::vector<size_t> edges);

		// Delete Faces
		void deleteFaces(std::vector<size_t> faces);
		
		// Extrude Faces
		std::vector<Face*> extrudeFaces(std::vector<size_t> faces);

		// Triangulate Mesh
		std::vector<Face*> triangulateFace(std::vector<size_t> faces);

		// Split Edge
		Vert *splitEdge(size_t edge_idx);

		// Connect Vertices
		std::vector<Edge*> connectVertices(std::vector<size_t> verts);

		// Bevel Edges
		std::vector<Edge*> bevelEdges(std::vector<size_t> edges, float offset);

		// Bevel Edges
		std::vector<Edge*> bevelEdges00(std::vector<size_t> edges, float offset);

		// Bevel Edges
		std::vector<Edge*> bevelEdges01(std::vector<size_t> edges, float offset);
		
		// Bevel Edges
		std::vector<Edge*> bevelEdges02(std::vector<size_t> edges, float offset);
	};
}

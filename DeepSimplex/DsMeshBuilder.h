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

#include <unordered_map>
#include <vector>
#include <string>
#include "DsMesh.h"

namespace ds {

	class MeshBuilder {
	private:
		Mesh *mesh;

	private:
		std::unordered_map<std::string, Edge*> edgeMap;

		std::string edgeKey(size_t a, size_t b);
		Edge *hasEdge(size_t a, size_t b);
		Edge *createEdge(size_t a, size_t b);

	public:
		
		MeshBuilder();
		~MeshBuilder();

		void setMesh(Mesh *msh);
		Mesh *getMesh();

		void addVertices(std::vector<glm::vec3> verts);
		Face *addFace(std::vector<size_t> indices);

	};

}
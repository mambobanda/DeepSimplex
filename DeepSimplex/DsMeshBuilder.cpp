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

#include <iostream>
#include "DsMeshBuilder.h"

namespace ds {

	MeshBuilder::MeshBuilder() {
		mesh = nullptr;
	}

	MeshBuilder::~MeshBuilder() {

	}

	void MeshBuilder::setMesh(Mesh *msh) {
		mesh = msh;
	}

	Mesh *MeshBuilder::getMesh() {
		return mesh;
	}

	std::string MeshBuilder::edgeKey(size_t a, size_t b) {
		return std::to_string(a) + ":" + std::to_string(b);
	}

	Edge *MeshBuilder::hasEdge(size_t a, size_t b) {
		auto key00 = edgeKey(a, b);
		auto key01 = edgeKey(b, a);

		auto itr00 = edgeMap.find(key00);
		auto itr01 = edgeMap.find(key01);

		if (itr00 != edgeMap.end()) {
			return itr00->second;
		}if (itr01 != edgeMap.end()) {
			return itr01->second;
		}

		return nullptr;
	}

	Edge *MeshBuilder::createEdge(size_t a, size_t b) {
		auto v1 = mesh->vertices()[a];
		auto v2 = mesh->vertices()[b];

		Edge *edg = mesh->addEdge(v1, v2);

		std::pair<std::string, Edge*> p(edgeKey(a,b), edg);
		edgeMap.insert(p);

		return edg;
	}

	void MeshBuilder::addVertices(std::vector<glm::vec3> verts) {

		if (mesh != nullptr) {
			for (auto v : verts) {
				mesh->addVertex(v);
			}
		}

	}

	Face *MeshBuilder::addFace(std::vector<size_t> verts) {

		if (mesh != nullptr) {

			if (verts.size() >= 3) {

				size_t last = verts[0];
				EdgeList ls;
				std::vector<Vert*> vertLst;
				
				for (size_t index=1; index < verts.size(); index++) {

					Edge *edge = nullptr;
					size_t current = verts[index];
					edge = hasEdge(last, verts[index]);

					if (edge == nullptr) edge = createEdge(last, verts[index]);

					vertLst.push_back(mesh->vertices()[last]);
					ls.push_back(edge);

					last = verts[index];
		
				}

				Edge *edge = nullptr;
				size_t current = verts[0];
				edge = hasEdge(last, verts[0]);

				if (edge == nullptr) edge = createEdge(last, verts[0]);

				vertLst.push_back(mesh->vertices()[last]);
				ls.push_back(edge);

				return mesh->addFace(ls, vertLst);

			}
		}

		return nullptr;
	}

}
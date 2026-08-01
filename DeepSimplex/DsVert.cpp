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

#include "DsEdge.h"
#include "DsVert.h"

 // Data Structure based on "Adjacency and incidence framework: a data structure for efficient and fast management of multiresolution meshes.(2009)" paper.

namespace ds {

	Vert::Vert() {
		pt = glm::vec3(0.f);
		index = 0;
	}

	Vert::~Vert() {

	}

	size_t Vert::getUID() {
		return uid;
	}

	void Vert::changeUID(size_t ud) {
		uid = ud;
	}

	size_t Vert::getIndex() {
		return index;
	}

	void Vert::changeIndex(size_t inx) {
		index = inx;
	}

	EdgeList &Vert::incidentEdges() {
		return li;
	}

	void Vert::removeEdge(Edge *ege) {
		auto it = li.begin();

		while (it != li.end()) {
			auto e = *it;
			if (e->getUID() == ege->getUID()) {
				li.erase(it);
				return;
			}
			it++;
		}
	}

	glm::vec3 &Vert::position() {
		return pt;
	}

	bool Vert::isPropertyNull(std::string name) {
		return pmap->isPropertyNull(name, getUID(), ds::PropertyType::VERTEX);
	}

	bool Vert::operator==(Vert& vt) {
		return uid == vt.getUID();
	}

	bool Vert::operator!=(Vert& vt) {
		return uid != vt.getUID();
	}

	bool Vert::operator==(Vert* vt) {
		return uid == vt->getUID();
	}

	bool Vert::operator!=(Vert* vt) {
		return uid != vt->getUID();
	}

	void Vert::setPropertyMap(PropertyMap *p) {
		pmap = p;
	}

	
}
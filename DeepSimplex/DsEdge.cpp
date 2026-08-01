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

#include "DsFace.h"
#include "DsEdge.h"


 // Data Structure based on "Adjacency and incidence framework: a data structure for efficient and fast management of multiresolution meshes.(2009)" paper.

namespace ds {

	Edge::Edge() {

	}

	Edge::~Edge() {

	}

	size_t Edge::getUID() {
		return uid;
	}

	void Edge::changeUID(size_t ud) {
		uid = ud;
	}

	size_t Edge::getIndex() {
		return index;
	}

	void Edge::changeIndex(size_t ix) {
		index = ix;
	}

	Vert *Edge::v00() {
		return vertex00;
	}

	Vert *Edge::v01() {
		return vertex01;
	}

	void Edge::setV00(Vert *vtx) {
		vertex00 = vtx;
	}

	void Edge::setV01(Vert *vtx) {
		vertex01 = vtx;
	}

	FaceList &Edge::incidentFaces() {
		return li;
	}

	void Edge::removeFace(Face *fce) {
		auto it = li.begin();

		while (it != li.end()) {
			auto f = *it;
			if (f->getUID() == fce->getUID()) {
				li.erase(it);
				return;
			}
			it++;
		}
	}

	bool Edge::isPropertyNull(std::string name) {
		return pmap->isPropertyNull(name, getUID(), ds::PropertyType::EDGE);
	}

	void Edge::setPropertyMap(PropertyMap *pm) {
		pmap = pm;
	}

	bool Edge::operator==(Edge& ed) {
		return uid == ed.getUID();
	}

	bool Edge::operator!=(Edge& ed) {
		return uid != ed.getUID();
	}

	bool Edge::operator==(Edge* ed) {
		return uid == ed->getUID();
	}

	bool Edge::operator!=(Edge* ed) {
		return uid != ed->getUID();
	}

}
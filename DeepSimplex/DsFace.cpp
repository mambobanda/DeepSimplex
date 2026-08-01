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
#include "DsVert.h"

 // Data Structure based on "Adjacency and incidence framework: a data structure for efficient and fast management of multiresolution meshes.(2009)" paper.

namespace ds {

	Face::Face() {
		vs = nullptr;
		vsCount = 0;
		surf_id = 0;
	}

	Face::~Face() {
		delete[] vs;
	}

	size_t Face::getUID() {
		return uid;
	}

	void Face::changeUID(size_t ud) {
		uid = ud;
	}

	size_t Face::getSurfID() {
		return surf_id;
	}

	void Face::setSurfID(size_t sid) {
		surf_id = sid;
	}

	size_t Face::getIndex() {
		return index;
	}

	void Face::changeIndex(size_t ix) {
		index = ix;
	}

	EdgeList &Face::adjacentEdges() {
		return la;
	}

	Vert **Face::vertices() {
		return vs;
	}

	std::vector<glm::vec2> &Face::uvs() {
		return _uvs;
	}

	std::unordered_map<size_t, glm::vec2> Face::mapUVS() {
		std::unordered_map<size_t, glm::vec2> output;

		for (size_t i = 0; i < verticesCount(); i++) {
			output[vs[i]->getIndex()] = _uvs[i];
		}

		return output;
	}

	int Face::verticesCount() {
		return vsCount;
	}

	void Face::newVerts(int count) {
		delete[] vs;
		vs = new Vert*[count];
		vsCount = count;
	}

	void Face::setPropertyMap(PropertyMap *pm) {
		pmap = pm;
	}

	bool Face::isPropertyNull(std::string name) {
		return pmap->isPropertyNull(name, getUID(), ds::PropertyType::FACE);
	}

	bool Face::operator==(Face& fc) {
		return uid == fc.getUID();
	}

	bool Face::operator!=(Face& fc) {
		return uid != fc.getUID();
	}

	bool Face::operator==(Face* fc) {
		return uid == fc->getUID();
	}

	bool Face::operator!=(Face* fc) {
		return uid != fc->getUID();
	}

}
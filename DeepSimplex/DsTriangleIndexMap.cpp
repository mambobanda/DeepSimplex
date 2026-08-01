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
 /**
	* @file ....cpp
	*
	* @author Mambo Banda
	*
	*/

#include "DsTriangleIndexMap.h"

namespace ds {

	TriangleIndexMap::TriangleIndexMap() {
		_size = 0;
	}

	TriangleIndexMap::~TriangleIndexMap() {

	}

	void TriangleIndexMap::addIndex(size_t inx, std::pair<size_t, Vert**> indices) {
		_indexMap[inx] = indices;
		_size += indices.first;
	}

	void TriangleIndexMap::removeIndex(size_t inx) {
		auto itr = _indexMap.find(inx);

		if (itr != _indexMap.end()) {
			_size -= itr->second.first;
			delete[] itr->second.second;
			_indexMap.erase(itr);
		}
	}

	std::pair<size_t, Vert**> TriangleIndexMap::getIndex(size_t inx) {
		auto itr = _indexMap.find(inx);

		if (itr != _indexMap.end()) {
			return itr->second;
		}

		return std::pair<size_t, Vert**>(0, nullptr);
	}

	bool TriangleIndexMap::hasIndex(size_t inx) {
		auto itr = _indexMap.find(inx);

		if (itr != _indexMap.end())  return true;

		return false;
	}

	size_t TriangleIndexMap::triangleCount() {
		return _size;
	}

	unsigned int *TriangleIndexMap::toIndexArray() {
		if (_size == 0) return nullptr;

		auto output = new unsigned int[_size * 3];

		size_t counter = 0;
		for (auto p: _indexMap) {
			
			for (size_t i = 0; i < p.second.first; i++) {
				size_t index = i * 3;

				output[counter]	  = (unsigned int)p.second.second[index]->getIndex();
				output[counter + 1] = (unsigned int)p.second.second[index + 1]->getIndex();
				output[counter + 2] = (unsigned int)p.second.second[index + 2]->getIndex();

				counter += 3;
			}
		}

		return output;
	}

	void TriangleIndexMap::toIndexArray(std::vector<size_t> &in, std::vector<unsigned int> &output) {
		if (_size == 0) return;

		size_t counter = 0;
		for (auto ele : in) {
			auto p = _indexMap[ele];
			for (size_t i = 0; i < p.first; i++) {
				size_t index = i * 3;

				output.push_back((unsigned int)p.second[index]->getIndex());
				output.push_back((unsigned int)p.second[index + 1]->getIndex());
				output.push_back((unsigned int)p.second[index + 2]->getIndex());
			}
		}

		return;
	}


	std::pair<size_t, Vert**> TriangleIndexMap::triangulateFace(Face *f) {
		std::pair<size_t, Vert**> output(0, nullptr);
		size_t triCount;
		Vert** vertList = nullptr;

		if (f->verticesCount() == 3) {
			triCount = 1;
			vertList = new Vert*[3];
			output = std::pair<size_t, Vert**>(triCount, vertList);

			vertList[0] = f->vertices()[0];
			vertList[1] = f->vertices()[1];
			vertList[2] = f->vertices()[2];
		}
		else {

			std::vector<glm::vec3> newVerts;
			std::vector<Edge*> existingEdges;
			std::unordered_map<size_t, Vert*> originalVerts;

			ds::Tesselator tess;
			tess.init();
			tess.newPoly(f->verticesCount());
			for (int j = 0; j < f->verticesCount(); j++) {
				tess.addVertex(j,
					f->vertices()[j]->position().x,
					f->vertices()[j]->position().y,
					f->vertices()[j]->position().z);
			}

			tess.commitPoly();
			tess.tesselate();

			if (f->verticesCount() <= 2) {
				return std::pair<size_t, Vert**>(0, nullptr);
			}

			triCount = tess.numberOfTriangles();
			vertList = new Vert*[triCount * 3];
			output = std::pair<size_t, Vert**>(triCount, vertList);

			for (int i = 0; i < f->verticesCount(); i++) {
				size_t current = 3 * i;
				glm::vec3 v(tess.vertValue(current), tess.vertValue(current + 1), tess.vertValue(current + 2));
				newVerts.push_back(v);
			}

			for (size_t i = 0; i < newVerts.size(); i++) {
				for (int j = 0; j < f->verticesCount(); j++) {
					if (newVerts[i] == f->vertices()[j]->position()) {

						auto vv = f->vertices()[j];
						std::pair<size_t, Vert*> p(i, vv);
						originalVerts.insert(p);
					}
				}
			}

			if (originalVerts.size() <= 2) {
				delete[] vertList;
				return std::pair<size_t, Vert**>(0, nullptr);
			}

			for (auto e : f->adjacentEdges()) {
				existingEdges.push_back(e);
			}

			for (int i = 0; i < tess.numberOfTriangles(); i++) {
				std::vector<size_t> indices;

				size_t current = 3 * i;
				int inx00 = tess.faceIndex(current);
				int inx01 = tess.faceIndex(current + 1);
				int inx02 = tess.faceIndex(current + 2);

				auto a = originalVerts.find(inx00)->second;
				auto b = originalVerts.find(inx01)->second;
				auto c = originalVerts.find(inx02)->second;

				vertList[current] = a;
				vertList[current + 1] = b;
				vertList[current + 2] = c;
			}

			tess.destroy();
		}

		return output;
	}
	
}
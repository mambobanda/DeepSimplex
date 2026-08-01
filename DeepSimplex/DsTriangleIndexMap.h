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
	* @file ...h
	*
	* @author Mambo Banda
	*
	*/

#pragma once

#include <unordered_map>
#include "DsVert.h"
#include "DsFace.h"
#include "DsTesselator.h"

namespace ds {

	class TriangleIndexMap {
	private:
		std::unordered_map<size_t, std::pair<size_t, Vert**>> _indexMap;
		size_t _size;
	public:
		TriangleIndexMap();
		~TriangleIndexMap();

		void addIndex(size_t, std::pair<size_t, Vert**>);
		void removeIndex(size_t);
		std::pair<size_t, Vert**> getIndex(size_t);

		std::pair<size_t, Vert**> triangulateFace(Face *f);

		size_t triangleCount();
		unsigned int *toIndexArray();
		void toIndexArray(std::vector<size_t> &in, std::vector<unsigned int> &out);

		bool hasIndex(size_t);
	};

}
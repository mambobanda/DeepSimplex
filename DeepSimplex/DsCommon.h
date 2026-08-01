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

#include <list>
#include <vector>
#include <string>
#include <locale>
#include <sstream>
#include <glm\glm.hpp>
#include <glm\geometric.hpp>

namespace ds {

	class Mesh;
	class Face;
	class Edge;
	class Vert;

	class Channel;
	class PointCloud;
	class PointCloudObserver;

	using FaceList = std::vector<Face*>;
	using EdgeList = std::vector<Edge*>;
	using VertexList = std::vector<Vert*>;

	// String to lower case;
	inline std::string to_lower(std::string str) {
		std::stringstream ss;
		std::locale loc;
		for (auto c : str) {
			ss << std::tolower(c, loc);
		}

		return ss.str();
	}
}
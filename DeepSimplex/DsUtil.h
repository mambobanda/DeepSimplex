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
#include <sstream>
#include <GTE/Mathematics/DistLineLine.h>
#include <GTE/Mathematics/DistRayRay.h>
#include <GTE/Mathematics/DistSegmentSegment.h>
#include "DsMeshQuery.h"
#include "DsEdge.h"
#include "DsFace.h"
#include "DsVert.h"

namespace ds {
	namespace util {

		class ProxyEdge {
		private:
			ds::Edge *_reference;
			ds::Vert *_a;
			ds::Vert *_b;

		public:
			ProxyEdge();
			ProxyEdge(ds::Edge *e, ds::Vert *a, ds::Vert* b);

			void setReference(ds::Edge *ref);
			ds::Edge *reference();

			void setA(ds::Vert *aa);
			Vert *a();

			void setB(ds::Vert *bb);
			Vert *b();
		};

		//
		void removeEdge(size_t, std::vector<Edge*> &ls);

		// Check if vertices belong to the same edge
		std::pair<bool, bool> edgeIdentical(ds::Vert *a, ds::Vert *b, ds::Edge*);

		// Check if a list of edges has edge with both input vertices
		std::pair<bool, bool> hasEdge(Vert *a, Vert *b, std::vector<Edge*> el, int &index);

		// Collect face points as oriented edge pairs
		std::vector<std::pair<Vert*, Vert*>> faceVerticesAsPairs(Face *);

		// Sort face edges (clockwise) into oriented edge 
		std::vector<util::ProxyEdge> sortFaceEdges(Face *);

		// Check if a point belongs to only one edge in a list of edges
		bool isEndPoint(Vert *v, std::vector<Edge*> el);

		// Replace Proxy Vert
		bool replaceProxyEdgeVertex(size_t uid, std::vector<ProxyEdge> &ls, Vert*);

		// Find face with edge in direction <from -> to> following winding order
		Face *adjacentFaceFromTo(Edge*,  Vert *from, Vert *to);

		// Find face with edge proxy
		util::ProxyEdge adjacentFaceProxy(Face* fce, Edge *edg);

		// Sorted Adjacent Point Edges
		std::vector<util::ProxyEdge> pointAdjacentEdgesCCW(Edge *startEdge, Vert *a, Vert *b);

		// Face shared by two edges
		Face *sharedFace(Edge *e00, Edge *e01);

		// Face shared by two edges
		Edge *sharedEdge(Face *f00, Face *f01);

		// Distance between two edges
		std::pair<float, glm::vec3> distance(Edge *a, Edge*b);
	}
}
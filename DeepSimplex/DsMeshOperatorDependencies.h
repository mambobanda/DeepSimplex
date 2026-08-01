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

#include "DsUtil.h"
#include "DsMeshQuery.h"
#include "DsVert.h"
#include "DsEdge.h"
#include "DsFace.h"

namespace ds {
	// Edge Bevel Dependencies
	struct BeveledEdge {
		Edge *originalEdge;
		Edge *hardEdgeA;
		Edge *hardEdgeB;
		Face *originalAdjacentFace_a;
		Face *originalAdjacentFace_b;
		bool v00_Midpoint;
		bool v01_Midpoint;
		Vert *v00a;
		Vert *v00a_old;
		Vert *v01a;
		Vert *v01a_old;
		Vert *v00b;
		Vert *v00b_old;
		Vert *v01b;
		Vert *v01b_old;

		std::vector<util::ProxyEdge> newFace_a;
		std::vector<util::ProxyEdge> newFace_b;
	};

	struct BeveledTempFaceMerge {
		size_t originalUID;
		std::vector<util::ProxyEdge> newedges;
		std::vector<std::pair<bool,bool>> overideFlags;
		std::vector<util::ProxyEdge> refereceEdges;
	};

	struct BevelTempFace {
		size_t original_uid;
		EdgeList ls;
		std::vector<Vert*> vts;
	};
};

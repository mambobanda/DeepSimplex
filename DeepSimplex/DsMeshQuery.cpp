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

#include "DsMeshOperator.h"

namespace ds {

	// Direct Queries
	EdgeList MeshQuery::vertexIncidentEdges(Vert *vt) {
		std::unordered_map<size_t, Edge*> map;
		EdgeList ls;

		for (auto e : vt->incidentEdges()) {
			std::pair<size_t, Edge*> p(e->getUID(), e);
			map.insert(p);
		}

		for (auto p : map) ls.push_back(p.second);

		return ls;
	}

	VertexList MeshQuery::edgeAdjacentVertices(Edge *ed) {
		VertexList vlst;
		vlst.push_back(ed->v00());
		vlst.push_back(ed->v01());

		return vlst;
	}

	FaceList MeshQuery::edgeIncidentFaces(Edge *ed) {
		std::unordered_map<size_t, Face*> map;
		FaceList ls;

		for (auto f : ed->incidentFaces()) {
			std::pair<size_t, Face*> p(f->getUID(), f);
			map.insert(p);
		}

		for (auto p : map) ls.push_back(p.second);

		return ls;
	}

	EdgeList MeshQuery::faceAdjacentEdges(Face *fc) {
		std::unordered_map<size_t, Edge*> map;
		EdgeList ls;

		for (auto e : fc->adjacentEdges()) {
			std::pair<size_t, Edge*> p(e->getUID(), e);
			map.insert(p);
		}

		for (auto p : map) ls.push_back(p.second);

		return ls;
	}

	// Indirect Queries
	FaceList MeshQuery::vertexIncidentFaces(Vert *vt) {
		std::unordered_map<size_t, Face*> map;
		FaceList fclst;

		for (auto ed : vertexIncidentEdges(vt)) {
			for (auto fc : edgeIncidentFaces(ed)) {
				std::pair<size_t, Face*> p(fc->getUID(), fc);
				map.insert(p);
			}
		}

		for (auto p : map) fclst.push_back(p.second);

		return fclst;
	}

	VertexList MeshQuery::faceBoundingVertices(Face *fc) {
		std::unordered_map<size_t, Vert*> map;
		VertexList vtlst;

		for (auto ed : faceAdjacentEdges(fc)) {
			for (auto vt : edgeAdjacentVertices(ed)) {
				std::pair<size_t, Vert*> p(vt->getUID(), vt);
				map.insert(p);
			}
		}

		for (auto p : map) vtlst.push_back(p.second);

		return vtlst;
	}

	VertexList MeshQuery::vertexNeighboringVertices(Vert *vt) {
		std::unordered_map<size_t, Vert*> map;
		VertexList vtlst;

		for (auto ed : vertexIncidentEdges(vt)) {
			for (auto _vt : edgeAdjacentVertices(ed)) {
				if (vt->getUID() != _vt->getUID()) {
					std::pair<size_t, Vert*> p(vt->getUID(), vt);
					map.insert(p);
				}
			}
		}

		for (auto p : map) vtlst.push_back(p.second);

		return vtlst;
	}

	EdgeList MeshQuery::edgeNeighboringEdges(Edge *ed) {
		std::unordered_map<size_t, Edge*> map;
		EdgeList edlst;

		for (auto vt: edgeAdjacentVertices(ed)) {
			for (auto _ed : vertexIncidentEdges(vt)) {
				if (ed->getUID() != _ed->getUID()) {
					std::pair<size_t, Edge*> p(ed->getUID(), ed);
					map.insert(p);
				}
			}
		}

		for (auto p : map) edlst.push_back(p.second);

		return edlst;
	}

	FaceList MeshQuery::faceNeighboringFaces(Face *fc) {
		std::unordered_map<size_t, Face*> map;
		FaceList fclst;

		for (auto ed : faceAdjacentEdges(fc)) {
			for (auto _fc : edgeIncidentFaces(ed)) {
				if (fc->getUID() != _fc->getUID()) {
					std::pair<size_t, Face*> p(_fc->getUID(), _fc);
					map.insert(p);

				}
			}
		}

		for (auto p : map) fclst.push_back(p.second);

		return fclst;
	}

	// Other Queries
	
	//Faceloop
	FaceList MeshQuery::faceLoop(FaceList faces) {
		FaceList lst;
		std::unordered_map<size_t, Face*> loopFaces;
		std::queue<Edge*> edges;

		bool found = false;
		Edge *edge = nullptr;

		if (faces.size() > 1) {
			for (auto f00 : faces) {

				if (f00->verticesCount() != 4) continue;

				for (auto f01 : faces) {
					
					if (f00->getUID() == f01->getUID()) continue;
					if (f01->verticesCount() != 4) continue;

					Edge *se = util::sharedEdge(f00, f01);

					if (se != nullptr) {
						edge = se;
						found = true;
						break;
					}
				}

				if (found) break;
			}

			if (edge != nullptr) {
				
				edges.push(edge);

				while (!edges.empty()) {

					for (auto f : edgeIncidentFaces(edges.front())) {

						auto itr = loopFaces.find(f->getUID());

						if (itr == loopFaces.end()) {

							if (f->verticesCount() == 4) {

								for (auto e : faceAdjacentEdges(f)) {

									if (e->v00()->getUID() != edges.front()->v00()->getUID() &&
										e->v00()->getUID() != edges.front()->v01()->getUID() &&
										e->v01()->getUID() != edges.front()->v00()->getUID() &&
										e->v01()->getUID() != edges.front()->v01()->getUID()) {

										edges.push(e);
										break;
									}
								}

								loopFaces[f->getUID()] = f;
							}							
						}
					}
					edges.pop();
				}

				for (auto p : loopFaces) lst.push_back(p.second);
			}
			
		}

		return lst;
	}

	/// Connected Faces
	FaceList MeshQuery::connectedFaces(Mesh *mesh, FaceList faces) {
		FaceList lst;
	
		std::queue<ds::Face*> queue;
		for (auto f : faces) {
			queue.push(f);
		}

		std::vector<bool> visited(mesh->faces().size(), false);

		while (!queue.empty()) {
			auto front = queue.front();
			queue.pop();

			lst.push_back(front);

			for (auto adj : faceNeighboringFaces(front)) {
				if (!visited[adj->getIndex()]) {
					queue.push(adj);
					visited[adj->getIndex()] = true;
				}
			}
		}

		return lst;
	}
}
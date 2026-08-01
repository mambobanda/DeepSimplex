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


#include "DsUtil.h"

namespace ds {
	namespace util {

		ProxyEdge::ProxyEdge() {
		}

		ProxyEdge::ProxyEdge(ds::Edge *e, ds::Vert *a, ds::Vert* b) {
			_reference = e;
			_a = a;
			_b = b;
		}

		void ProxyEdge::setReference(ds::Edge *ref) {
			_reference = ref;
		}

		ds::Edge *ProxyEdge::reference() {
			return _reference;
		}

		void ProxyEdge::setA(ds::Vert *aa) {
			_a = aa;
		}

		ds::Vert *ProxyEdge::a() {
			return _a;
		}

		void ProxyEdge::setB(ds::Vert *bb) {
			_b = bb;
		}

		ds::Vert *ProxyEdge::b() {
			return _b;
		}

		void removeEdge(size_t uid, std::vector<Edge*> &ls) {
			auto itr = ls.begin();
			
			while (itr != ls.end()) {
				if ((*itr)->getUID() == uid) {
					ls.erase(itr);
					return;
				}

				itr++;
			}
		}

		// Check if vertices belong to the same edge
		std::pair<bool, bool> edgeIdentical(ds::Vert *a, ds::Vert *b, ds::Edge* edge) {
			bool identical = false;
			bool identicalOrder = false;

			std::string edgeid = std::to_string(edge->v00()->getUID()) + ":" + std::to_string(edge->v01()->getUID());
			std::string id00 = std::to_string(a->getUID()) + ":" + std::to_string(b->getUID());
			std::string id01 = std::to_string(b->getUID()) + ":" + std::to_string(a->getUID());

			if (edgeid == id00) {
				identical = true;
				identicalOrder = true;
			}
			else if (edgeid == id01) {
				identical = true;
			}

			return std::pair<bool, bool>(identical, identicalOrder);
		}

		// Check if a list of edges has edge with both input vertices
		std::pair<bool, bool> hasEdge(Vert *a, Vert *b, std::vector<Edge*> el, int &index) {
			bool identical = false;
			bool identicalOrder = false;

			int counter = 0;
			for (auto e : el) {
				auto rc = edgeIdentical(a, b, e);

				if (rc.first) {
					index = counter;
					return rc;
				}

				counter++;
			}

			return std::pair<bool, bool>(identical, identicalOrder);
		}

		std::vector<std::pair<Vert*, Vert*>> faceVerticesAsPairs(Face *f) {
			std::vector<std::pair<Vert*, Vert*>> vertPairs;

			for (int i = 1; i < f->verticesCount(); i++) {
				auto a = f->vertices()[i - 1];
				auto b = f->vertices()[i];

				std::pair<Vert*, Vert*> p(a, b);

				vertPairs.push_back(p);
			}

			vertPairs.push_back(std::pair<Vert*, Vert*>(f->vertices()[f->verticesCount() - 1], f->vertices()[0]));

			return vertPairs;
		}

		// Sort face edges (clockwise) into oritented edge 
		std::vector<util::ProxyEdge> sortFaceEdges(Face *f) {
			MeshQuery mq;

			// Resolve orientation of andjacent face edges  
			std::vector<util::ProxyEdge> orientedEdges;
			auto vertPairs = faceVerticesAsPairs(f);
			
			// loop to resolve and store orientation (used for side faces)
			for (auto pr : vertPairs) {
				for (auto e : mq.faceAdjacentEdges(f)) {
					auto rs = util::edgeIdentical(pr.first, pr.second, e);

					if (rs.first) {

						if (rs.second) {
							util::ProxyEdge pr(e, e->v00(), e->v01());
							orientedEdges.push_back(pr);
						}
						else {
							util::ProxyEdge pr(e, e->v01(), e->v00());
							orientedEdges.push_back(pr);
						}
					}
				}
			}

			return orientedEdges;
		}

		// Check if a point belongs to only one edge in a list of edges
		bool isEndPoint(Vert *v, std::vector<Edge*> el) {
			int counter = 0;

			for (auto e : el) {

				if (v->getUID() == e->v00()->getUID() ||
					v->getUID() == e->v01()->getUID()) {
					counter++;
				}
			}

			if (counter == 1) return true;
			else return false;
		}

		// Replace Proxy Vert
		bool replaceProxyEdgeVertex(size_t uid, std::vector<ProxyEdge> &ls, Vert *vt) {

			for (auto &p : ls) {
				if (p.a()->getUID() == uid) {
					p.setA(vt);
					return true;
				}
				else if (p.b()->getUID() == uid) {
					p.setB(vt);
					return true;
				}
			}

			return false;
		}

		// Find face with edge in direction <from -> to> following winding order
		Face *adjacentFaceFromTo(Edge *edge, Vert *from, Vert *to) {
			MeshQuery mq;
			Face *output = nullptr;

			auto ifs = mq.edgeIncidentFaces(edge);

			for (auto f : ifs) {
				for (auto &se : util::sortFaceEdges(f)) {

					if (edge->getUID() == se.reference()->getUID()) {

						if (se.a()->getUID() == from->getUID() && se.b()->getUID() == to->getUID()) {
							return f;
						}
					}
				}
			}

			return output;
		}

		// Find face with edge proxy
		util::ProxyEdge adjacentFaceProxy(Face* fce, Edge *edg) {

			for (auto &se : util::sortFaceEdges(fce)) {
				if (edg->getUID() == se.reference()->getUID()) {
					return se;
				}
			}

			return util::ProxyEdge();
		}

		// Sorted Adjacent Point Edges
		//std::vector<util::ProxyEdge> pointAdjacentEdgesCCW(Edge *startEdge, Vert *a, Vert *b) {
		//	MeshQuery mq;
		//	std::vector<util::ProxyEdge> output;

		//	auto ifs = mq.edgeIncidentFaces(startEdge);
		//	Face *nextFace = nullptr;
		//	util::ProxyEdge nextEdge;

		//	for (auto &se : util::sortFaceEdges(ifs.front())) {

		//		if (se.a()->getUID() == a->getUID() && se.b()->getUID() == b->getUID()) {
		//			nextFace = ifs.front();
		//			//printf("Clock Wise Face Found00\n");
		//		}
		//	}

		//	if (nextFace == nullptr) {
		//		for (auto &se : util::sortFaceEdges(ifs.back())) {

		//			if (se.a()->getUID() == a->getUID() && se.b()->getUID() == b->getUID()) {
		//				nextFace = ifs.back();
		//				//printf("Clock Wise Face Found01\n");
		//			}
		//		}
		//	}
		//
		//	nextEdge.setA(a);
		//	nextEdge.setB(b);
		//	nextEdge.setReference(startEdge);

		//	while (nextFace != nullptr && nextEdge.reference() != nullptr){
		//		output.push_back(nextEdge);
		//		auto nextFaceUID = nextFace->getUID();
		//		auto ses = util::sortFaceEdges(nextFace);
		//		nextFace = nullptr;

		//		int counter = 0;
		//		for (auto &se : ses) {
		//			if (se.reference()->getUID() == nextEdge.reference()->getIndex()) {

		//				bool flip = false;
		//				if (se.b()->getUID() == a->getUID()) flip = true;

		//				if (flip) {
		//					if (counter == ses.size() - 1) {
		//						nextEdge = ses[0];
		//					}
		//					else {
		//						nextEdge = ses[counter + 1];
		//					}
		//				}
		//				else {
		//					if (counter == 0) {
		//						nextEdge = ses[ses.size() - 1];
		//					}
		//					else {
		//						nextEdge = ses[counter - 1];
		//					}
		//				}

		//				if (nextEdge.reference()->getUID() == startEdge->getUID()) {
		//					output.push_back(nextEdge);
		//					break;
		//				}

		//				auto n_ifs = mq.edgeIncidentFaces(nextEdge.reference());

		//				if (n_ifs.size() == 2) {

		//					if (n_ifs.front()->getUID() != nextFaceUID && n_ifs.back()->getUID() == nextFaceUID) {
		//						nextFace = n_ifs.front();
		//						nextFaceUID = nextFace->getUID();
		//					}
		//					else if (n_ifs.back()->getUID() != nextFaceUID && n_ifs.front()->getUID() == nextFaceUID) {
		//						nextFace = n_ifs.back();
		//						nextFaceUID = nextFace->getUID();
		//					}

		//				}
		//				
		//			}
		//			counter++;
		//			if (nextFace != nullptr) break;
		//		}
		//	}

		//	return output;
		//}

		// Sorted Adjacent Point Edges
		std::vector<util::ProxyEdge> pointAdjacentEdgesCCW(Edge *startEdge, Vert *a, Vert *b) {

			MeshQuery mq;
			std::vector<util::ProxyEdge> output;

			auto ifs = mq.edgeIncidentFaces(startEdge);
			Face *curentFace = adjacentFaceFromTo(startEdge, a, b);
			util::ProxyEdge currentEdge;
			currentEdge.setA(a);
			currentEdge.setB(b);
			currentEdge.setReference(startEdge);
			output.push_back(currentEdge);

			bool done = false;

			while (!done && curentFace != nullptr) {
				done = true;
				for (auto &se : util::sortFaceEdges(curentFace)) {
					if (se.b()->getUID() == a->getUID()) {

						if (se.reference()->getUID() != startEdge->getUID()) {
							util::ProxyEdge edg;
							edg.setA(se.b());
							edg.setB(se.a());
							edg.setReference(se.reference());
							output.push_back(edg);

							curentFace = adjacentFaceFromTo(se.reference(), se.b(), se.a());
							done = false;
							break;
						}
						else {
							output.push_back(currentEdge);
							done = true;
						}
					}
				}
			}

			return output;
		}

		// Face shared by two edges
		Face *sharedFace(Edge *e00, Edge *e01) {
			MeshQuery mq;
			Face *output = nullptr;

			auto ifs00 = mq.edgeIncidentFaces(e00);
			auto ifs01 = mq.edgeIncidentFaces(e01);

			for (auto f00 : ifs00) {
				for (auto f01 : ifs01) {
					if (f00->getUID() == f01->getUID()) return f00;
				}
			}

			return output;
		}

		// Face shared by two edges
		Edge *sharedEdge(Face *f00, Face *f01){
			MeshQuery mq;

			for (auto e : mq.faceAdjacentEdges(f00)) {

				for (auto f: mq.edgeIncidentFaces(e)) {

					if (f->getUID() == f01->getUID()) {
						return e;
					}
				}
			}

			return nullptr;
		}

		// Distance between two edges
		std::pair<float, glm::vec3> distance(Edge *a, Edge*b) {

			gte::Segment3<float> r1;
			gte::Segment3<float> r2;
			r1.p[0] = gte::Vector<3, float>{ a->v00()->position().x, a->v00()->position().y, a->v00()->position().z };
			r1.p[1] = gte::Vector<3, float>{ a->v01()->position().x, a->v01()->position().y, a->v01()->position().z };
			r2.p[0] = gte::Vector<3, float>{ b->v00()->position().x, b->v00()->position().y, b->v00()->position().z };
			r2.p[1] = gte::Vector<3, float>{ b->v01()->position().x, b->v01()->position().y, b->v01()->position().z };
			gte::DCPSegment3Segment3<float> qry;
			auto result = qry(r1, r2);

			float dist = result.distance;
			glm::vec3 closest(result.closest[1][0], result.closest[1][1], result.closest[1][2]);

			return std::pair<float, glm::vec3>(dist, closest);
		}

		
	}
}
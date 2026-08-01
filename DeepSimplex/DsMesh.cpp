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


#include "DsMesh.h"

// Data Structure based on "Adjacency and incidence framework: a data structure for efficient and fast management of multiresolution meshes.(2009)" paper.

namespace ds {
	size_t Mesh::MESH_UID_SOURCE = 0x64;

	Mesh::Mesh() {
		uid = MESH_UID_SOURCE++;

		VERTEX_UID_SOURCE = 0;
		EDGE_UID_SOURCE = 0;
		FACE_UID_SOURCE = 0;

		_useTriangleCaching = true;
	}

	Mesh::~Mesh() {
		
	}

	size_t Mesh::getUID() {
		return uid;
	}

	void Mesh::changeUID(size_t ud) {
		uid = ud;
	}

	std::vector<Vert*> &Mesh::vertices() {
		return vl;
	}

	std::vector<Edge*> &Mesh::edges() {
		return el;
	}

	std::vector<Face*> &Mesh::faces() {
		return fl;
	}

	void Mesh::reindexVertices() {
		size_t counter = 0;
		for (auto v: vl) {
			v->changeIndex(counter++);
		}
	}

	void Mesh::reindexEdges() {
		size_t counter = 0;
		for (auto e : el) {
			e->changeIndex(counter++);
		}
	}

	void Mesh::reindexFaces() {
		size_t counter = 0;
		for (auto f : fl) {
			f->changeIndex(counter++);
		}
	}

	void Mesh::setMeshIDSourceBase(size_t bs) {
		MESH_UID_SOURCE = bs;
	}

	size_t Mesh::getMeshIDSourceBase() {
		return MESH_UID_SOURCE;
	}

	void Mesh::setVertexIDSourceBase(size_t bs) {
		VERTEX_UID_SOURCE = bs;
	}

	size_t Mesh::getVertexIDSourceBase() {
		return VERTEX_UID_SOURCE;
	}

	void Mesh::setEdgeIDSourceBase(size_t bs) {
		EDGE_UID_SOURCE = bs;
	}

	size_t Mesh::getEdgeIDSourceBase() {
		return EDGE_UID_SOURCE;
	}

	void Mesh::setFaceIDSourceBase(size_t bs) {
		FACE_UID_SOURCE = bs;
	}

	size_t Mesh::getFaceIDSourceBase() {
		return FACE_UID_SOURCE;
	}

	Vert *Mesh::addVertex(glm::vec3 pt) {
		return addVertex(pt.x, pt.y, pt.z);
	}

	Vert *Mesh::addVertex(float a, float b, float c) {

		Vert *v = new ds::Vert();
		v->changeUID(VERTEX_UID_SOURCE++);
		v->position() = glm::vec3(a, b, c);
		
		v->setPropertyMap(&pmap);
		pmap.addToMap(v->getUID(), ds::PropertyType::VERTEX);

		if (pmap.hasProperty("uvs", ds::PropertyType::VERTEX)) {
			v->setProperty<glm::vec2>("uvs", glm::vec2(0));
		}

		if (pmap.hasProperty("smooth_normals", ds::PropertyType::VERTEX)) {
			v->setProperty<glm::vec3>("smooth_normals", glm::vec3(0));
		}
		
		vl.push_back(v);

		return v;
	}

	void Mesh::removeVertex(Vert *vt) {
		auto it = vl.begin();

		std::vector<Edge*> edges;
		for (auto ed : vt->incidentEdges())
			edges.push_back(ed);

		for(auto ed: edges)
			removeEdge(ed);

		while (it != vl.end()) {
			if ((*it)->getUID() == vt->getUID()) {
				vl.erase(it);
				break;
			}
			it++;
		}

		pmap.removeFromMap(vt->getUID(), ds::PropertyType::VERTEX);

		delete vt;
	}

	Edge *Mesh::addEdge(Vert *v00, Vert *v01) {
		Edge *e = new Edge();
		e->changeUID(EDGE_UID_SOURCE++);
		e->setV00(v00);
		e->setV01(v01);
		v00->incidentEdges().push_back(e);
		v01->incidentEdges().push_back(e);

		e->setPropertyMap(&pmap);
		pmap.addToMap(e->getUID(), ds::PropertyType::EDGE);

		el.push_back(e);

		return e;
	}

	void Mesh::removeEdge(Edge *ed) {
		auto it = el.begin();

		std::vector<Face*> afaces;
		for (auto fc : ed->incidentFaces())
			afaces.push_back(fc);

		for(auto fc: afaces)
			removeFace(fc);

		while (it != el.end()) {
			if ((*it)->getUID() == ed->getUID()) {

				ed->v00()->removeEdge(ed);
				ed->v01()->removeEdge(ed);

				el.erase(it);
				break;
			}
			it++;
		}

		pmap.removeFromMap(ed->getUID(), ds::PropertyType::EDGE);

		delete ed;
	}

	Face *Mesh::addFace(EdgeList ls, std::vector<Vert*> vs) {

		Face *f = new Face();
		f->changeUID(FACE_UID_SOURCE++);
		f->adjacentEdges() = ls;
		f->newVerts((int)vs.size());

		int counter = 0;
		for (auto vt : vs) {
			f->vertices()[counter++] = vt;
			f->uvs().push_back(glm::vec2(0.f));
		}

		for (auto ed : ls) ed->incidentFaces().push_back(f);
		
		f->setPropertyMap(&pmap);
		pmap.addToMap(f->getUID(), ds::PropertyType::FACE);

		fl.push_back(f);

		if (_useTriangleCaching) {
			auto triIndexList = _triangleIndexMap.triangulateFace(f);
			if (triIndexList.second != nullptr) _triangleIndexMap.addIndex(f->getUID(), triIndexList);
		}
		
		return f;
	}

	void Mesh::removeFace(Face *fc) {

		auto it = fl.begin();

		while (it != fl.end()) {
			if ((*it)->getUID() == fc->getUID()) {

				auto eitr = fc->adjacentEdges().begin();

				while (eitr != (*it)->adjacentEdges().end()) {
					(*eitr)->removeFace(fc);
					eitr++;
				}

				fl.erase(it);
				break;
			}
			it++;
		}

		pmap.removeFromMap(fc->getUID(), ds::PropertyType::FACE);

		if (_useTriangleCaching) {
			_triangleIndexMap.removeIndex(fc->getUID());
		}

		delete fc;
	}

	void Mesh::addProperty(std::string name, ds::PropertyType type, unsigned int valType) {
		std::vector<size_t> all_ids;

		if (type == ds::PropertyType::VERTEX) {

			for (auto v : vl) {
				all_ids.push_back(v->getUID());
			}

			pmap.addProperty(name, all_ids, ds::PropertyType::VERTEX, valType);

		}
		else if (type == ds::PropertyType::EDGE) {

			for (auto v : el) {
				all_ids.push_back(v->getUID());
			}

			pmap.addProperty(name, all_ids, ds::PropertyType::EDGE, valType);

		}
		else if (type == ds::PropertyType::FACE) {

			for (auto v : fl) {
				all_ids.push_back(v->getUID());
			}

			pmap.addProperty(name, all_ids, ds::PropertyType::FACE, valType);

		}

	}

	std::vector<std::string> Mesh::propertyKeys(ds::PropertyType type) {

		if (type == ds::PropertyType::VERTEX) {
			return pmap.propertyKeys(ds::PropertyType::VERTEX);
		}
		else if (type == ds::PropertyType::EDGE) {
			return pmap.propertyKeys(ds::PropertyType::EDGE);
		}
		else if (type == ds::PropertyType::FACE) {
			return pmap.propertyKeys(ds::PropertyType::FACE);
		}

		return std::vector<std::string>();
	}

	PropertyMap &Mesh::getPropertyMap() {
		return pmap;
	}


	bool Mesh::useTriangleCaching() {
		return _useTriangleCaching;
	}

	void Mesh::setUseTriangleCaching(bool val) {
		_useTriangleCaching = val;
	}

	TriangleIndexMap &Mesh::triangleIndexMap() {
		return _triangleIndexMap;
	}

	void Mesh::clearMesh() {
		std::vector<Vert*> vts;

		for (auto v : vl) vts.push_back(v);

		for (auto v : vts) removeVertex(v);

		el.clear();
		vl.clear();
		fl.clear();

		pmap.clear();
	}

	void Mesh::fastClearMesh() {
		pmap.clear();

		for (auto v : vl) delete v;
		for (auto e : el) delete e;
		for (auto f : fl) delete f;

		vl.clear();
		el.clear();
		fl.clear();
	}

	void Mesh::copyMesh(Mesh *mesh) {
		clearMesh();

		pmap.copyMap(&mesh->getPropertyMap());

		if (mesh->vertices().size() > 0) vl.resize(mesh->vertices().size());
		if (mesh->edges().size() > 0) el.resize(mesh->edges().size());
		if (mesh->faces().size() > 0) fl.resize(mesh->faces().size());

		for (int i = 0; i < mesh->vertices().size(); i++) {
			
			Vert *vTemp = new Vert();

			vTemp->position() = mesh->vertices()[i]->position();
			vTemp->changeUID(mesh->vertices()[i]->getUID());
			vTemp->changeIndex(mesh->vertices()[i]->getIndex());
			vTemp->setPropertyMap(&pmap);

			vl[i] = vTemp;
		}

		for (int i = 0; i < mesh->edges().size(); i++) {

			Edge *eTemp = new Edge();

			auto v00 = vl[mesh->edges()[i]->v00()->getIndex()];
			auto v01 = vl[mesh->edges()[i]->v01()->getIndex()];
			eTemp->setV00(v00);
			eTemp->setV01(v01);
			v00->incidentEdges().push_back(eTemp);
			v01->incidentEdges().push_back(eTemp);
			eTemp->changeUID(mesh->edges()[i]->getUID());
			eTemp->changeIndex(mesh->edges()[i]->getIndex());
			eTemp->setPropertyMap(&pmap);

			el[i] = eTemp;
		}

		for (int i = 0; i < mesh->faces().size(); i++) {

			Face *fTemp = new Face();
			fTemp->changeUID(mesh->faces()[i]->getUID());
			fTemp->changeIndex(mesh->faces()[i]->getIndex());
			fTemp->setPropertyMap(&pmap);

			for (auto e_r : mesh->faces()[i]->adjacentEdges()) {
				auto e_o = el[e_r->getIndex()];
				fTemp->adjacentEdges().push_back(e_o);
				e_o->incidentFaces().push_back(fTemp);
			}

			if (mesh->faces()[i]->verticesCount() > 0) {
				fTemp->newVerts(mesh->faces()[i]->verticesCount());
				
				for (int j = 0; j < mesh->faces()[i]->verticesCount(); j++)
					fTemp->vertices()[j] = vl[mesh->faces()[i]->vertices()[j]->getIndex()];
			}

			fl[i] = fTemp;

			if (_useTriangleCaching) {
				auto triIndexList = _triangleIndexMap.triangulateFace(fTemp);
				if (triIndexList.second != nullptr) _triangleIndexMap.addIndex(fTemp->getUID(), triIndexList);
			}
		}

		setMeshIDSourceBase(mesh->getMeshIDSourceBase());
		setVertexIDSourceBase(mesh->getVertexIDSourceBase());
		setEdgeIDSourceBase(mesh->getEdgeIDSourceBase());
		setFaceIDSourceBase(mesh->getFaceIDSourceBase());

	}

	void Mesh::removeProperty(std::string name, ds::PropertyType type) {
		pmap.removeProperty(name, type);
	}

	bool Mesh::hasProperty(std::string name, ds::PropertyType type) {
		return pmap.hasProperty(name, type);
	}

	std::unordered_map<size_t, Any>* Mesh::vertexPropertyMap(std::string map) {
		return pmap.vertexPropertyMap(map);
	}

	std::unordered_map<size_t, Any>* Mesh::edgePropertyMap(std::string map) {
		return pmap.edgePropertyMap(map);
	}

	std::unordered_map<size_t, Any>* Mesh::facePropertyMap(std::string map) {
		return pmap.facePropertyMap(map);
	}

	bool Mesh::operator==(Mesh& ms) {
		return uid == ms.getUID();
	}

	bool Mesh::operator!=(Mesh& ms) {
		return uid != ms.getUID();
	}

	bool Mesh::operator==(Mesh* ms) {
		return uid == ms->getUID();
	}

	bool Mesh::operator!=(Mesh* ms) {
		return uid != ms->getUID();
	}
}
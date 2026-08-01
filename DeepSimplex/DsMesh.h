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

#include <vector>
#include "DsCommon.h"
#include "DsMeshQuery.h"
#include "DsVert.h"
#include "DsEdge.h"
#include "DsFace.h"
#include "DsMesh.h"
#include "DsTriangleIndexMap.h"
#include "DsPropertyMap.h"

namespace ds {
	
	class Mesh {
	private:
		static size_t MESH_UID_SOURCE;
		size_t VERTEX_UID_SOURCE;
		size_t EDGE_UID_SOURCE;
		size_t FACE_UID_SOURCE;

	private:
		PropertyMap pmap;
		TriangleIndexMap _triangleIndexMap;

		bool _useTriangleCaching;
	private:
		size_t uid; //mesh ID
		std::vector<Vert*> vl; //vertices
		std::vector<Edge*> el; //edges
		std::vector<Face*> fl; //vector of faces

	public:
		Mesh();
		virtual ~Mesh();

		size_t getUID();
		void changeUID(size_t);

		std::vector<Vert*> &vertices();
		std::vector<Edge*> &edges();
		std::vector<Face*> &faces();

		void reindexVertices();
		void reindexEdges();
		void reindexFaces();

		void setMeshIDSourceBase(size_t);
		size_t getMeshIDSourceBase();

		void setVertexIDSourceBase(size_t);
		size_t getVertexIDSourceBase();

		void setEdgeIDSourceBase(size_t);
		size_t getEdgeIDSourceBase();

		void setFaceIDSourceBase(size_t);
		size_t getFaceIDSourceBase();

		Vert *addVertex(glm::vec3 pt);
		Vert *addVertex(float a, float b, float c);
		void removeVertex(Vert *vt);

		Edge *addEdge(Vert *v00, Vert *v01);
		void removeEdge(Edge *ed);

		Face *addFace(EdgeList ls, std::vector<Vert*> vs);
		void removeFace(Face *vt);

		bool useTriangleCaching();
		void setUseTriangleCaching(bool);

		TriangleIndexMap &triangleIndexMap();

		void clearMesh();
		void fastClearMesh();
		void copyMesh(Mesh*);

		void addProperty(std::string name, ds::PropertyType type, unsigned int valType);
		void removeProperty(std::string name, ds::PropertyType type);
		bool hasProperty(std::string name, ds::PropertyType type);
		std::vector<std::string> propertyKeys(ds::PropertyType type);

		PropertyMap &getPropertyMap();

		template<typename T>
		inline void fillProperty(std::string name, T val, ds::PropertyType type){
			pmap.fillProperty<T>(name, val, type);
		}

		std::unordered_map<size_t, Any>* vertexPropertyMap(std::string map);

		std::unordered_map<size_t, Any>* edgePropertyMap(std::string map);

		std::unordered_map<size_t, Any>* facePropertyMap(std::string map);

		bool operator==(Mesh&);
		bool operator!=(Mesh&);
		bool operator==(Mesh*);
		bool operator!=(Mesh*);
 
	};
}
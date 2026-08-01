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
#include "DsPropertyMap.h"

namespace ds {

	class Face {
	private:
		size_t uid;
		EdgeList la; // adjacent edges
		//std::vector<Vert*> vs; // face verts
		Vert **vs;
		std::vector<glm::vec2> _uvs;
		int vsCount;
		size_t index;
		size_t surf_id;
	private:
		PropertyMap *pmap;
	public:

		Face();
		virtual ~Face();

		size_t getUID();
		void changeUID(size_t);

		size_t getIndex();
		void changeIndex(size_t);

		EdgeList &adjacentEdges();
		Vert** vertices();
		int verticesCount();

		size_t getSurfID();
		void setSurfID(size_t);

		std::vector<glm::vec2> &uvs();
		std::unordered_map<size_t, glm::vec2> mapUVS();

		void newVerts(int count);

		bool isPropertyNull(std::string name);

		template<typename T>
		inline void setProperty(std::string name, T val) {
			pmap->setProperty<T>(name, getUID(), val, ds::PropertyType::FACE);
		}

		template<typename T>
		inline T getProperty(std::string name) {
			return pmap->getProperty<T>(name, getUID(), ds::PropertyType::FACE);
		}

		void setPropertyMap(PropertyMap *);

		bool operator==(Face&);
		bool operator!=(Face&);
		bool operator==(Face*);
		bool operator!=(Face*);
	};
}
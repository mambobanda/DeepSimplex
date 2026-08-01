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

#include "DsCommon.h"
#include "DsPropertyMap.h"

namespace ds {

	class Vert {
	private:
		size_t uid;
		size_t index;
		EdgeList li;  // incident edges
		glm::vec3 pt; // geometry

	private:
		PropertyMap *pmap;
	public:
		Vert();
		virtual ~Vert();

		size_t getUID();
		void changeUID(size_t);

		size_t getIndex();
		void changeIndex(size_t);

		EdgeList &incidentEdges();
		glm::vec3 &position();

		void removeEdge(Edge *);

		bool isPropertyNull(std::string name);

		template<typename T>
		inline void setProperty(std::string name, T val) {
			pmap->setProperty<T>(name, getUID(), val, ds::PropertyType::VERTEX);
		}

		template<typename T>
		inline T getProperty(std::string name) {
			return pmap->getProperty<T>(name, getUID(), ds::PropertyType::VERTEX);
		}

		void setPropertyMap(PropertyMap *);

		bool operator==(Vert&);
		bool operator!=(Vert&);
		bool operator==(Vert*);
		bool operator!=(Vert*);
	};

}


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
#include "DsVert.h"

namespace ds {

	class Edge {
	private:
		size_t uid;
		Vert *vertex00, *vertex01; // adjacent vertices
		size_t index;
		FaceList li; // incident faces

	private:
		PropertyMap *pmap;
	public:

		Edge();
		virtual ~Edge();

		size_t getUID();
		void changeUID(size_t);

		size_t getIndex();
		void changeIndex(size_t);

		Vert *v00();
		Vert *v01();
		void setV00(Vert *);
		void setV01(Vert *);

		FaceList &incidentFaces();

		void removeFace(Face *);

		bool isPropertyNull(std::string name);

		template<typename T>
		inline void setProperty(std::string name, T val) {
			pmap->setProperty<T>(name, getUID(), val, ds::PropertyType::EDGE);
		}

		template<typename T>
		inline T getProperty(std::string name) {
			return pmap->getProperty<T>(name, getUID(), ds::PropertyType::EDGE);
		}

		void setPropertyMap(PropertyMap *);

		bool operator==(Edge&);
		bool operator!=(Edge&);
		bool operator==(Edge*);
		bool operator!=(Edge*);
	};
}
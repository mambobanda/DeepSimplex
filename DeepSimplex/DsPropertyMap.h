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
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <unordered_map>
#include "DsAny.h"

namespace ds {
	
	enum class PropertyType {VERTEX, EDGE, FACE};

	class PropertyMap {
	private:
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> vertexMap;
		std::unordered_map <std::string, unsigned int> vertexMapType;
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> edgeMap;
		std::unordered_map <std::string, unsigned int> edgeMapType;
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> faceMap;
		std::unordered_map <std::string, unsigned int> faceMapType;
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> emptyMap;

	private: 
		
		///////////////////////////////
		//Base Functions
		//////////////////////////////

		void addProperty(std::string name, std::vector<size_t> references, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		bool isPropertyNull(std::string name, size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		bool hasProperty(std::string name, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		void addToMap(size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		void removeFromMap(size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		void removeProperty(std::string name, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		std::vector<std::string> propertyKeys(std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map);

		template<typename T>
		inline T getProperty(std::string name, size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {

			auto itr00 = map.find(name);

			if (itr00 != map.end()) {
				auto itr01 = itr00->second->find(id);

				if (itr01 != itr00->second->end()) {
					return itr01->second.getValue<T>();
				}
			}

			return T();
		}

		template<typename T>
		inline void setProperty(std::string name, size_t id, T val, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {

			auto itr00 = map.find(name);

			if (itr00 != map.end()) {
				auto itr01 = itr00->second->find(id);

				if (itr01 != itr00->second->end()) {
					itr01->second.setValue<T>(val);
				}
			}
		}

		template<typename T>
		inline void fillProperty(std::string name, T val, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
			auto itr00 = map.find(name);

			if (itr00 != map.end()) {
				for (auto &p : *itr00->second) {
					p.second.setValue<T>(val);
				}
			}
		}

		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &getMap(PropertyType type);
		//////////////////////////////

		void copyMap(
			std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &from,
			std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &to,
			std::unordered_map <std::string, unsigned int> &typeMap);

		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &getVertexMap();
		std::unordered_map <std::string, unsigned int> &getVertexMapType();
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &getEdgeMap();
		std::unordered_map <std::string, unsigned int> &getEdgeMapType();
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &getFaceMap();
		std::unordered_map <std::string, unsigned int> &getFaceMapType();
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &getEmptyMap();
	public:		

		//////////////////////////////
		//Vertex Map Functions
		//////////////////////////////
		void addProperty(std::string name, std::vector<size_t> references, PropertyType type, unsigned int valType);

		bool hasProperty(std::string name, PropertyType type);

		bool isPropertyNull(std::string name, size_t id, PropertyType type);

		void addToMap(size_t id, PropertyType type);

		std::unordered_map<size_t, Any>* vertexPropertyMap(std::string map);

		std::unordered_map<size_t, Any>* edgePropertyMap(std::string map);
		
		std::unordered_map<size_t, Any>* facePropertyMap(std::string map);

		void removeFromMap(size_t id, PropertyType type);

		void removeProperty(std::string name, PropertyType type);

		unsigned int propertyType(std::string name, PropertyType type);

		std::vector<std::string> propertyKeys(PropertyType type);

		void clear();

		void copyMap(PropertyMap *_map);
		
		template<typename T>
		inline T getProperty(std::string name, size_t id, PropertyType type) {
			return getProperty<T>(name, id, getMap(type));
		}

		template<typename T>
		inline void setProperty(std::string name, size_t id, T val, PropertyType type) {
			setProperty<T>(name, id, val, getMap(type));
		}

		template<typename T>
		inline void fillProperty(std::string name, T val, PropertyType type) {
			fillProperty<T>(name, val, getMap(type));
		}

		/////////////////////////////////////////////////////////


	};
}
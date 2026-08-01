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

#include "DsPropertyMap.h"
#include "DsType.h"

namespace ds {
	///////////////////////
	//Baisc Functions
	//////////////////////

	std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &PropertyMap::getMap(PropertyType type) {
		if (type == PropertyType::VERTEX) {
			return vertexMap;
		}
		else if (type == PropertyType::EDGE) {
			return edgeMap;
		}
		else if (type == PropertyType::FACE) {
			return faceMap;
		}

		return emptyMap;
	}
	
	void PropertyMap::addProperty(std::string name, std::vector<size_t> references, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {

		std::pair<std::string, std::unordered_map<size_t, Any>*> pmap(name, new std::unordered_map<size_t, Any>());
		map.insert(pmap);

		auto ref = map.find(name);
		for (auto in_ref : references) {
			std::pair<size_t, Any> p(in_ref, Any());
			ref->second->insert(p);
		}
	}

	bool PropertyMap::hasProperty(std::string name, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
		auto ref = map.find(name);

		if (ref == map.end()) {
			return false;
		}
		else {
			return true;
		}
	}

	std::vector<std::string> PropertyMap::propertyKeys(std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
		std::vector<std::string> output;

		for (auto pair : map) {
			output.push_back(pair.first);
		}

		return output;
	}

	bool PropertyMap::isPropertyNull(std::string name, size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
		auto itr00 = map.find(name);

		if (itr00 != map.end()) {
			auto itr01 = itr00->second->find(id);

			if (itr01 != itr00->second->end()) {
				return itr01->second.isNull();
			}
		}

		return true;
	}

	void PropertyMap::addToMap(size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
		for (auto mp : map) {
			mp.second->insert(std::pair<size_t, Any>(id, Any()));
			auto it = mp;
		}
	}

	void PropertyMap::removeFromMap(size_t id, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
		for (auto mp : map) {
			mp.second->erase(id);
		}
	}

	void PropertyMap::removeProperty(std::string name, std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &map) {
		auto ref = map.find(name);

		if (ref != map.end()) {
			delete ref->second;
			map.erase(name);
		}
	}

	/////////////////////
	void PropertyMap::addProperty(std::string name, std::vector<size_t> references, PropertyType type, unsigned int valType) {
		addProperty(name, references, getMap(type));

		if (type == PropertyType::VERTEX) {
			vertexMapType[name] = valType;
		}
		else if (type == PropertyType::EDGE) {
			edgeMapType[name] = valType;
		}
		else if (type == PropertyType::FACE) {
			faceMapType[name] = valType;
		}
	}

	unsigned int PropertyMap::propertyType(std::string name, PropertyType type) {
		
		if (type == PropertyType::VERTEX) {
			auto itr = vertexMapType.find(name);

			if (itr != vertexMapType.end()) return itr->second;
		}
		else if (type == PropertyType::EDGE) {
			auto itr = edgeMapType.find(name);

			if (itr != edgeMapType.end()) return itr->second;
		}
		else if (type == PropertyType::FACE) {
			auto itr = faceMapType.find(name);

			if (itr != faceMapType.end()) return itr->second;
		}

		return std::numeric_limits<unsigned int>::infinity();
	}

	bool PropertyMap::hasProperty(std::string name, PropertyType type) {
		return hasProperty(name, getMap(type));
	}

	bool PropertyMap::isPropertyNull(std::string name, size_t id, PropertyType type) {
		return isPropertyNull(name, id, getMap(type));
	}

	void PropertyMap::addToMap(size_t id, PropertyType type) {
		addToMap(id, getMap(type));
	}

	std::unordered_map<size_t, Any>* PropertyMap::vertexPropertyMap(std::string map) {
		auto itr = vertexMap.find(map);

		if (itr != vertexMap.end()) {
			return itr->second;
		}
		else {
			return nullptr;
		}
	}

	std::unordered_map<size_t, Any>* PropertyMap::edgePropertyMap(std::string map) {
		auto itr = edgeMap.find(map);

		if (itr != edgeMap.end()) {
			return itr->second;
		}
		else {
			return nullptr;
		}
	}

	std::unordered_map<size_t, Any>* PropertyMap::facePropertyMap(std::string map) {
		auto itr = faceMap.find(map);

		if (itr != faceMap.end()) {
			return itr->second;
		}
		else {
			return nullptr;
		}
	}

	void PropertyMap::removeFromMap(size_t id, PropertyType type) {
		removeFromMap(id, getMap(type));
	}

	void PropertyMap::removeProperty(std::string name, PropertyType type) {
		removeProperty(name, getMap(type));

		if (type == PropertyType::VERTEX) {
			vertexMapType.erase(name);
		}
		else if (type == PropertyType::EDGE) {
			edgeMapType.erase(name);
		}
		else if (type == PropertyType::FACE) {
			faceMapType.erase(name);
		}
	}

	std::vector<std::string> PropertyMap::propertyKeys(PropertyType type) {
		return propertyKeys(getMap(type));
	}

	void PropertyMap::clear() {

		vertexMapType.clear();
		edgeMapType.clear();
		faceMapType.clear();

		for (auto &p : vertexMap) delete p.second;
		for (auto &p : edgeMap) delete p.second;
		for (auto &p : faceMap) delete p.second;

		vertexMap.clear();
		edgeMap.clear();
		faceMap.clear();

	}

	void PropertyMap::copyMap(
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &from,
		std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &to,
		std::unordered_map <std::string, unsigned int> &typeMap) {

		for (auto p00 : from) {

			auto nmap = new std::unordered_map<size_t, Any>;
			to[p00.first] = nmap;

			auto _type = typeMap[p00.first];
			for (auto p01 : *p00.second) {
				nmap->insert(std::pair<size_t, Any>(p01.first, Any()));

				switch (_type) {
				case ds::type::BOOL:
					(*nmap)[p01.first].setValue<bool>(p01.second.getValue<bool>());
					break;
				case ds::type::CHAR:
					(*nmap)[p01.first].setValue<char>(p01.second.getValue<char>());
					break;
				case ds::type::DMAT3:
					(*nmap)[p01.first].setValue<glm::dmat3>(p01.second.getValue<glm::dmat3>());
					break;
				case ds::type::DMAT4:
					(*nmap)[p01.first].setValue<glm::mat4>(p01.second.getValue<glm::dmat4>());
					break;
				case ds::type::DOUBLE:
					(*nmap)[p01.first].setValue<double>(p01.second.getValue<double>());
					break;
				case ds::type::DVEC2:
					(*nmap)[p01.first].setValue<glm::dvec2>(p01.second.getValue<glm::dvec2>());
					break;
				case ds::type::DVEC3:
					(*nmap)[p01.first].setValue<glm::dvec3>(p01.second.getValue<glm::dvec3>());
					break;
				case ds::type::DVEC4:
					(*nmap)[p01.first].setValue<glm::dvec4>(p01.second.getValue<glm::dvec4>());
					break;
				case ds::type::FLOAT:
					(*nmap)[p01.first].setValue<float>(p01.second.getValue<float>());
					break;
				case ds::type::INT:
					(*nmap)[p01.first].setValue<int>(p01.second.getValue<int>());
					break;
				case ds::type::IVEC2:
					(*nmap)[p01.first].setValue<glm::ivec2>(p01.second.getValue<glm::ivec2>());
					break;
				case ds::type::IVEC3:
					(*nmap)[p01.first].setValue<glm::ivec3>(p01.second.getValue<glm::ivec3>());
					break;
				case ds::type::IVEC4:
					(*nmap)[p01.first].setValue<glm::ivec4>(p01.second.getValue<glm::ivec4>());
					break;
				case ds::type::MAT3:
					(*nmap)[p01.first].setValue<glm::mat3>(p01.second.getValue<glm::mat3>());
					break;
				case ds::type::MAT4:
					(*nmap)[p01.first].setValue<glm::mat4>(p01.second.getValue<glm::mat4>());
					break;
				case ds::type::QUAT:
					(*nmap)[p01.first].setValue<glm::quat>(p01.second.getValue<glm::quat>());
					break;
				case ds::type::SIZE_T:
					(*nmap)[p01.first].setValue<size_t>(p01.second.getValue<size_t>());
					break;
				case ds::type::STRING: {
					auto val = p01.second.getValue<std::string>();
					(*nmap)[p01.first].setValue<std::string>(p01.second.getValue<std::string>());
				}
					break;
				case ds::type::UINT:
					(*nmap)[p01.first].setValue<unsigned int>(p01.second.getValue<unsigned int>());
					break;
				case ds::type::VEC2:
					(*nmap)[p01.first].setValue<glm::vec2>(p01.second.getValue<glm::vec2>());
					break;
				case ds::type::VEC3:
					(*nmap)[p01.first].setValue<glm::vec3>(p01.second.getValue<glm::vec3>());
					break;
				case ds::type::VEC4:
					(*nmap)[p01.first].setValue<glm::vec4>(p01.second.getValue<glm::vec4>());
					break;
				default:
					break;
				}
			}
		}
	}

	void PropertyMap::copyMap(PropertyMap *_map) {
		for (auto p : _map->getVertexMapType()) vertexMapType[p.first] = p.second;
		for (auto p : _map->getEdgeMapType()) edgeMapType[p.first] = p.second;
		for (auto p : _map->getFaceMapType()) faceMapType[p.first] = p.second;
		
		copyMap(_map->getVertexMap(), vertexMap, _map->getVertexMapType());
		copyMap(_map->getEdgeMap(), edgeMap, _map->getEdgeMapType());
		copyMap(_map->getFaceMap(), faceMap, _map->getFaceMapType());
	}

	std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &PropertyMap::getVertexMap() {
		return vertexMap;
	}

	std::unordered_map <std::string, unsigned int> &PropertyMap::getVertexMapType() {
		return vertexMapType;
	}

	std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &PropertyMap::getEdgeMap() {
		return edgeMap;
	}

	std::unordered_map <std::string, unsigned int> &PropertyMap::getEdgeMapType() {
		return edgeMapType;
	}

	std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &PropertyMap::getFaceMap() {
		return faceMap;
	}

	std::unordered_map <std::string, unsigned int> &PropertyMap::getFaceMapType() {
		return faceMapType;
	}

	std::unordered_map <std::string, std::unordered_map<size_t, Any>*> &PropertyMap::getEmptyMap() {
		return emptyMap;
	}

}
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
 /**
	* @file ....cpp
	*
	* @author Mambo Banda
	*
	*/

#include "DsPointCloud.h"
#include "DsPointCloudObserver.h"

namespace ds {
	
	PointCloud::PointCloud() {
		position = new Channel;

		auto v = new std::vector<glm::vec3>();
		v->resize(10);

		position->channel = v;
		position->name = "@position";
		position->type = ds::type::VEC3;

		_attributeMap["@position"] = position;
	}

	PointCloud::~PointCloud() {
		position->free_channel();
		delete position;

		removeAllAttributes();
	}

	size_t PointCloud::size() {

		auto cnn = (std::vector<glm::vec3>*)position->channel;
		return cnn->size();
	}

	void PointCloud::resize(size_t sz) {

		auto cnn = (std::vector<glm::vec3>*)position->channel;
		cnn->resize(sz);

		for (auto attr : _attributeMap) attr.second->resize(sz);

		notifyObservers(pstate::RESIZE);
	}

	size_t PointCloud::attributeType(std::string attrName) {

		auto itr = _attributeMap.find(attrName);

		if (itr != _attributeMap.end()) {
			return itr->second->type;
		}

		return 0xf4240;
	}

	Channel *PointCloud::channel(std::string attrName) {

		auto itr = _attributeMap.find(attrName);

		if (itr != _attributeMap.end()) {
			return itr->second;
		}

		return nullptr;
	}

	size_t PointCloud::attributeCount() {
		return _attributeMap.size();
	}

	bool PointCloud::removeAttribute(std::string attrName) {
		if (ds::to_lower(attrName) == "@position") return false;
	
		auto itr = _attributeMap.find(attrName);

		if (itr != _attributeMap.end()) {
			_attributeMap.erase(itr);
			return true;
		}

		return false;
	}


	void PointCloud::clear() {
	
		for (auto attr : _attributeMap) attr.second->clear();

		notifyObservers(pstate::CLEAR);
	}

	void PointCloud::removeAllAttributes() {

		for (auto attr : _attributeMap) {
			if (ds::to_lower(attr.first) == "@position") continue;

			attr.second->free_channel();
			delete attr.second;
		}

		_attributeMap.clear();
		_attributeMap["@position"] = position;

		notifyObservers(pstate::REMOVE_ATTRIBUTE);
	}

	void PointCloud::addObserver(PointCloudObserver *obs) {
		obs->setParent(this);
		_observers[obs->id()] = obs;
	}

	void PointCloud::removeObserver(PointCloudObserver *obs) {
		_observers.erase(obs->id());
	}

	void PointCloud::notifyObservers(size_t state) {
		for (auto obs : _observers) obs.second->update(state);
	}
};
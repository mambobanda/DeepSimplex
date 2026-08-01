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
	* @file ...h
	*
	* @author Mambo Banda
	*
	*/

#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <typeinfo>
#include "DsType.h"
#include "DsCommon.h"
#include "DsChannel.h"
#include "DsPointCloudState.h"

namespace ds {

	class PointCloud {
	private:
		Channel *position;
		std::unordered_map<std::string, Channel*> _attributeMap;
	private:
		std::unordered_map<size_t, PointCloudObserver*> _observers;
	public:

		PointCloud();
		virtual ~PointCloud();

		size_t size();

		void resize(size_t);

		Channel *channel(std::string attrName);

		size_t attributeType(std::string attrName);

		size_t attributeCount();

		bool removeAttribute(std::string attrName);

		void clear();

		void removeAllAttributes();

		void addObserver(PointCloudObserver *obs);

		void removeObserver(PointCloudObserver *obs);

		void notifyObservers(size_t state);

		template<typename T>
		void set(size_t attrIndex, size_t index, T val) {

			notifyObservers(pstate::SET);
		}

		template<typename T>
		void set(std::string attrName, size_t index, T val) {

			notifyObservers(pstate::SET);
		}

		template<typename T>
		void addAttribute(std::string attrName) {

			if (ds::to_lower(attrName) == "position") return;
		
			unsigned int type = 0;
			bool validType = false;
			bool isString = false;

			if (std::is_same_v<T, int>) {
				type = ds::type::INT;
				validType = true;
			}
			else if (std::is_same_v<T, size_t>) {
				type = ds::type::SIZE_T;
				validType = true;
			}
			else if (std::is_same_v<T, unsigned int>) {
				type = ds::type::UINT;
				validType = true;
			}
			else if (std::is_same_v<T, float>) {
				type = ds::type::FLOAT;
				validType = true;
			}
			else if (std::is_same_v<T, double>) {
				type = ds::type::DOUBLE;
				validType = true;
			}
			else if (std::is_same_v<T, std::string>) {
				type = ds::type::STRING;
				validType = true;
			}
			else if (std::is_same_v<T, char*>) {
				type = ds::type::STRING;
				validType = true;
				isString = true;
			}
			else if (std::is_same_v<T, const char*>) {
				type = ds::type::STRING;
				validType = true;
				isString = true;
			}
			else if (std::is_same_v<T, bool>) {
				type = ds::type::BOOL;
				validType = true;
			}
			else if (std::is_same_v<T, char>) {
				type = ds::type::CHAR;
				validType = true;
			}
			else if (std::is_same_v<T, glm::vec2>) {
				type = ds::type::VEC2;
				validType = true;
			}
			else if (std::is_same_v<T, glm::vec3>) {
				type = ds::type::VEC3;
				validType = true;
			}
			else if (std::is_same_v<T, glm::vec4>) {
				type = ds::type::VEC4;
				validType = true;
			}
			else if (std::is_same_v<T, glm::dvec2>) {
				type = ds::type::DVEC2;
				validType = true;
			}
			else if (std::is_same_v<T, glm::dvec3>) {
				type = ds::type::DVEC3;
				validType = true;
			}
			else if (std::is_same_v<T, glm::dvec4>) {
				type = ds::type::DVEC4;
				validType = true;
			}
			else if (std::is_same_v<T, glm::ivec2>) {
				type = ds::type::IVEC2;
				validType = true;
			}
			else if (std::is_same_v<T, glm::ivec3>) {
				type = ds::type::IVEC3;
				validType = true;
			}
			else if (std::is_same_v<T, glm::ivec4>) {
				type = ds::type::IVEC4;
				validType = true;
			}
			else if (std::is_same_v<T, glm::mat3>) {
				type = ds::type::MAT3;
				validType = true;
			}
			else if (std::is_same_v<T, glm::mat4>) {
				type = ds::type::MAT4;
				validType = true;
			}
			else if (std::is_same_v<T, glm::dmat3>) {
				type = ds::type::DMAT3;
				validType = true;
			}
			else if (std::is_same_v<T, glm::dmat4>) {
				type = ds::type::DMAT4;
				validType = true;
			}
			else if (std::is_same_v<T, glm::quat>) {
				type = ds::type::QUAT;
				validType = true;
			}
			else {
				throw std::runtime_error("ds::PointCloud Error: addAttribute() unknown type.");
			}

			if (validType) {

				Channel *cnn = new Channel;
				cnn->type = type;
				cnn->name = attrName;

				if (!isString) {
					auto v = new std::vector<T>();
					cnn->channel = v;

					if (size() > 0) v->resize(size());
				}
				else {
					auto v = new std::vector<std::string>();
					cnn->channel = v;

					if (size() > 0) v->resize(size());
				}

				_attributeMap[attrName] = cnn;

				notifyObservers(pstate::ADD_ATTRIBUTE);
			}
		}

		template<typename T>
		void fill(std::string attrName, T val) {
			auto itr = _attributeMap.find(attrName);

			if (itr != _attributeMap.end()) {
				auto cnn = (std::vector<T>*)itr->second->channel;
				for (size_t inx = 0; inx < cnn->size(); inx++) (*cnn)[inx] = val;

				notifyObservers(pstate::SET);
			}
		}

		template<typename T>
		T *at(std::string attrName) {

			auto itr = _attributeMap.find(attrName);

			if (itr != _attributeMap.end()) {
				auto cnn = (std::vector<T>*)itr->second->channel;
				return &(*cnn)[0];
			}

			return nullptr;
		}

	};
}
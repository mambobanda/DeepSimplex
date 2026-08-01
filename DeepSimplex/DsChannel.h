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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "DsCommon.h"
#include "DsType.h"

namespace ds {

	class Channel {
	private:
		std::string name;
		size_t type;

		void *channel;

		friend class PointCloud;

		void free_channel();
		void clear();
		void resize(size_t);
	public:
		virtual ~Channel();

		template<typename T>
		T *at() {
			auto cnn = (std::vector<T>*)channel;
			return &(*cnn)[0];
		}

		size_t cType();
	};
}
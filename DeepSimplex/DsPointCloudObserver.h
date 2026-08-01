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

#include "DsCommon.h"

namespace ds {

	class PointCloudObserver {
	private:
		static size_t ID_GEN;
		size_t _id;

		PointCloud *_parent;
	public:

		PointCloudObserver();
		virtual ~PointCloudObserver();

		void setParent(PointCloud *parent);

		size_t id();

		void update(unsigned int state);

		virtual void update(unsigned int state, PointCloud *_parent) = 0;
	};
}
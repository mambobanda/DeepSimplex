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

#include "DsPointCloudObserver.h"
#include "DsPointCloud.h"

namespace ds {

	size_t PointCloudObserver::ID_GEN = 0xff;

	PointCloudObserver::PointCloudObserver() {
		_id = ID_GEN++;
		_parent = nullptr;
	}

	PointCloudObserver::~PointCloudObserver() {
		if (_parent != nullptr) {
			_parent->removeObserver(this);
		}
	}

	void PointCloudObserver::setParent(PointCloud *parent) {
		_parent = parent;
	}

	size_t PointCloudObserver::id() {
		return _id;
	}

	void PointCloudObserver::update(unsigned int state) {
		update(state, _parent);
	}

}
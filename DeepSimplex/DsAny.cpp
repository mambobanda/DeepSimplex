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

#include "DsAny.h"

namespace ds {

	Any::Any() {
		_value = nullptr;
	}

	Any::~Any() {
		clear();
	}

	bool Any::isNull() const{
		return (_value == nullptr);
	}

	void Any::clear() {
		delete _value;
	}
}
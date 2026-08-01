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

namespace ds {

	class Any {
	private:
		void *_value;
	public:

		Any();

		virtual ~Any();

		bool isNull() const;

		void clear();

		template<typename T>
		inline void setValue(T val) {
			delete _value;
			T *t_val = new T;
			*t_val = val;
			_value = t_val;
		}

		template<typename T>
		inline T getValue() {
			T *t_val = (T*)_value;
			return *t_val;
		}
	};

}
/*
 * Klass.hpp
 *
 *  Created on: Feb 10, 2025
 *      Author: swalton
 */

#ifndef KLASS_HPP_
#define KLASS_HPP_
#include <stdint.h>
#include <iostream>

typedef uint8_t uchar;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint32_t wchar;
typedef __int128_t int128_t;

typedef const char cchar;
class String;

	class Klass {
		public:
			Klass(void) {}
			virtual ~Klass(void) {}

		public:
//			virtual cchar* getChars(void) const = 0;
			virtual String toString(void) const = 0;
			virtual String serialize(void) const = 0;
			virtual Klass *clone(void) const = 0;
/*??*/		friend std::ostream& operator<<(std::ostream& stream, const Klass& object);
	};

#endif /* KLASS_HPP_ */

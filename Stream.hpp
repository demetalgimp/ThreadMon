//editor: tab=4
/*
 * Stream.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef __STREAM__
#define __STREAM__

#include "String.hpp"

	class Stream: public Klass {
		friend class UnitTests;

		protected:
			uint linenum = 1;

		public:
			virtual ~Stream(void) {}

		public:
			uint getLineNumber(void) { return linenum; }

		public:
			virtual int  back(void) = 0;
			virtual int  current(void) = 0;
			virtual int  next(void) = 0;
			virtual void setBookmark(void) = 0;
			virtual void recallBookmark(void) = 0;
			virtual int  peek(uint ahead = 0) const = 0;
			virtual bool peek(const String& seed, bool consume = false) = 0;
			virtual bool isEOF(void) const = 0;
			virtual void skip(uint chars = 1) = 0;
			virtual uint getLength(void) const = 0;
			virtual uint getPosition(void) const = 0;
	};

#endif

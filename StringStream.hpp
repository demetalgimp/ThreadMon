//editor: tab=4
/*
 * StringStream.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef STRINGSTREAM_HPP_
#define STRINGSTREAM_HPP_

#include "Stream.hpp"

	class StringStream: public Stream {
		friend class UnitTests;

		String string;
		bool first_time = true;
		uint position = 0;
		uint bookmark = 0;

		public:
/**/		StringStream(const String& string): string(string) {}

		public:
/**/		virtual int  current(void);
/**/		virtual int  next(void);
/**/		virtual int  peek(uint ahead = 0) const;
/**/		virtual bool peek(const String& seed, bool consume = false); // <-- method not 'const' b/c revises object if "skip_seed" true
/**/		virtual int  back(void);
/**/		virtual void skip(uint chars = 1);
/**/		virtual void setBookmark(void)				{ bookmark = position; }
/**/		virtual void recallBookmark(void)			{ position = bookmark; bookmark = 0; first_time = true; }

		public:
/**/		virtual int operator*(void)					{ return current(); }
/**/		virtual int operator++(void)				{ return next(); }
/**/		virtual int operator--(void)				{ return back(); }

		public:
/**/		virtual bool isEOF(void) const				{ return (position >= string.getLength()); }
/**/		virtual uint getLength(void) const			{ return string.getLength(); }
/**/		virtual uint getPosition(void) const		{ return position; }
/**/		virtual cchar* getChars(void) const			{ return string.getChars() + position; }
/**/		virtual String toString(void) const			{ return String::formatString("[position=%u stream=\"%s\"]", position, string.getChars()); }
/**/friend	std::ostream& operator<<(std::ostream& stream, const StringStream& string) { return (stream << string.toString()); }
	};

#endif /* STRINGSTREAM_HPP_ */

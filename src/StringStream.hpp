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
/*tested*/	StringStream(const String& string): string(string) {}

		public:
/*tested*/	virtual int  current(void);
/*tested*/	virtual int  next(void);
/*tested*/	virtual int  peek(uint ahead = 0) const;
/*tested*/	virtual bool peek(const String& seed, bool consume = false); // <-- method not 'const' b/c revises object if "skip_seed" true
/*tested*/	virtual int  back(void);
/*tested*/	virtual void skip(uint chars = 1);
/*tested*/	virtual void setBookmark(void)				{ bookmark = position; }
/*tested*/	virtual void recallBookmark(void)			{ position = bookmark; bookmark = 0; first_time = true; }

		public:
/*tested*/	virtual int operator*(void)					{ return current(); }
/*tested*/	virtual int operator++(void)				{ return next(); }
/*tested*/	virtual int operator--(void)				{ return back(); }

		public: // Getters/setters
/*tested*/	virtual bool isEOF(void) const					{ return (position >= string.getLength()); }
/*tested*/	virtual uint getLength(void) const				{ return string.getLength(); }
/*tested*/	virtual uint getPosition(void) const			{ return position; }
///*tested*/	virtual cchar* getChars(void) const				{ return string.getChars() + position; }

		public: //Overrides
/*??*/		virtual Klass *clone(void) const override		{ return new StringStream(*this); }
/*??*/		virtual String toString(void) const override	{ return String::formatString("[StringStream: %llX]", this); }
			virtual String serialize(void) const override;

/*tested*/	friend std::ostream& operator<<(std::ostream& stream, const StringStream& string) { return (stream << string.toString()); }
	};

#endif /* STRINGSTREAM_HPP_ */

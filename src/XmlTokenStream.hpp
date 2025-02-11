//editor: tab=4
/*
 * XmlTokenStream.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef XMLTOKENSTREAM_HPP_
#define XMLTOKENSTREAM_HPP_

#include "String.hpp"
#include "Token.hpp"
#include "TokenStream.hpp"

	class XmlTokenStream: public TokenStream {
		friend class UnitTests;

		public:
/*tested*/	XmlTokenStream(Stream *stream): TokenStream(stream) {}
/*tested*/	XmlTokenStream(const String& text): TokenStream(new StringStream(text)) {}

		protected:
/*tested*/	virtual String scoopSpace(void) override;
/*tested*/	virtual Token  scoopTag(const String& spaces);
/*tested*/	virtual Token  scoopString(const String& spaces) override;
/*tested*/	virtual Token  scoopNumber(const String& spaces) override { return Token(); }

		public:
/*tested*/	virtual Token  next(void) override;
/*??*/		virtual Klass *clone(void) const override		{ return new XmlTokenStream(*this); }
/*??*/		virtual String toString(void) const override	{ return String::formatString("[XmlTokenStream: %llX]", this); }
/*??*/		virtual String serialize(void) const override {
				return "{ \"XmlTokenStream\": " + TokenStream::serialize() + " }";
			}

		public:
/*tested*/	friend std::ostream& operator<<(std::ostream& stream, const XmlTokenStream& tokens);
	};

#endif /* XMLTOKENSTREAM_HPP_ */

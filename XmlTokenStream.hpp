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
/**/		XmlTokenStream(Stream *stream): TokenStream(stream) {}
/**/		XmlTokenStream(const String& text): TokenStream(new StringStream(text)) {}


		protected:
/**/		virtual String scoopSpace(void) override;
/**/		virtual Token  scoopTag(const String& spaces);
/**/		virtual Token  scoopString(const String& spaces) override;
/**/		virtual Token  scoopNumber(const String& spaces) override { return Token(); }

		public:
/**/		virtual Token  next(void) override;
			virtual String toString(void) const override { return "XmlTokenStream: " + TokenStream::toString(); }

		public:
/**/friend 	std::ostream& operator<<(std::ostream& stream, const XmlTokenStream& tokens);
	};

//	class XmlTagTokenStream: public TokenStream {
//		friend class UnitTests;
//
//		public:
///**/		XmlTagTokenStream(Stream *stream): TokenStream(stream) {}
///**/		XmlTagTokenStream(const String& string): TokenStream(new StringStream(string)) {}
//
//		public:
///**/		virtual Token next(void) override;
///**/		void mustBe(const std::set<Token>& tokens);
///**/		void mustBe(const Token& token);
///**/		bool mayBe(const std::set<Token>& tokens);
///**/		bool mayBe(const Token& token);
//
//		public:
//			virtual String toString(void) const override { return "XmlTagTokenStream: " + TokenStream::toString(); }
//
//		public:
//			friend 	std::ostream& operator<<(std::ostream& stream, const XmlTokenStream& tokens);
//	};

#endif /* XMLTOKENSTREAM_HPP_ */

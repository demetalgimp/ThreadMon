//editor: tab=4
/*
 * CppTokenStream.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef CPPTOKENSTREAM_HPP_
#define CPPTOKENSTREAM_HPP_

#include "String.hpp"
#include "Token.hpp"
#include "TokenStream.hpp"

	class CppTokenStream: public TokenStream {
		friend class UnitTests;

		public:
/**/		CppTokenStream(Stream *stream): TokenStream(stream) {}
/**/		CppTokenStream(const String& string): TokenStream(new StringStream(string)) {}

		private: //--- TokenStream overrides
/**/		virtual String scoopSpace(void) override;
/**/		virtual Token  scoopNumber(const String& spaces) override;
/**/		virtual Token  scoopWord(const String& spaces) override;

		public: //--- TokenStream overrides
/**/		virtual Token  next(void);

		public: //--- Klass overrides
			virtual String toString(void) const override { return "CppTokenStream: " + TokenStream::toString(); }
	};

#endif /* CPPTOKENSTREAM_HPP_ */

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
/*tested*/	CppTokenStream(Stream *stream): TokenStream(stream) {}
/*tested*/	CppTokenStream(const String& string): TokenStream(new StringStream(string)) {}

		private: //--- TokenStream overrides
/*tested*/	virtual String scoopSpace(void) override;
/*tested*/	virtual Token  scoopNumber(const String& spaces) override;
/*tested*/	virtual Token  scoopWord(const String& spaces) override;

		public: //--- TokenStream overrides
/*tested*/	virtual Token  next(void);

		public: //--- Klass overrides
/*??*/		virtual String toString(void) const override { return "CppTokenStream: " + TokenStream::toString(); }
	};

#endif /* CPPTOKENSTREAM_HPP_ */

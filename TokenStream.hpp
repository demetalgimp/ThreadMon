//editor: tab=4
/*
 * TokenStream.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef __TOKEN_STREAM__
#define __TOKEN_STREAM__

#include <ctype.h>
#include <vector>
#include <map>
#include <set>

#include "Token.hpp"
#include "Stream.hpp"
#include "StringStream.hpp"

	class TokenStream: public Klass {
		friend class UnitTests;

		protected:
			Stream *stream;
			Token current_token;

		public:
/**/		TokenStream(Stream *stream): stream(stream) {}
/**/		TokenStream(const String& string): stream(new StringStream(string)) {}
/**/		virtual ~TokenStream(void) { delete stream; }

		protected:
/**/		virtual String scoopSpace(void);
/**/		virtual Token  scoopWord(const String& spaces);
/**/		virtual void   scoopDigits(TextAccumulator& acc);
/**/		virtual Token  scoopNumber(const String& spaces);
/**/		virtual Token  scoopString(const String& spaces);

		public:
/**/		virtual Token current(void);
/**/		virtual Token next(void);
/**/		virtual void setBookmark(void) 				{ stream->setBookmark(); }

//NOTE: TokenStream::recallBookmark() differs from the Stream::recallBookmark().
//		Stream's current() after recallBookmark() returns the *last* letter.
//		TokenStream's current() after recallBookmark() returns the *current* token.
//	REASON: TokenStream's index into the stream is updated with at _least_ one character. Tracking the _last_
//		index introduces more complexity than useful.
/**/		virtual void recallBookmark(void)			{ stream->recallBookmark(); current_token.type = eEmpty; }
			virtual Token mustBe(const std::set<Token>& tokens);
			virtual Token mustBe(const Token& token);
			virtual Token mayBe(const std::set<Token>& tokens);
			virtual Token mayBe(const Token& token);
//BROKEN!
//			virtual void setStream(Stream *stream) {
//				delete stream;
//				this->stream = stream;
//				current_token = Token();
//			}
/**/		virtual bool  isEOF(void) 					{ return (stream->isEOF()  ||  current_token.type == eEOF); }

		public: //--- Klass overrides
/**/		virtual cchar* getChars(void) const override { return "TokenStream"; }
/**/		virtual String toString(void) const override;

		public:
/**/friend 	std::ostream& operator<<(std::ostream& stream, const TokenStream& tokens);
	};

#endif





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
/*tested*/	TokenStream(Stream *stream): stream(stream) {}
/*tested*/	TokenStream(const String& string): stream(new StringStream(string)) {}
/*tested*/	virtual ~TokenStream(void) { delete stream; }

		protected:
/*tested*/	virtual String scoopSpace(void);
/*tested*/	virtual Token  scoopWord(const String& spaces);
/*tested*/	virtual void   scoopDigits(TextAccumulator& acc);
/*tested*/	virtual Token  scoopNumber(const String& spaces);
/*tested*/	virtual Token  scoopString(const String& spaces);

		public:
/*tested*/	virtual Token current(void);
/*tested*/	virtual Token next(void);
/*tested*/	virtual void setBookmark(void) 						{ stream->setBookmark(); }

//NOTE: TokenStream::recallBookmark() differs from the Stream::recallBookmark().
//		Stream's current() after recallBookmark() returns the *last* letter.
//		TokenStream's current() after recallBookmark() returns the *current* token.
//	REASON: TokenStream's index into the stream is updated with at _least_ one character. Tracking the _last_
//		index introduces more complexity than useful.
/*tested*/	virtual void recallBookmark(void)					{ stream->recallBookmark(); current_token.type = eEmpty; }
/*tested*/	virtual Token mustBe(const std::set<Token>& tokens);
/*tested*/	virtual Token mustBe(const Token& token);
/*tested*/	virtual Token mayBe(const std::set<Token>& tokens);
/*tested*/	virtual Token mayBe(const Token& token);
/*tested*/	virtual bool  isEOF(void) 							{ return (stream->isEOF()  ||  current_token.type == eEOF); }

		public: //--- Klass overrides
/*tested*/	virtual cchar* getChars(void) const override		{ return "TokenStream"; }
/*??*/		virtual Klass *clone(void) const				{ return new TokenStream(*this); }
/*tested*/	virtual String toString(void) const override;

		public:
/*tested*/	friend 	std::ostream& operator<<(std::ostream& stream, const TokenStream& tokens);
	};

#endif




	//BROKEN! There's no way to reuse a stream at this time.
	//			virtual void setStream(Stream *stream) {
	//				delete stream;
	//				this->stream = stream;
	//				current_token = Token();
	//			}

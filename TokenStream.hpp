//editor: tab=4
#ifndef __TOKEN_STREAM__
#define __TOKEN_STREAM__

#include <ctype.h>
#include <vector>
#include <map>
#include <set>
#include "Stream.hpp"

	class TokenStream: public Klass {
		protected:
			Stream *stream;
			Token current_token;

		public:
/**/		TokenStream(Stream *stream): stream(stream) {}
/**/		TokenStream(const String& string): stream(new StringStream(string)) {}
/**/		virtual ~TokenStream(void) { delete stream; }

//		protected:
/**/		virtual String scoopSpace(void);
/**/		virtual Token  scoopWord(const String& spaces);
/**/		virtual void   scoopDigits(TextAccumulator& acc);
/**/		virtual Token  scoopNumber(const String& spaces);
/**/		virtual Token  scoopString(const String& spaces);

		public:
/**/		virtual bool  isEOF(void) { return (stream->isEOF()  ||  current_token.type == eEOF); }
/**/		virtual Token current(void);
/**/		virtual Token next(void);
//			virtual void setStream(Stream *stream) {
//				delete stream;
//				this->stream = stream;
//				current_token = Token();
//			}

		public: //--- Klass overrides
/**/		virtual cchar* getChars(void) const override { return "TokenStream"; }
/**/		virtual String toString(void) const override;

		public:
/**/friend 	std::ostream& operator<<(std::ostream& stream, const TokenStream& tokens);
	};

	class CppTokenStream: public TokenStream {
		public:
/**/		CppTokenStream(Stream *stream): TokenStream(stream) {}

		public: //--- TokenStream overrides
/**/		virtual String scoopSpace(void) override;
/**/		virtual Token  scoopNumber(const String& spaces) override;
/**/		virtual Token  scoopWord(const String& spaces) override;

		public: //--- TokenStream overrides
/**/		virtual Token  next(void);

		public: //--- Klass overrides
			virtual String toString(void) const override;
	};

	class XmlTokenStream: public TokenStream {
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
			virtual String toString(void) const override { return "XmlTokenStream"; }
	};

	class TagTokenStream: public TokenStream {
		public:
/**/		TagTokenStream(Stream *stream): TokenStream(stream) {}
/**/		TagTokenStream(const String& string): TokenStream(new StringStream(string)) {}

		public:
/**/		virtual Token next(void) override;
			void mustBe(const std::set<Token>& tokens);
			void mustBe(const Token& token);
			bool mayBe(const std::set<Token>& tokens);
			bool mayBe(const Token& token);

		public:
			virtual String toString(void) const override { return "TagTokenStream"; }
	};

#endif





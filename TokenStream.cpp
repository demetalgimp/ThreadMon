//editor: tab=4
#include <stdlib.h>
#include <arpa/inet.h>
#include "TokenStream.hpp"

	String TokenStream::scoopSpace(void) {
		TextAccumulator acc;
		while ( isspace(stream->current()) ) {
			acc += stream->current();
			stream->next();
		}
		String spaces = acc.toString();
		return acc.toString();
	}

	Token TokenStream::scoopWord(const String& spaces) {
		TokenType type = TokenType::eEOF;
		TextAccumulator acc;
		if ( stream->current() != EOF ) {
			while ( Token::isWord(stream->current()) ) {
				acc += stream->current();
				stream->next();
			}
			type = TokenType::eWord;
		}
		current_token = Token(type, acc.toString(), spaces);
		return current_token;
	}

//--- consumed by children
	Token TokenStream::scoopString(const String& spaces) {
		TextAccumulator acc;
		char delimiter = stream->current();
		acc += delimiter;
		while ( stream->next() != EOF ) {
			if ( stream->current() == delimiter ) {
				acc += stream->current();
				stream->next();
				break;
			}

			acc += stream->current();
			if ( stream->current() == '\\' ) {
				acc += stream->next();
			}
		}
		return Token((delimiter == '"'? eString: eChar), acc.toString(), spaces);
	}

	void TokenStream::scoopDigits(TextAccumulator& acc) {
		int (*is_digit)(int) = (acc.getLength() > 1  &&  tolower(acc[1]) == 'x'? isxdigit: isdigit);

		while ( is_digit(stream->current()) ) {
			acc += stream->current();
			stream->next();
		}
	}

//--- Integer only
	Token TokenStream::scoopNumber(const String& spaces) {
		TokenType type = TokenType::eEOF;
		TextAccumulator acc;
		if ( stream->current() != EOF ) {
			while ( isdigit(stream->current()) ) {
				acc += stream->current();
				stream->next();
			}
			type = TokenType::eNumber;
		}
		current_token = Token(type, acc.toString(), spaces);
		return current_token;
	}

	Token TokenStream::current(void) {
		if ( current_token.type == eEmpty ) {
			return next();
		}
		return current_token;
	}

	Token TokenStream::next(void) {
		String spaces = scoopSpace();

		int c = stream->current();

		if ( isdigit(c) ) {
			current_token = scoopNumber(spaces);

		} else if ( Token::isWordStart(c) ) {
			current_token = scoopWord(spaces);

		} else {
			current_token = Token((TokenType)c);
			stream->next();
		}
		return current_token;
	}

	String TokenStream::toString(void) const {
		return String::formatString("['%s': \"%s\", \"%s\"]", Token::wideCharToString(current_token.type).getChars(), current_token.text.getChars(), current_token.whitespace.getChars());
	}

	std::ostream& operator<<(std::ostream& ostream, const TokenStream& tokenstream) {
		return ostream << tokenstream.current_token.whitespace + tokenstream.current_token.text;
	}

//=== CppTokenStream ================================================================================================================================
	Token CppTokenStream::scoopNumber(const String& spaces) {
		TextAccumulator acc;
		TokenType type = eNumber;

	//--- integer portion
		if ( stream->current() == '0'  &&  tolower(stream->peek(1)) == 'x' ) {//  &&  (isxdigit(stream->peek(1))) ) {
			acc += stream->current();
			acc += stream->next();
			stream->next();
//FIXME: test pass/fail
			scoopDigits(acc);

		} else {
//FIXME: test pass/fail
			scoopDigits(acc);

		//--- decimal portion
			if ( stream->current() == '.' ) {
				acc += '.';
				stream->next();
				scoopDigits(acc);
//FIXME: test pass/fail
				type = eFloat;
			}

		//--- exponent portion
			if ( tolower(stream->current()) == 'e' ) {
				acc += stream->current();
				stream->next();

			//--- check for sign
				if ( stream->current() == '-'  ||  stream->current() == '+' ) {
					acc += stream->current();
					stream->next();
				}

			//--- 123435e12
				if ( isdigit(stream->current()) ) {
//FIXME: test pass/fail
					scoopDigits(acc);
				}

				type = eFloat;
			}
		}

//TODO: finish this!!
	//---int <= {u/l/s, su/SU, us/US, lu/LU, ls/LS}
		if ( type != eFloat ) {

	//---float <= {f/F, fl/FL, lf/LF}
		} else if ( toupper(stream->current()) == 'U'  ||  toupper(stream->current()) == 'F'  ||  toupper(stream->current()) == 'L') {

		}
		current_token = Token(type, acc.toString(), spaces);
		return current_token;
	}

	String CppTokenStream::scoopSpace(void) {
		TextAccumulator acc;
		bool done = false;
		while ( !done ) {
			done = true;

		//--- white space
			if ( isspace(stream->current()) ) {
				done = false;
				while ( isspace(stream->current()) ) {
					acc += stream->current();
					stream->next();
				}
			}

		//--- block comment
			if ( stream->peek("/*", true) ) {
				acc += "/*";
				while ( !stream->isEOF() ) {
					acc += stream->current();
					if ( stream->peek("*/", true) ) {
						acc += '/';
						break;
					}
					stream->next();
				}
				done = false;
			}

		//--- line comment
			if ( stream->peek("//", true) ) {
				acc += "//";
				while ( !stream->isEOF() ) {
					acc += stream->current();
					if ( stream->current() == '\n'  ||  stream->current() == '\r' ) {
						stream->next();
						break;
					}
					stream->next();
				}
				done = false;
			}
		}
		return acc.toString();
	}

	Token CppTokenStream::scoopWord(const String& spaces) {
		TokenType type = TokenType::eUnknown;
		char tmps[100];
		uint index = 0;
		tmps[index] = 0;
		if ( Token::isWordStart(stream->current()) ) {
			do {
				tmps[index++] = stream->current();
			} while ( Token::isWord(stream->next()) );
			tmps[index] = 0;
			type = TokenType::eWord;
		}
		current_token = Token(type, tmps, spaces);
		return current_token;
	}

	Token CppTokenStream::next(void) {
		String spaces = scoopSpace();
		int c = stream->current();
		if ( c != eEOF ) {
			if ( Token::isWordStart(c) ) {
				current_token = scoopWord(spaces);

			} else if ( isdigit(c) ) {
				current_token = scoopNumber(spaces);

			} else if ( c == '\''  ||  c == '"' ) {
				current_token = scoopString(spaces);

			} else {
				//NOTE: order is important!
				static const Token special_symbols[] = {
					Token(eElipsis, "..."),          Token(eMember, "."),
					Token(eBitNotAssign, "~="),      Token(eBitNot, "~"),
					Token(eNotEquals, "!="),         Token(eBoolNot, "!"),
					Token(eModulusAssign, "%="),     Token(eModulus, "%"),
					Token(eBitXorAssign, "^="),      Token(eBitXor, "^"),
					Token(eBoolAndAssign, "&&="),    Token(eBoolAnd, "&&"),     Token(eBitAndAssign, "&="),  Token(eBitAnd, "&"),
					Token(eMinusAssign, "-="),       Token(eDereference, "->"), Token(eDecrement, "--"),     Token(eMinus, "-"),
					Token(eEqualsTo, "=="),          Token(eAssign, "="),
					Token(eSumAssign, "+="),         Token(eIncrement, "++"),   Token(eSum, "+"),
					Token(eBoolOrAssign, "||="),     Token(eBoolOr, "||"),      Token(eBitOrAssign, "|="),   Token(eBitOr, "|"),
					Token(eShiftLeftAssign, "<<="),  Token(eShiftLeft, "<<"),   Token(eLessEquals, "<="),    Token(eLessThan, "<"),
					Token(eShiftRightAssign, ">>="), Token(eShiftRight, ">>"),  Token(eGreaterEquals, ">="), Token(eGreaterThan, ">"),
					Token(eDivideAssign, "/="),      Token(eDivide, "/"),
					Token(eMultiplyAssign, "*="),    Token(eMultiply, "*"),
					Token(eScope, "::"),             Token(eColon, ":"),
				};

				current_token = Token();
				for ( Token token : special_symbols ) {
					if ( stream->peek(token.text, true) ) {
						current_token = Token(token.type, token.text, spaces);
						break;
					}
				}

				if ( current_token.type == TokenType::eEmpty ) {
					TokenType type = (TokenType)stream->current();
					String text = Token::wideCharToString(type).strip();
					current_token = Token(type, text, spaces);
					stream->next();
				}
			}

		} else {
			current_token = Token(eEOF, "", spaces);
		}
		return current_token;
	}

	String CppTokenStream::toString(void) const {
		char tmps[current_token.text.getLength() + 16];
		snprintf(tmps, sizeof(tmps), "['%c'(%d): \"%s\"]", current_token.type, current_token.type, current_token.text.getChars());
		return String(tmps);
	}

//=== XMLTokenStream ================================================================================================================================
	String XmlTokenStream::scoopSpace(void) {
		if ( stream->peek("<!", false) ) {
			TextAccumulator acc;
			do {
				acc += stream->next();
			} while ( !(stream->isEOF()  ||  stream->peek("-->", true)) );

			if ( !stream->isEOF() ) {
				acc += "-->";
			}
			return acc.toString();

		} else {
			return "";
		}
	}

	Token XmlTokenStream::scoopTag(const String& spaces) {
		char tmps[1024];
		uint index = 0;
		TokenType type = eStartTag;

		memset(tmps, 0, sizeof(tmps));
		tmps[index++] = stream->current();

	//--- "</tag>"
		if ( stream->next() == '/' ) {
			type = eEndTag;
			tmps[index++] = stream->current();
			if ( Token::isWord(stream->next()) ) {
				do {
					tmps[index++] = stream->current();
				} while ( Token::isWord(stream->next())  &&  stream->current() != '>'  &&  !stream->isEOF() );

				if ( stream->current() == '>' ) {
					tmps[index++] = stream->current();

				} else {
					THROW_ERROR("malformed XML tag on line #%d: expecting %c but got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
				}

			} else {
				THROW_ERROR("malformed XML tag [line#%d]: expecting start of tag name after '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
			}

	//--- "<tag>" or "<tag/>"
		} else {
			tmps[index++] = stream->current();
			stream->next();
			while ( !stream->isEOF()  &&  stream->current() != '/'  &&  stream->current() != '>' ) {
				tmps[index++] = stream->current();
				if ( stream->next() == '"' ) {

					while ( !stream->isEOF()  &&  stream->current() != '"' ) {
						tmps[index++] = stream->current();
						if ( stream->next() == '\\' ) {
							tmps[index++] = stream->current();
							tmps[index++] = stream->next();
							stream->next();
						}
					}
				}
			}
			tmps[index++] = stream->current();
		}

	//--- end of eStartTag
		if ( stream->current() == '>' ) {
//			type = eStartTag; <-- already known
			stream->next();

	//--- end of eAtomTag
		} else if ( stream->current() == '/' ) {

			type = eAtomTag;
			if ( stream->next() != '>' ) {
//				throw String::formatString("Error[%s,%d]: [line#%d]: malformed XML tag: expecting terminal '>' after '/' but instead got %c (0x%2X)", __FILE__, __LINE__, stream->getLineNumber(), stream->current(), stream->current());
				THROW_ERROR("malformed XML tag[line#%d]: expecting terminal '>' after '/' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
			}
			tmps[index++] = stream->current();
			stream->next();

		} else {
//			throw String::formatString("Error[%s,%d]: [line#%d]: malformed XML tag: expecting terminal '>' but instead got %c (0x%2X)", __FILE__, __LINE__, stream->getLineNumber(), stream->current(), stream->current());
			THROW_ERROR("malformed XML tag[line#%d]: expecting terminal '>' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
		}

		tmps[index] = 0;
		return Token((TokenType)type, tmps, spaces);
	}

	Token XmlTokenStream::scoopString(const String& spaces) {
		char tmps[stream->getLength() - stream->getPosition() + 1];

		uint index = 0;
		while ( stream->current() != eEOF  &&  index < sizeof(tmps)  &&  stream->current() != '<' ) {
			tmps[index++] = stream->current();
			stream->next();
		}
		tmps[index] = 0;

		return Token((stream->current() != eEOF? eText: eEOF), tmps, spaces);
	}

	Token XmlTokenStream::next(void) {
		if ( isEOF() ) {
			current_token = Token(eEOF);

		} else {
			String spaces = scoopSpace();
			if ( stream->current() == '<' ) {
				current_token = scoopTag(spaces);

			} else {
				current_token = scoopString(spaces);
			}
		}
		return current_token;
	}

//=== TagTokenStream ================================================================================================================================

	Token TagTokenStream::next(void) {
		if ( !stream->isEOF() ) {
			String spaces = scoopSpace();

		//--- tag *or* attribute name
			if ( Token::isWordStart(stream->current()) ) {
				current_token = scoopWord(spaces);

		//--- attribute value (as string)
			} else if ( stream->current() == '\''  ||  stream->current() == '"' ) {
				current_token = scoopString(spaces);
				current_token.type = TokenType::eString;

		//--- end tag
			} else if ( stream->peek("</", true) ) {
				current_token = Token(TokenType::eEndTag);

		//--- atom tag
			} else if ( stream->peek("/>", true) ) {
				current_token = Token(TokenType::eAtomTag);

		//--- char ∈ {'<', '=', '>'}
			} else {
				current_token = Token((TokenType)stream->current());
				current_token.whitespace = spaces;
				stream->next();
			}

		} else {
			current_token = Token(eEOF);
		}
		return current_token;
	}

	void TagTokenStream::mustBe(const std::set<Token>& tokens) {
		String spaces = scoopSpace();
		Token token = current();
		if ( tokens.find(token) == tokens.end() ) {
			THROW_ERROR("malformed XML tag[line#%d]: expecting '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
		}
	}
	void TagTokenStream::mustBe(const Token& token) {
		String spaces = scoopSpace();
		if ( token != current() ) {
			THROW_ERROR("malformed XML tag[line#%d]: expecting '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
		}
	}
	bool TagTokenStream::mayBe(const std::set<Token>& tokens) {
		String spaces = scoopSpace();
		Token token = current();
		return ( tokens.find(token) == tokens.end() );
	}
	bool TagTokenStream::mayBe(const Token& token) {
		String spaces = scoopSpace();
		return ( token == current() );
	}



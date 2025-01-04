//editor: tab=4
/*
 * TokenStream.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

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

		} else if ( c == '"'  ||  c == '\'' ) {
			current_token = scoopString(spaces);
			current_token.type = TokenType::eString;

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

	Token TokenStream::mustBe(const std::set<Token>& tokens) {
		Token token = current();
		if ( token == TokenType::eEOF  ||  tokens.find(token) == tokens.end() ) {
			THROW_ERROR("malformed XML tag[line#%d]: expecting '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
		}
		next();
		return token;
	}

	Token TokenStream::mustBe(const Token& expected_token) {
		Token token = current();
		if ( expected_token != token.type ) {
			THROW_ERROR("malformed XML tag[line#%d]: expecting '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
		}
		next();
		return token;
	}

	Token TokenStream::mayBe(const std::set<Token>& tokens) {
		Token token = current();
		if ( tokens.find(token) != tokens.end() ) {
			next();
			return token;
		}
		return Token();
	}

	Token TokenStream::mayBe(const Token& expected_token) {
		Token token = current();
		if ( expected_token == token.type ) {
			next();
			return token;
		}
		return Token();
	}

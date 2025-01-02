/*
 * CppTokenStream.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#include "CppTokenStream.hpp"

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

//	String CppTokenStream::toString(void) const {
//		char tmps[current_token.text.getLength() + 16];
//		snprintf(tmps, sizeof(tmps), "['%c'(%d): \"%s\"]", current_token.type, current_token.type, current_token.text.getChars());
//		return String(tmps);
//	}





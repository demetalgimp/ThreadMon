/*
 * XmlTokenStream.cpp
 *
 *  Created on: Dec 30, 2024
 *      Author: swalton
 */

#include "String.hpp"
#include "Stream.hpp"
#include "TokenStream.hpp"
#include "XmlTokenStream.hpp"

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

//	Token XmlTagTokenStream::next(void) {
//		if ( !stream->isEOF() ) {
//			String spaces = scoopSpace();
//
//		//--- tag *or* attribute name
//			if ( Token::isWordStart(stream->current()) ) {
//				current_token = scoopWord(spaces);
//
//		//--- attribute value (as string)
//			} else if ( stream->current() == '\''  ||  stream->current() == '"' ) {
//				current_token = scoopString(spaces);
//				current_token.type = TokenType::eString;
//
//		//--- end tag
//			} else if ( stream->peek("</", true) ) {
//				current_token = Token(TokenType::eEndTag);
//
//		//--- atom tag
//			} else if ( stream->peek("/>", true) ) {
//				current_token = Token(TokenType::eAtomTag);
//
//		//--- char ∈ {'<', '=', '>'}
//			} else {
//				current_token = Token((TokenType)stream->current());
//				current_token.whitespace = spaces;
//				stream->next();
//			}
//
//		} else {
//			current_token = Token(eEOF);
//		}
//		return current_token;
//	}
//
//	void XmlTagTokenStream::mustBe(const std::set<Token>& tokens) {
//		String spaces = scoopSpace();
//		Token token = current();
//		if ( tokens.find(token) == tokens.end() ) {
//			THROW_ERROR("malformed XML tag[line#%d]: expecting '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
//
//		} else {
//			next();
//		}
//	}
//	void XmlTagTokenStream::mustBe(const Token& token) {
//		String spaces = scoopSpace();
//		if ( token != current().type ) {
//			THROW_ERROR("malformed XML tag[line#%d]: expecting '<' but instead got %c (0x%2X)", stream->getLineNumber(), stream->current(), stream->current());
//
//		} else {
//			next();
//		}
//	}
//	bool XmlTagTokenStream::mayBe(const std::set<Token>& tokens) {
//		String spaces = scoopSpace();
//		Token token = current();
//		if ( tokens.find(token) != tokens.end() ) {
//			next();
//			return true;
//		}
//		return false;
//	}
//	bool XmlTagTokenStream::mayBe(const Token& token) {
//		String spaces = scoopSpace();
//		if ( token == current().type ) {
//			next();
//			return true;
//		}
//		return false;
//	}




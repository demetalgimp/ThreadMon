//editor: tab=4
/*
 * Token.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include "String.hpp"

#pragma GCC diagnostic ignored "-Wmultichar"
	enum TokenType {
		eEOF = -1, eEmpty = 0, eUnknown,

		eAssign='=',             eBackslash='\\',     eBitAnd='&',            eBitNot='~',            eBitOr='|',
		eBitXor='^',             eBoolNot='!',        eColon=':',             eComma=',',             eDivide='/',
		eDot='.',                eGreaterThan='>',    eHash='#',              eLeftBrace='{',         eLeftBracket='[',
		eLeftParen='(',          eLessThan='<',       eMinus='-',             eModulus='%',           eMultiply='*',
		eMember='.',             eArithIf='?',        eRightBrace='}',        eRightBracket=']',      eRightParen=')',
		eSemicolon=';',          eSum='+',

		eAmpersand='&',          eAsterix='*',        eAt='@',                eBackQuote='`',         eBang='!',
		eBar='|',                eCircumflex='^',     eDollar='$',            eDoubleQuote='"',       eEquals='=',
		eExclamationPoint='!',   eHyphen='-',         eLeftAngleBracket='<',  ePercent='%',           ePeriod='.',
		ePlus='+',               eQuestionMark='?',   eRightAngleBracket='>', eSingleQuote='\'',      eSlash='/',
		eTilde='~',              eUnderscore='_',

		eString='str',           eChar='char',        eWord='word',           eNumber='num',          eFloat = '#.##',
		eSpace=' ',

		eBitAndAssign='&=',      eBitNotAssign='~=',  eBitOrAssign='|=',      eBitXorAssign='^=',     eBoolAnd='&&',
		eBoolAndAssign='&&=',    eBoolOr='||',        eBoolOrAssign='||=',    eDecrement='--',        eDivideAssign='/=',
		eEqualsTo='==',          eElipsis='...',      eGreaterEquals='>=',    eIncrement='++',        eLessEquals='<=',
		eMinusAssign='-=',       eModulusAssign='%=', eMultiplyAssign='*=',   eNotEquals='!=',        eDereference='->',
		ePlusAssign='+=',        eScope='::',         eShiftLeft='<<',        eShiftLeftAssign='<<=', eShiftRight='>>',
		eShiftRightAssign='>>=', eSumAssign='+=',

		eText='text',            eStartTag='<t>',     eEndTag='</t>',         eAtomTag='<t/>',
		eJsonDict='{..}',        eJsonArray='[..]'
	};
#pragma GCC diagnostic pop

	struct Token: public Klass {
			TokenType type = eEmpty;
			String text;
			String whitespace;

/**/		Token(void): type(eEmpty) {}
/**/		Token(TokenType type)										: type(type) { text = wideCharToString(type).strip(); }
/**/		Token(TokenType type, String text, String whitespace = "")	: Token(type) { this->text = text; this->whitespace = whitespace; }
/**/		Token(wchar type)											: type((TokenType)type), text(wideCharToString(type).strip()) {}
/**/		Token(wchar type, String text, String whitespace = "")		: Token(type) { this->text = text; this->whitespace = whitespace; }
/**/		Token(const Token& token)									: type(token.type), text(token.text), whitespace(token.whitespace) {}
/**/		bool isEOF(void) { return (type == eEOF); }

/**/		Token& operator=(const Token& token)		{ type = token.type; text = token.text; whitespace = token.whitespace; return *this; }

/**/		bool operator==(const Token& token) const	{ return (type == token.type  &&  text == token.text  &&  whitespace == token.whitespace); }
/**/		bool operator==(TokenType type) const 		{ return (this->type == type); }
/**/		bool operator==(const String& text) const	{ return (this->text == text); }
/**/		bool operator==(int value) const			{ return (value == type); }
/**/friend 	bool operator==(int value, Token token) 	{ return (value == token.type); }

/**/		bool operator!=(const Token& token) const	{ return !(type == token.type  &&  text == token.text  &&  whitespace == token.whitespace); }
/**/		bool operator!=(TokenType type) const		{ return (this->type != type); }
/**/		bool operator!=(const String& text) const	{ return (this->text != text); }

/**/		bool operator<(const Token& token) const	{ return (type < token.type  &&  text < token.text); }


/**/static 	String wideCharToString(wchar wide_char);
/**/		virtual const char* getChars(void) const	{ return text.getChars(); }
/**/		virtual String toString(void) const { return String::formatString("['%s': \"%s\", \"%s\"]", wideCharToString(type).getChars(), text.getChars(), whitespace.getChars()); }
/**/		static bool isWordStart(int c);
/**/		static bool isWord(int c) { return ( isWordStart(c) || isdigit(c) ); }

/**/friend	std::ostream& operator<<(std::ostream& ostream, const Token& token) { return ostream << token.toString(); }
	};

#endif /* TOKEN_HPP_ */

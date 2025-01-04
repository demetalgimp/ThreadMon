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

/*tested*/	Token(void): type(eEmpty) {}
/*tested*/	Token(TokenType type)										: type(type) { text = wideCharToString(type).strip(); }
/*tested*/	Token(TokenType type, String text, String whitespace = "")	: Token(type) { this->text = text; this->whitespace = whitespace; }
/*tested*/	Token(wchar type)											: type((TokenType)type), text(wideCharToString(type).strip()) {}
/*tested*/	Token(wchar type, String text, String whitespace = "")		: Token(type) { this->text = text; this->whitespace = whitespace; }
/*tested*/	Token(const Token& token)									: type(token.type), text(token.text), whitespace(token.whitespace) {}
/*tested*/	bool isEOF(void) { return (type == eEOF); }

/*tested*/	Token& operator=(const Token& token)		{ type = token.type; text = token.text; whitespace = token.whitespace; return *this; }

/*tested*/	bool operator==(const Token& token) const	{ return (type == token.type  &&  text == token.text  &&  whitespace == token.whitespace); }
/*tested*/	bool operator==(TokenType type) const 		{ return (this->type == type); }
/*tested*/	bool operator==(const String& text) const	{ return (this->text == text); }
/*tested*/	bool operator==(int value) const			{ return (value == type); }
/*tested*/	friend bool operator==(int value, Token token) 	{ return (value == token.type); }

/*tested*/	bool operator!=(const Token& token) const	{ return !(type == token.type  &&  text == token.text  &&  whitespace == token.whitespace); }
/*tested*/	bool operator!=(TokenType type) const		{ return (this->type != type); }
/*tested*/	bool operator!=(const String& text) const	{ return (this->text != text); }

/*tested*/	bool operator<(const Token& token) const	{ return (type < token.type  &&  text < token.text); }


/*tested*/	static String wideCharToString(wchar wide_char);
/*tested*/	virtual const char* getChars(void) const	{ return text.getChars(); }
/*??*/		virtual Klass *clone(void) const		{ return new Token(*this); }
/*tested*/	virtual String toString(void) const { return String::formatString("['%s': \"%s\", \"%s\"]", wideCharToString(type).getChars(), text.getChars(), whitespace.getChars()); }
/*tested*/	static bool isWordStart(int c);
/*tested*/	static bool isWord(int c) { return ( isWordStart(c) || isdigit(c) ); }

/*tested*/	friend std::ostream& operator<<(std::ostream& ostream, const Token& token) { return ostream << token.toString(); }
	};

#endif /* TOKEN_HPP_ */

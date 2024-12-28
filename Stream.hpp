//editor: tab=4
#ifndef __STREAM__
#define __STREAM__

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
/**/		Token(TokenType type): type(type) { text = wideCharToString(type).strip(); }
/**/		Token(TokenType type, String text, String whitespace = ""): Token(type) { this->text = text; this->whitespace = whitespace; }
/**/		Token(wchar type): type((TokenType)type), text(wideCharToString(type).strip()) {}
/**/		Token(wchar type, String text, String whitespace = ""): Token(type) { this->text = text; this->whitespace = whitespace; }
/**/		Token(const Token& token): type(token.type), text(token.text), whitespace(token.whitespace) {}
/**/		bool isEOF(void) { return (type == eEOF); }

/**/		Token& operator=(const Token& token) { type = token.type; text = token.text; whitespace = token.whitespace; return *this; }

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

	class Stream: public Klass {
		public:
			uint linenum = 1;

		public:
			virtual ~Stream(void) {}

		public:
			uint getLineNumber(void) { return linenum; }

		public:
			virtual int  back(void) = 0;
			virtual int  current(void) = 0;
			virtual int  next(void) = 0;
			virtual int  peek(uint ahead = 0) const = 0;
			virtual bool peek(const String& seed, bool consume = false) = 0;
			virtual bool isEOF(void) const = 0;
			virtual void skip(uint chars = 1) = 0;
			virtual uint getLength(void) const = 0;
			virtual uint getPosition(void) const = 0;
	};

	class StringStream: public Stream {
		String string;
		bool first_time = true;
		uint position = 0;

		public:
/**/		StringStream(const String& string): string(string) {}

		public:
/**/		virtual int  back(void);
/**/		virtual int  current(void);
/**/		virtual int  next(void);
/**/		virtual int  peek(uint ahead = 0) const;
/**/		virtual bool peek(const String& seed, bool consume = false); // <-- method not 'const' b/c revises object if "skip_seed" true
/**/		virtual bool isEOF(void) const				{ return (position >= string.getLength()); }
/**/		virtual void skip(uint chars = 1);
/**/		virtual uint getLength(void) const			{ return string.getLength(); }
/**/		virtual uint getPosition(void) const		{ return position; }

		public:
/**/		virtual int operator*(void)					{ return current(); }
/**/		virtual int operator++(void)				{ return next(); }
/**/		virtual int operator--(void)				{ return back(); }

		public:
/**/		virtual cchar* getChars(void) const			{ return string.getChars() + position; }
/**/		virtual String toString(void) const			{ return String::formatString("[position=%u stream=\"%s\"]", position, string.getChars()); }
/**/	friend std::ostream& operator<<(std::ostream& stream, const StringStream& string) { return (stream << string.toString()); }
	};

#endif

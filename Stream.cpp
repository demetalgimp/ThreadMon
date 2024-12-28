//editor: tab=4
#include "Stream.hpp"

String Token::wideCharToString(wchar wide_char) {
	static constexpr wchar charnames[128] = {
			'NULL', 'SOH', 'STX', 'ETX', 'EOT', 'ENQ', 'ACK', 'BELL',
			'BS',   'HT',  'LF',  'VT',  'FF',  'CR',  'SO',  'SI',
			'DLE',  'DC1', 'DC2', 'DC3', 'DC4', 'NAK', 'SYN', 'ETB',
			'CAN',  'EM',  'SUB', 'ESC', 'FS',  'GS',  'RS',  'US',
			'SP',   '!',   '"',   '#',   '$',   '%',   '&',   '\'',
			'(',    ')',   '*',   '+',   ',',   '-',   '.',   '/',
			'0',    '1',   '2',   '3',   '4',   '5',   '6',   '7',
			'8',    '9',   ':',   ';',   '<',   '=',   '>',   '?',
			'@',    'A',   'B',   'C',   'D',   'E',   'F',   'G',
			'H',    'I',   'J',   'K',   'L',   'M',   'N',   'O',
			'P',    'Q',   'R',   'S',   'T',   'U',   'V',   'W',
			'X',    'Y',   'Z',   '[',   '\\',  ']',   '^',   '_',
			'`',    'a',   'b',   'c',   'd',   'e',   'f',   'g',
			'h',    'i',   'j',   'k',   'l',   'm',   'n',   'o',
			'p',    'q',   'r',   's',   't',   'u',   'v',   'w',
			'x',    'y',   'z',   '{',   '|',   '}',   '~',   'BEL'
	};

	if ( (int)wide_char != EOF ) {
		if ( 0 <= wide_char  &&  wide_char <= 0x7F ) {
			wide_char = charnames[wide_char];
		}

		char wchar_str[] =
			{
				(char)((wide_char >> 24) & 0xFF?: ' '),
				(char)((wide_char >> 16) & 0xFF?: ' '),
				(char)((wide_char >> 8) & 0xFF?: ' '),
				(char) (wide_char & 0xFF), 0
			};
		return wchar_str;

	} else {
		return " EOF";
	}
}


//--- StringStream --------------------------------------------------------------------------------
	int StringStream::current(void) {
		first_time = false;
		return string[position];
	}

	int StringStream::next(void) {
		if ( position >= string.getLength() ) {
			return EOF;
		}

	//--- special case (index == 0) where this is the first time being called: we don't to miss the first symbol.
		int c = (first_time? string[position]: string[++position]);
		if ( c == '\n' ) {
			linenum++;
		}
		first_time = false;
		return c;
	}

	int StringStream::back(void) {
        if ( position > 0 ) {
        	if ( string[position] == '\n' ) {
				linenum--;
			}
            return string[--position];
        }
        return string[position];
	}

	int StringStream::peek(uint ahead) const {
		return (position < string.getLength()? string[position + ahead]: EOF);
	}

	bool StringStream::peek(const String& seed, bool skip_seed) {
		bool found = false;
		if ( seed.getLength() > 0  &&  position < string.getLength() ) {
			const char *tmps = string.getChars() + position;//(index > 0? index - 1: index);
			found = string.strsub_fn(tmps, seed.getChars());
			if ( found && skip_seed ) {
				position += seed.getLength();// + 1;
			}
		}
		return found;
	}

	void StringStream::skip(uint chars)	{
		while ( position < string.getLength()  &&  chars > 0 ) {
			if ( string[++position] == '\n' ) {
				linenum++;
			}
			chars--;
		}
	}

	bool Token::isWordStart(int c) {
		if ( c != -1 ) {
			bool lower_case = ('a' <= c  &&  c <= 'z');
			bool upper_case = ('A' <= c  &&  c <= 'Z');
			return (lower_case || upper_case || c == '_');

		} else {
			return false;
		}
	}

//	bool Token::isDigit(int c) {
//		if ( c != -1 ) {
//			return ('0' <= c  &&  c <= '9');
//
//		} else {
//			return false;
//		}
//	}



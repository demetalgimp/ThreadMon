//editor: tab=4
/*
 * StringStream.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#include "StringStream.hpp"

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

	String StringStream::serialize(void) const	{
		return String("{") +
				"\"StringStream\": "
					"{"
						"\"position\": \"" + position + "\", "
						"\"bookmark\": \"" + bookmark + "\", "
						"\"string\": \"" + string + "\""
					"}"
				"}";
	}

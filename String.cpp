//editor: tab=4
#include <string>
#include "String.hpp"

	void TextAccumulator::resizeBy(uint increment) {
		if ( position + increment >= current_size ) {
			current_size += increment + BUFFER_SIZE_INC;
			buffer = reinterpret_cast<char*>(realloc(buffer, current_size + 1));
		}
	}

	TextAccumulator& TextAccumulator::operator+=(char c) {
		resizeBy(1);
		buffer[position++] = c;
		buffer[position] = 0;
		return *this;
	}
	TextAccumulator& TextAccumulator::operator+=(const char *str) {
		if ( str != nullptr  &&  *str != 0 ) {
			uint len = strlen(str);
			resizeBy(len);

			while ( *str != 0 ) {
				buffer[position++] = *str++;
			}
			buffer[position] = 0;
		}
		return *this;
	}
	char TextAccumulator::operator[](uint index) {
		if ( index < position ) {
			return buffer[index];
		}
		THROW_ERROR("Bad TextAccumulator index: size=%u index=%u.", position, index);
		return -1;
	}
	String TextAccumulator::toString(void) {
		return buffer;
	}

//===================================================================================================================================================
	String::String(const char *str, size_t count): length(0), size(BUFFER_SIZE_INC) {
		if ( str == nullptr  ||  *str == 0 ) {
			buffer = new char[size + 1];
			*buffer = 0;

		} else {
			size = length = (count?: strlen(str));
			buffer = new char[size + 1];
			strncpy(buffer, str, size);
			buffer[size] = 0;
		}
	}

	String::String(char c) {
		buffer = new char[2];
		buffer[0] = c;
		buffer[1] = 0;
		size = length = 1;
	}

	String::String(const String& string): length(string.length), size(string.size), compare_fn(string.compare_fn), strsub_fn(string.strsub_fn), strstr_fn(string.strstr_fn) {
		buffer = new char[size + 10];
		strncpy(buffer, string.buffer, length + 1);
		buffer[length] = 0;
	}

	String::~String(void) {
		delete [] buffer;
	}

	String String::operator+(const char *str) const {
		if ( str != nullptr  &&  *str != 0 ) {
			int len = strlen(str);
			char tmps[len + length + 1];
			snprintf(tmps, sizeof(tmps), "%s%s", buffer, str);
			return tmps;

		} else {
			return *this;
		}
	}

	String String::operator+(const String& string) const {
	 	char tmps[length + string.length + 1];
	 	snprintf(tmps, sizeof(tmps), "%s%s", buffer, string.buffer);
	 	return tmps;
	}

	char String::operator[](uint index) const {
		if ( index < length ) {
			return buffer[index];

		} else {
			return -1;
		}
	}

	String& String::operator=(const char *str) {
		if ( str != nullptr  &&  *str != 0 ) {
			int len = strlen(str);
			resize(len);
			length = len;
			strncpy((char*)buffer, str, length + 1);

		} else {
			length = 0;
			*buffer = 0;
		}
		return *this;
	}

	String& String::operator=(const std::string& std_string) {
		return operator=(std_string.c_str());
	}

	String& String::operator=(const String& string) {
		resize(string.length);
		length = string.length;
		strncpy((char*)buffer, (char*)string.buffer, length + 1);
		return *this;
	}

	String& String::clear(void) {
		length = 0;
		if ( size < BUFFER_SIZE_INC ) {  // <-- It is easy to assume that this object will be involved in string manipulation.
			delete [] buffer;
			size = BUFFER_SIZE_INC;
			buffer = new char[size];
		}
		return *this;
	}

	String& String::resize(uint new_size) {
		if ( new_size > size ) {
			size = new_size + BUFFER_SIZE_INC;
			buffer = reinterpret_cast<char*>(realloc(buffer, size + 1));
//			char *tmps = new char[size + 1];
//			strncpy(tmps, buffer, length + 1);
//			delete [] buffer;
//			buffer = tmps;
		}
		return *this;
	}

	String& String::operator+=(const char *str) {
		if ( str != nullptr  &&  *str != 0 ) {
			int len = strlen(str);
			resize(length + len);
			length += len;
			strncat(buffer, str, length);
		}
		return *this;
	}

	String& String::operator+=(const String& string) {
		if ( !string.isEmpty() ) {
			resize(length + string.length);
			length += string.length;
			strncat(buffer, string.buffer, length);
		}
		return *this;
	}

	std::ostream& operator<<(std::ostream& stream, const String& string) {
		stream << string.getChars();
		return stream;
	}

	String operator+(const char* str, const String& string) {
		if ( str != nullptr  &&  *str != 0 ) {
			uint length = strlen(str) + string.length;
			uint size = length - length % String::BUFFER_SIZE_INC + String::BUFFER_SIZE_INC;
			if ( size - length == 0 ) {
				size += String::BUFFER_SIZE_INC;
			}
			char *buffer = new char[size + 1];
			snprintf(buffer, size + 1, "%s%s", str, string.buffer);
			return String(buffer, length, size);

		} else {
			return string;
		}
	}

//---------------------------------------------------------------------------------------------------------------------------------------------------
	bool String::startsWith(const char* str) const {
		return strsub_fn((char*)buffer, str);
	}

	bool String::startsWith(const String& string) const {
		return strsub_fn((char*)buffer, (char*)string.buffer);
	}

	String String::strip(void) const {
		char *head = buffer;
		while ( isspace(*head) ) {
			head++;
		}
		char *tail = buffer + length - 1;
		while ( tail > head  &&  isspace(*tail) ) {
			tail--;
		}
		return ( tail >= head? String(head, tail - head + 1): "");
	}

	static int testNull(const char *str, const char *sub) {
		if ( str != nullptr  &&  (sub == nullptr  ||  *sub == 0) ) {
			return 1;

		} else if ( str != nullptr  &&  sub != nullptr ) {
			return -1;

		} else {
			return 0;
		}
	}

	String& String::enableIgnoreCase(bool ignore)  {
		compare_fn = (ignore? strcasecmp: strcmp);
		strsub_fn = (ignore? strcasesub: strsub);
		strstr_fn = (ignore? (strstr_fn_t*)strcasestr: (strstr_fn_t*)strstr);
		return *this;
	}

	bool String::strsub(const char *str, const char *sub) {
		int test = testNull(str, sub);
		if ( test == -1 ) {
			while ( *str != 0  &&  *sub != 0  &&  *str == *sub ) {
				str++, sub++;
			}
			return (*sub == 0);

		} else {
			return test;
		}
	}

	bool String::strcasesub(const char *str, const char *sub) {
		int test = testNull(str, sub);
		if ( test == -1 ) {
			while ( *sub != 0  &&  toupper(*str) == toupper(*sub) ) {
				str++, sub++;
			}
			return (*sub == 0);

		} else {
			return test;
		}
	}

	String String::toString(long long value, uint base) {
		static const char charmap[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+;<=>?@^_`{|}~";
		if ( base < sizeof(charmap) ) {
			bool is_negative = (value < 0);
			value = (value < 0? -value: value);

		//--- The size of the string will be no more than the number of bits (base 2)
			char tmps[sizeof(value) * 8 + 1];
			memset(tmps, 0, sizeof(tmps));

		//--- Use modulus, walking backwards as digits are extracted
			char *s = &tmps[sizeof(tmps) - 1];
			*--s = 0;
			do {
				int n = value % base;
				*s-- = charmap[n];
				value /= base;
			} while ( value != 0 );

		//--- Restore signedness
			if ( is_negative ) {
				*s-- = '-';
			}
			return s+1;

		} else {
			throw String::formatString("toString(int) does not allow base >%d.", sizeof(charmap));
		}
	}

	std::vector<String> String::split(const char* needle) const {
		uint needle_len = strlen(needle);
		std::vector<String> fragments;
		const char *marker = buffer;
		const char *found = nullptr;
		while ( (found = strstr_fn(marker, needle)) != nullptr ) {
			fragments.push_back( (found - marker > 0? String(marker, found - marker): "") );
			marker = found + needle_len;
		}
		fragments.push_back(marker);
		return fragments;
	}

	std::vector<String> String::split(const String& needle) const {
		return split(needle.buffer);
	}

	String String::formatString(const String& fmt, ...) {
		va_list ap;

	//--- Calculate number of bytes
		va_start(ap, fmt);
		uint len = vsnprintf(nullptr, 0, (char*)fmt.getChars(), ap);
		va_end(ap);

	//--- Create buffer
		char buffer[len + 1];

	//--- Fill
		va_start(ap, fmt);
		vsnprintf(buffer, sizeof(buffer), (char*)fmt.getChars(), ap);
		va_end(ap);

		return buffer;
	}


#define NUM_TO_HEX(num) (num + (num <= 9? '0': 'A' - 10))
#define DIGIT_TO_HEX(digit, pos) ((digit >> (pos * 4)) & 0x0F)
// 0000: 00 00 00 00  00 00 00 00 | 00 00 00 00  00 00 00 00   0123456789ABCDEF
//                 1               2               3               4          v
// 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
	String String::hexDump(void) const {
		String result;
		const uint BYTES_WIDE = 16;
		char tmps[0x50 + 1];

		uint line_index = 0;
		for ( uint buffer_index = 0; buffer_index < length; buffer_index++ ) {

			if ( buffer_index % 16 == 0 ) {
				line_index = 0;
				memset(tmps, ' ', sizeof(tmps));
				tmps[sizeof(tmps) - 1] = 0;
				tmps[sizeof(tmps) - 2] = '\n';
			}

		//--- index
			if ( buffer_index % BYTES_WIDE == 0 ) {
				tmps[line_index++] = NUM_TO_HEX(DIGIT_TO_HEX(buffer_index, 3)); // *or* ((i >> 12) & 0x0F) + '0';
				tmps[line_index++] = NUM_TO_HEX(DIGIT_TO_HEX(buffer_index, 2)); // *or* ((i >>  8) & 0x0F) + '0';
				tmps[line_index++] = NUM_TO_HEX(DIGIT_TO_HEX(buffer_index, 1)); // *or* ((i >>  4) & 0x0F) + '0';
				tmps[line_index++] = NUM_TO_HEX(DIGIT_TO_HEX(buffer_index, 0)); // *or*  (i        & 0x0F) + '0';
				tmps[line_index++] = ':';
				tmps[line_index++] = ' ';
			}

		//--- byte block
			tmps[0x3C + buffer_index % 16] = (' ' <= buffer[buffer_index]? buffer[buffer_index]: '.'); // <-- 0x80-0xFF still appears as a '.'

		//--- hex block
			tmps[line_index++] = NUM_TO_HEX(DIGIT_TO_HEX(buffer[buffer_index], 1)); // *or* ((buffer[i] >> 4) & 0x0F) + '0';
			tmps[line_index++] = NUM_TO_HEX(DIGIT_TO_HEX(buffer[buffer_index], 0)); // *or*  (buffer[i]       & 0x0F) + '0';
			tmps[line_index++] = ' ';

		//--- spacing
			if ( buffer_index % 16 == 7 ) {
				tmps[line_index++] = '|';
				tmps[line_index++] = ' ';

			} else if ( buffer_index % 16 == 3  ||  buffer_index % 16 == 11 ) {
				tmps[line_index++] = ' ';

		//--- append line
			} else if ( buffer_index % 16 == 15 ) {
				result += tmps;
			}
		}

		if ( length % 16 > 0 ) {
			result += tmps;
		}
		return result;
	}

	String String::wideCharToString(uint wchar) {
		char str[] = { (char)((wchar >> 24) & 0xFF?: ' '), (char)((wchar >> 16) & 0xFF?: ' '), (char)((wchar >> 8) & 0xFF?: ' '), (char)(wchar & 0xFF), 0 };
		return str;
	}


//---------------------------------------------------------------------------------------------------------------------------------------------------
	FILE *fopen(const String& filename, const String& permissions) {
		return fopen((char*)filename.getChars(), (char*)permissions.getChars());
	}
//{
//	FILE *fp = fopen(filename.getChars(), permissions.getChars());
//	if ( fp == nullptr ) {
////	extern int errno;
////	int err = errno;
//		throw String::formatString("[%s,%d]Error: fopen(\"%s\", \"%s\") failed with %s", __FILE__, __LINE__, filename.getChars(), permissions.getChars(), strerror(errno));
//	}
//}

	bool fgets(String& buffer, FILE *fp) {
		char line[1024], *result;
		do {
			if ( (result = fgets(line, sizeof(line), fp)) != nullptr) {
				buffer += line;
			}
		} while ( result != nullptr  &&  !strchr(line, '\n') );
		return (buffer.isEmpty());
	}

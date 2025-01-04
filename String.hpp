//editor: tab=4
/*
 * String.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef __STRING__
#define __STRING__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

typedef uint8_t uchar;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint32_t wchar;
typedef __int128_t int128_t;

typedef const char cchar;
class String;

#define THROW_ERROR(fmt, ...) throw String::formatString("Error[%s:%d]:- " fmt "\n", __FILE__, __LINE__, __VA_ARGS__)

	class Klass {
		public:
			Klass(void) {}
			virtual ~Klass(void) {}

		public:
			virtual cchar* getChars(void) const = 0;
			virtual String toString(void) const = 0;
			virtual Klass *clone(void) const = 0;
/*??*/		friend std::ostream& operator<<(std::ostream& stream, const Klass& object);
	};

	class TextAccumulator: public Klass {
		friend class UnitTests;

		private:
			static const uint BUFFER_SIZE_INC = 1024;

		private:
			uint current_size = BUFFER_SIZE_INC;
			char *buffer = new char[current_size + 1];
			uint position = 0;

		public:
/*tested*/	TextAccumulator(void)					{ buffer[position] = 0; }
/*tested*/	virtual ~TextAccumulator(void)			{ delete [] buffer; }

		public:
/*tested*/	TextAccumulator& operator+=(char c);
/*tested*/	TextAccumulator& operator+=(const char *str);
/*tested*/	char operator[](uint index);

		private:
/*??*/		void resizeBy(uint increment);

		public:
/*tested*/	void clear(void) { position = 0; buffer[position] = 0; }
/*tested*/	uint getLength(void) { return position; }
/*tested*/	uint getSize(void) { return current_size; }

		public:
/*??*/		cchar *getChars(void) const { return "TextAccumulator"; }
/*tested*/	String toString(void) const;
/*??*/		TextAccumulator *clone(void) const {
				TextAccumulator *acc = new TextAccumulator();
				acc->current_size = current_size;
				acc->buffer = strdup(buffer);
				acc->position = position;
				return acc;
			}
	};

	class String: public Klass {
		friend class UnitTests;

		private:
			static const size_t BUFFER_SIZE_INC = 250;
			typedef cchar* (strstr_fn_t)(cchar*, cchar*);
			typedef int (compare_fn_t)(cchar*, cchar*);
			typedef bool (strsub_fn_t)(cchar*, cchar*);

		private:
			char *buffer = nullptr;
			size_t length = 0;
			size_t size = 0;

		public:
/*tested*/	compare_fn_t *compare_fn = strcmp;
/*tested*/	strsub_fn_t *strsub_fn = strsub;
/*tested*/	strstr_fn_t *strstr_fn = strstr;

		public:
/*tested*/	String(cchar *str = nullptr, size_t count = 0);
/*tested*/	String(const String& string);
/*tested*/	String(const std::string& std_string): String(std_string.c_str()) {}
/*tested*/	explicit String(char value);
/*tested*/	virtual ~String(void);

		private:
/*??*/		String(char *str, size_t length, size_t size): buffer(str), length(length), size(size) {}

		public: //--- Operator overloads
/*tested*/	String operator+(cchar *str) const;
/*tested*/	String operator+(const String& string) const;
/*tested*/	friend String operator+(cchar* str, const String& string);
/*tested*/	char operator[](uint index) const;
/*tested*/	bool operator==(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) == 0); }
/*tested*/	bool operator==(const std::string& std_string) const					{ return (compare_fn((cchar*)buffer, (std_string.c_str()?: "")) == 0); }
/*tested*/	friend bool operator==(const std::string& std_string, const String& string)	{ return (string.compare_fn((std_string.c_str()?: ""), (cchar*)string.buffer) == 0); }
/*tested*/	bool operator==(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) == 0); }
/*tested*/	friend bool operator==(const char *str, const String& string)					{ return (string.compare_fn(str, (cchar*)string.buffer) == 0); }
/*tested*/	bool operator!=(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) != 0); }
/*tested*/	bool operator!=(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) != 0); }
/*tested*/	bool operator>=(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) >= 0); }
/*tested*/	bool operator>=(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) >= 0); }
/*tested*/	bool operator<=(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) <= 0); }
/*tested*/	bool operator<=(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) <= 0); }
/*tested*/	bool operator>(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) > 0); }
/*tested*/	bool operator>(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) > 0); }
/*tested*/	bool operator<(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) < 0); }
/*tested*/	bool operator<(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) < 0); }
/*tested*/	String& operator=(cchar *str);
/*tested*/	String& operator=(const std::string& std_string);
/*tested*/	String& operator=(const String& string);
/*tested*/	String& operator+=(cchar *str);
/*tested*/	String& operator+=(const String& string);
/*tested*/	friend std::ostream& operator<<(std::ostream& stream, const String& string);

		public: //--- manipulation methods
/*tested*/	bool startsWith(cchar* sub) const;
/*tested*/	bool startsWith(const String& sub) const;
/*tested*/	bool contains(cchar* sub) const							{ return (strstr_fn(buffer, (sub?: "")) != nullptr); }
/*tested*/	bool contains(const String& sub) const					{ return (strstr_fn(buffer, sub.buffer) != nullptr); }
/*tested*/	String strip(void) const;
/*tested*/	String& clear(void);
/*tested*/	std::vector<String> split(cchar* needle) const;
/*tested*/	std::vector<String> split(const String& needle) const;

		protected:
/*??*/		String& resize(uint new_size);

		public: //--- getters/setters
/*tested*/	bool isEmpty(void) const								{ return (length == 0); }
/*tested*/	size_t getLength(void) const							{ return length; }
/*tested*/	size_t getSize(void) const								{ return size; }
/*??*/		String *clone(void) const 							{ return new String(*this); }

/*tested*/	cchar* getChars(void) const								{ return buffer; }
/*tested*/	String toString(void) const								{ return *this; }
/*tested*/	String& enableIgnoreCase(bool ignore);

		public:
/*tested*/	static bool strsub(cchar *str, cchar *sub);
/*tested*/	static bool strcasesub(cchar *str, cchar *sub);
/*tested*/	static String toString(long long number, uint base = 10);
/*tested*/	static String formatString(const String& fmt, ...);
/*tested*/	String hexDump(void) const;
/*tested*/	static String wideCharToString(uint wchar);
	};

FILE *fopen(const String& filename, const String& permissions);
bool fgets(String& buffer, FILE *fp);
int vprintf(const String& fmt, ...);
size_t read(int fd, String& buffer, size_t count = 0);
ssize_t write(int fd, const String& buffer);

//uchar *strcpy(uchar *dst, const uchar *src);                // { return (uchar*)strcpy((char*)dst, (cchar*)src); }
//uchar *strncpy(uchar *dst, const uchar *src, size_t count); // { return (uchar*)strncpy((char*)dst, (cchar*)src, count); }
//uchar *strdup(const uchar *src);                            // { return (uchar*)strdup((cchar*)srd); }
//uchar *strstr(const uchar *haystack, const uchar *needle);  //|
//uchar *strnstr(const uchar *haystack, const uchar *needle); //|

#endif

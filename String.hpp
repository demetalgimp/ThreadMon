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
//#define THROW_ERROR(fmt, ...) std::cerr << String::formatString("Error[%s:%d]:- " fmt, __FILE__, __LINE__, __VA_ARGS__) << std::endl

	class Klass {
		public:
			Klass(void) {}
			virtual ~Klass(void) {}

		public:
			virtual cchar* getChars(void) const = 0;
			virtual String toString(void) const = 0;
			friend std::ostream& operator<<(std::ostream& stream, const Klass& object);
	};

	class TextAccumulator {
		friend class UnitTests;

		private:
			static const uint BUFFER_SIZE_INC = 1024;

		private:
			uint current_size = BUFFER_SIZE_INC;
			char *buffer = new char[current_size + 1];
			uint position = 0;

		public:
/**/		TextAccumulator(void)					{ buffer[position] = 0; }
/**/		virtual ~TextAccumulator(void)			{ delete [] buffer; }

		public:
/**/		TextAccumulator& operator+=(char c);
/**/		TextAccumulator& operator+=(const char *str);
/**/		char operator[](uint index);

		private:
			void resizeBy(uint increment);

		public:
/**/		void clear(void) { position = 0; buffer[position] = 0; }
/**/		uint getLength(void) { return position; }
/**/		uint getSize(void) { return current_size; }

		public:
/**/		String toString(void);
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
/**/		compare_fn_t *compare_fn = strcmp;
/**/		strsub_fn_t *strsub_fn = strsub;
/**/		strstr_fn_t *strstr_fn = strstr;

		public:
/**/		String(cchar *str = nullptr, size_t count = 0);
/**/		String(const String& string);
/**/		String(const std::string& std_string): String(std_string.c_str()) {}
/**/		explicit String(char value);
/**/		virtual ~String(void);

		private:
			String(char *str, size_t length, size_t size): buffer(str), length(length), size(size) {}

		public: //--- Operator overloads
/**/		String operator+(cchar *str) const;
/**/		String operator+(const String& string) const;
/**/		friend String operator+(cchar* str, const String& string);
/**/		char operator[](uint index) const;
/**/		bool operator==(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) == 0); }
/**/		bool operator==(const std::string& std_string) const					{ return (compare_fn((cchar*)buffer, (std_string.c_str()?: "")) == 0); }
/**/friend  bool operator==(const std::string& std_string, const String& string)	{ return (string.compare_fn((std_string.c_str()?: ""), (cchar*)string.buffer) == 0); }
/**/		bool operator==(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) == 0); }
/**/friend  bool operator==(const char *str, const String& string)					{ return (string.compare_fn(str, (cchar*)string.buffer) == 0); }
/**/		bool operator!=(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) != 0); }
/**/		bool operator!=(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) != 0); }
/**/		bool operator>=(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) >= 0); }
/**/		bool operator>=(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) >= 0); }
/**/		bool operator<=(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) <= 0); }
/**/		bool operator<=(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) <= 0); }
/**/		bool operator>(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) > 0); }
/**/		bool operator>(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) > 0); }
/**/		bool operator<(cchar *str) const										{ return (compare_fn((cchar*)buffer, (str?: "")) < 0); }
/**/		bool operator<(const String& string) const								{ return (compare_fn((cchar*)buffer, (cchar*)string.buffer) < 0); }
/**/		String& operator=(cchar *str);
/**/		String& operator=(const std::string& std_string);
/**/		String& operator=(const String& string);
/**/		String& operator+=(cchar *str);
/**/		String& operator+=(const String& string);
/**/friend  std::ostream& operator<<(std::ostream& stream, const String& string);

		public: //--- manipulation methods
/**/		bool startsWith(cchar* sub) const;
/**/		bool startsWith(const String& sub) const;
/**/		bool contains(cchar* sub) const							{ return (strstr_fn(buffer, (sub?: "")) != nullptr); }
/**/		bool contains(const String& sub) const					{ return (strstr_fn(buffer, sub.buffer) != nullptr); }
/**/		String strip(void) const;
/**/		String& clear(void);
/**/		std::vector<String> split(cchar* needle) const;
/**/		std::vector<String> split(const String& needle) const;

		protected:
			String& resize(uint new_size);

		public: //--- getters/setters
/**/		bool isEmpty(void) const								{ return (length == 0); }
/**/		size_t getLength(void) const							{ return length; }
/**/		size_t getSize(void) const								{ return size; }
/**/		cchar* getChars(void) const								{ return buffer; }
/**/		String toString(void) const								{ return *this; }
/**/		String& enableIgnoreCase(bool ignore);

		public:
/**/		static bool strsub(cchar *str, cchar *sub);
/**/		static bool strcasesub(cchar *str, cchar *sub);
/**/		static String toString(long long number, uint base = 10);
/**/		static String formatString(const String& fmt, ...);
/**/		String hexDump(void) const;
/**/		static String wideCharToString(uint wchar);
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

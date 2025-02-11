//editor: tab=4
/*
 * UnitTests.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>

#include "String.hpp"
#include "Stream.hpp"
#include "Xml.hpp"
#include "TokenStream.hpp"
#include "CppTokenStream.hpp"
#include "XmlTokenStream.hpp"
#include "AsciiVT200.hpp"
#include "IAS_TestSuite.hpp"
#include "UnitTests.hpp"

	void IAS_unittest_assert(const char *src_filename, const char *method, uint lineno, const char *str_to_test, const auto to_test) {
		test_number++;

		if ( to_test ) {
			tests_that_passed++;
			std::cout << std::flush
				<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
				<< str_to_test << "... PASSED."
				<< std::endl
				<< std::flush;

		} else {
			tests_that_failed++;
			std::cerr << std::flush
				<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
				<< "expression: "
				<< AsciiVT200::redForeground
				<< (strchr(str_to_test, '\n')? ":\n": " ") << "... "
				<< str_to_test << "... FAILED."
				<< AsciiVT200::resetTerminal
				<< std::endl
				<< std::flush;
		}
	}

	String autoToString(auto var) {
		std::ostringstream output;
		output << var;
		return output.str();
	}

	void IAS_unittest_equals(const char *src_filename, const char *method, uint lineno, const char *str_output_to_test, const char *str_expected, const auto output_to_test, const auto expected) {
		test_number++;

		String out__str = autoToString(output_to_test);
		String exp__str = autoToString(output_to_test);

		if ( output_to_test == expected ) {
			tests_that_passed++;
			std::cout << std::flush
				<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
				<< "expression: " << (strchr(out__str.getChars(), '\n')? '\n': ' ') << output_to_test << "... PASSED.";
			std::cout
				<< std::endl
				<< std::flush;

		} else {
			tests_that_failed++;
			std::cerr << std::flush
				<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
				<< "expression: " << (strchr(str_output_to_test, '\n')? ":\n": " ") << str_output_to_test << "... "
				<< AsciiVT200::redForeground << "FAILED: " << AsciiVT200::resetTerminal
				<< "expected: ["
				<< AsciiVT200::redForeground << (strchr(exp__str.escape_ize().getChars(), '\n')? "\n": "") << expected << AsciiVT200::resetTerminal
				<< "] but got: ["
				<< AsciiVT200::redForeground << (strchr(out__str.escape_ize().getChars(), '\n')? "\n": "") << output_to_test << AsciiVT200::resetTerminal
				<< "]";
			std::cout
				<< std::endl
				<< std::flush;
		}
	}

#define PASS() { \
		const char *usecase_file = __FILE__; \
		const char *usecase_method = __FUNCTION__; \
		uint usecase_lineno = __LINE__; \
		IAS_pass(usecase_file, usecase_method, usecase_lineno); \
	}

#define FAIL(MSG) { \
		const char *usecase_file = __FILE__; \
		const char *usecase_method = __FUNCTION__; \
		uint usecase_lineno = __LINE__; \
		IAS_fail(usecase_file, usecase_method, usecase_lineno, MSG); \
	}

//--- ERROR! Because I am using Eclipse (which has fallen from grace), moving these macros cannot be left in a header file without TONS of errors.
#define UNITTEST_ASSERT(TO_TEST) { \
		const char *usecase_file = __FILE__; \
		const char *usecase_method = __FUNCTION__; \
		uint usecase_lineno = __LINE__; \
		IAS_unittest_assert(usecase_file, usecase_method, usecase_lineno, #TO_TEST, TO_TEST); \
	}

#define UNITTEST_EQUALS(OUTPUT_TO_TEST, EXPECTED) { \
		const char *usecase_file = __FILE__; \
		const char *usecase_method = __FUNCTION__; \
		uint usecase_lineno = __LINE__; \
		IAS_unittest_equals(usecase_file, usecase_method, usecase_lineno, #OUTPUT_TO_TEST, #EXPECTED, OUTPUT_TO_TEST, EXPECTED); \
	}
#define UNITTEST_PATTERN(OUTPUT_TO_TEST, PATTERN) { \
		const char *usecase_file = __FILE__; \
		const char *usecase_method = __FUNCTION__; \
		uint usecase_lineno = __LINE__; \
		IAS_unittest_pattern(usecase_file, usecase_method, usecase_lineno, #OUTPUT_TO_TEST, #PATTERN, OUTPUT_TO_TEST, PATTERN); \
	}

//=== class TextAccumulator =========================================================================================================================
	void UnitTests::test_TextAccumulator(void) {
	//---	TextAccumulator(void) {}
		{	TextAccumulator acc;
			UNITTEST_EQUALS(acc.getLength(), 0u);
			try {
				acc[0];
				FAIL("Should have thrown!");
			} catch ( const String& string ) {
				PASS();
			}
		}

	//---	TextAccumulator& operator+=(char c);
		{	TextAccumulator acc;
			acc += 'a';
			UNITTEST_EQUALS(acc.toString(), "a");
			UNITTEST_EQUALS(acc.getLength(), 1u);

			acc += 'b';
			UNITTEST_EQUALS(acc.toString(), "ab");
			UNITTEST_EQUALS(acc.getLength(), 2u);

			acc.operator+=('c').operator+=('d').operator+=('e');
			UNITTEST_EQUALS(acc.toString(), "abcde");
			UNITTEST_EQUALS(acc.getLength(), 5u);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);

			UNITTEST_EQUALS(acc[0], 'a');
			UNITTEST_EQUALS(acc[4], 'e');
			try {
				acc[5];
				FAIL("Should have thrown!");
			} catch ( const String& string ) {
				PASS();
			}
		}
		{	TextAccumulator acc;
			for ( int i = 0; i < 1024 - 1; i++) {
				acc += 'a';
			}
			UNITTEST_EQUALS(acc.getLength(), TextAccumulator::BUFFER_SIZE_INC - 1);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);
			acc += 'Z';
			UNITTEST_EQUALS(acc.getLength(), TextAccumulator::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC * 2 + 1);
		}

	//---	TextAccumulator& operator+=(const char *str)
		{	TextAccumulator acc;
			for ( int i = 0; i < 1024 - 1; i++) {
				acc += "a";
			}
			UNITTEST_EQUALS(acc.getLength(), TextAccumulator::BUFFER_SIZE_INC - 1);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);
			acc += "Z";
			UNITTEST_EQUALS(acc.getLength(), TextAccumulator::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC * 2 + 1);
		}
		{	TextAccumulator acc;
			for ( int i = 0; i < (1024 - 1)/16; i++) {
				acc += "0123456789ABCDEF";
			}
			UNITTEST_EQUALS(acc.getLength(), TextAccumulator::BUFFER_SIZE_INC - 16);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);
			acc += "0123456789ABCDEF";
			UNITTEST_EQUALS(acc.getLength(), TextAccumulator::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC * 2 + 16);
		}

	//---	TextAccumulator& operator+=(const char *str);
	//---	String toString(void);
		{	TextAccumulator acc;
			UNITTEST_EQUALS(acc.getLength(), 0u);
			acc += "";
			UNITTEST_EQUALS(acc.toString(), "");
			UNITTEST_EQUALS(acc.getLength(), 0u);
			acc += "a";
			UNITTEST_EQUALS(acc.toString(), "a");
			UNITTEST_EQUALS(acc.getLength(), 1u);
			acc += "bcdef";
			UNITTEST_EQUALS(acc.toString(), "abcdef");
			UNITTEST_EQUALS(acc.getLength(), 6u);
		}

	//---	void clear(void) { position = 0; buffer[position] = 0; }
	//---	uint getLength(void) { return position; }
	//---	uint getSize(void) { return current_size; }
		{	TextAccumulator acc;
			acc.clear();
			UNITTEST_EQUALS(acc.getLength(), 0u);
			acc += "";
			acc.clear();
			UNITTEST_EQUALS(acc.getLength(), 0u);
			acc += "abcdef";
			acc.clear();
			UNITTEST_EQUALS(acc.getLength(), 0u);
			for ( int i = 0; i < (1024 - 1)/16; i++) {
				acc += "0123456789ABCDEF";
			}
			UNITTEST_EQUALS(acc.getLength(), 1024u - 16);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);
			acc.clear();
			UNITTEST_EQUALS(acc.getLength(), 0u);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);
			for ( int i = 0; i < (1024 - 1)/16; i++) {
				acc += "0123456789ABCDEF";
			}
			acc += "0123456789ABCDEF";
			UNITTEST_EQUALS(acc.getLength(), 1024u);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC * 2 + 16);
			acc.clear();
			UNITTEST_EQUALS(acc.getLength(), 0u);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC * 2 + 16);
		}
	//---	void resizeBy(uint increment);
		{
			TextAccumulator acc;
			UNITTEST_EQUALS(acc.getLength(), 0u);
			UNITTEST_EQUALS(acc.getSize(), TextAccumulator::BUFFER_SIZE_INC);
		}
	}

//=== class String ==================================================================================================================================
	void UnitTests::test_String(void) {
	//---	String::String(const char *str = nullptr, size_t count = 0); --------------------------------------
		{	String string;
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
		}
		{	String string(nullptr);
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
		}
		{	String string("");
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
		}
		{	String string("A");
			UNITTEST_EQUALS(string.getLength(), 1u);
			UNITTEST_EQUALS(string.getSize(), 1u);
			UNITTEST_EQUALS(strcmp(string.getChars(), "A"), 0);
		}
		{	String string("01234567890123456789012345678901234567890123456789");
			UNITTEST_EQUALS(string.getLength(), 50u);
			UNITTEST_EQUALS(string.getSize(), 50u);
			UNITTEST_EQUALS(strcmp(string.getChars(), "01234567890123456789012345678901234567890123456789"), 0);
		}
		{	String string("01234567890123456789012345678901234567890123456789", 30);
			UNITTEST_EQUALS(string.getLength(), 30u);
			UNITTEST_EQUALS(string.getSize(), 30u);
			UNITTEST_EQUALS(strcmp(string.getChars(), "012345678901234567890123456789"), 0);
		}

	//---	String::String(const String& string); -------------------------------------------------------------
		{	String string("012345678901234567890123456789");
			String new_string(string);
			UNITTEST_EQUALS(new_string.getLength(), 30u);
			UNITTEST_EQUALS(new_string.getSize(), 30u);
			UNITTEST_EQUALS(strcmp(new_string.getChars(), "012345678901234567890123456789"), 0);
		}
		{	String string;
			String new_string(string);
			UNITTEST_EQUALS(new_string.getLength(), 0u);
			UNITTEST_EQUALS(new_string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(new_string.getChars(), ""), 0);
		}

	//---	String(const std::string& std_string); ------------------------------------------------------------
		{
			UNITTEST_EQUALS(String(std::string("")), "");
			UNITTEST_EQUALS(String(std::string("this is a test")), "this is a test");
		}

	//---	explicit String::String(char value); --------------------------------------------------------------
		{	String string('A');
			UNITTEST_EQUALS(string.getLength(), 1u);
			UNITTEST_EQUALS(string.getSize(), 1u);
			UNITTEST_EQUALS(strcmp(string.getChars(), "A"), 0);
		}

	//---	String String::operator+(const char *str) const; --------------------------------------------------
		{	String string;
			String string_result;
			string_result = string + nullptr;
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), ""), 0);
			string_result = string + "";
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), ""), 0);
		}
		{	String string("123456");
			String string_result;
			string_result = string + nullptr;
			UNITTEST_EQUALS(string.getLength(), 6u);
			UNITTEST_EQUALS(string.getSize(), 6u);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "123456"), 0);
			string_result = string + "";
			UNITTEST_EQUALS(string.getLength(), 6u);
			UNITTEST_EQUALS(string.getSize(), 6u);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "123456"), 0);
		}
		{	String string("123456");
			String string_result;
			string_result = string + "7890";
			UNITTEST_EQUALS(string_result.getLength(), 10u);
			UNITTEST_EQUALS(string_result.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "1234567890"), 0);
			string_result = string + "7890" + "abcdefghij";
			UNITTEST_EQUALS(string_result.getLength(), 20u);
			UNITTEST_EQUALS(string_result.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "1234567890abcdefghij"), 0);
		}

	//---	String String::operator+(const String& string) const; ---------------------------------------------
		{	String string;
			String string_result;
			string_result = string + String();
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), ""), 0);
		}
		{	String string("123456");
			String string_result;
			string_result = string + String();
			UNITTEST_EQUALS(string.getLength(), 6u);
			UNITTEST_EQUALS(string.getSize(), 6u);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "123456"), 0);
		}
		{	String string("123456");
			String string_result;
			string_result = string + String("7890");
			UNITTEST_EQUALS(string_result.getLength(), 10u);
			UNITTEST_EQUALS(string_result.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "1234567890"), 0);
			string_result = string + String("7890") + String("abcdefghij");
			UNITTEST_EQUALS(string_result.getLength(), 20u);
			UNITTEST_EQUALS(string_result.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string_result.getChars(), "1234567890abcdefghij"), 0);
		}

	//---	char String::operator[](uint index) const; --------------------------------------------------------
		{	String string;
			UNITTEST_EQUALS(string[0], -1);
		}
		{	String string("1234567890");
			UNITTEST_EQUALS(string[0], '1');
			UNITTEST_EQUALS(string[9], '0');
			UNITTEST_EQUALS(string[10], -1);
			UNITTEST_EQUALS(string[100], -1);
		}

	//---	bool String::operator==(const char *str) const; ---------------------------------------------------
	//---	bool String::operator==(const String& string) const; ----------------------------------------------
		{
			UNITTEST_ASSERT(String() == "");
			UNITTEST_ASSERT(String() == nullptr);
			UNITTEST_ASSERT(String() == String());
		}
		{
			UNITTEST_EQUALS(String("1234567890"), "1234567890");
			UNITTEST_EQUALS(String("123456") + "7890", String("1234567890"));
			UNITTEST_EQUALS(String("1234567890"), "1234567890");
			UNITTEST_EQUALS(String("123456") + "7890", String("1234567890"));
		}
	//---	bool operator==(const std::string& std_string) const; ---------------------------------------------
	//---friend  bool operator==(const std::string& std_string, const String& string); ------------------------
		{
			UNITTEST_EQUALS(String("1234567890"), std::string("1234567890"));
			UNITTEST_ASSERT(!(String("1234567890") == std::string("")));
			UNITTEST_EQUALS(std::string("1234567890"), String("1234567890"));
			UNITTEST_ASSERT(!(std::string("") == String("1234567890")));
		}
	//---	bool String::operator!=(const String& string) const; ----------------------------------------------
		{
			UNITTEST_ASSERT(String("1234567890") != "");
			UNITTEST_ASSERT(String("1234567890") != "1");
			UNITTEST_ASSERT(String("1234567890") != "123456789");
			UNITTEST_ASSERT(String("1234567890") != String(""));
			UNITTEST_ASSERT(String("1234567890") != String("1"));
			UNITTEST_ASSERT(String("1234567890") != String("123456789"));
		}

	//---	bool String::operator>=(const String& string) const;
		{
			UNITTEST_ASSERT(String("a") >= "");
			UNITTEST_ASSERT(String("a") >= nullptr);
			UNITTEST_ASSERT(String("ab") >= "a");
			UNITTEST_ASSERT(String() >= "");
			UNITTEST_ASSERT(String() >= nullptr);
			UNITTEST_ASSERT(String("a") >= "a");
			UNITTEST_ASSERT(String("ab") >= "ab");

			UNITTEST_ASSERT(String("a") >= String());
			UNITTEST_ASSERT(String("ab") >= String("a"));
			UNITTEST_ASSERT(String() >= String());
			UNITTEST_ASSERT(String("a") >= String("a"));
			UNITTEST_ASSERT(String("ab") >= String("ab"));
		}

	//---	bool String::operator<=(const String& string) const;
		{
			UNITTEST_ASSERT(String() <= "a");
			UNITTEST_ASSERT(String("a") <= "ab");
			UNITTEST_ASSERT(String() <= "");
			UNITTEST_ASSERT(String() <= nullptr);
			UNITTEST_ASSERT(String("a") <= "a");
			UNITTEST_ASSERT(String("ab") <= "ab");

			UNITTEST_ASSERT(String() <= String("a"));
			UNITTEST_ASSERT(String("a") <= String("ab"));
			UNITTEST_ASSERT(String() <= String());
			UNITTEST_ASSERT(String("a") <= String("a"));
			UNITTEST_ASSERT(String("ab") <= String("ab"));
		}

	//---	bool String::operator>(const String& string) const;
		{
			UNITTEST_ASSERT(String("a") > "");
			UNITTEST_ASSERT(String("a") > nullptr);
			UNITTEST_ASSERT(String("ab") > "a");

			UNITTEST_ASSERT(String("a") > String());
			UNITTEST_ASSERT(String("ab") > String("a"));
		}

	//---	bool String::operator<(const String& string) const;
		{
			UNITTEST_ASSERT(String() < "a");
			UNITTEST_ASSERT(String("a") < "ab");

			UNITTEST_ASSERT(String() < String("a"));
			UNITTEST_ASSERT(String("a") < String("ab"));
		}

	//---	String& String::operator=(const char *str);
		{	String string;
			string = nullptr;
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string.getChars(), ""), 0);
			string = "123456";
			UNITTEST_EQUALS(string.getLength(), 6u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string.getChars(), "123456"), 0);
			string = "123";
			UNITTEST_EQUALS(string.getLength(), 3u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
			UNITTEST_EQUALS(strcmp(string.getChars(), "123"), 0);
		}
		{	String string("this is a test");
			string = nullptr;
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), 14u);
			UNITTEST_EQUALS(strcmp(string.getChars(), ""), 0);

			string = "123456";
			UNITTEST_EQUALS(string.getLength(), 6u);
			UNITTEST_EQUALS(string.getSize(), 14u);
			UNITTEST_EQUALS(strcmp(string.getChars(), "123456"), 0);

			string = "12345678901234567890";
			UNITTEST_EQUALS(string.getLength(), 20u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC + 20u);
			UNITTEST_EQUALS(strcmp(string.getChars(), "12345678901234567890"), 0);
		}

	//---	String& String::operator=(const String& string); ------------------------------------------------------
		{	String string;
			string = String();
			UNITTEST_EQUALS(string, "");
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);

			string = String("this is a test of the emergency");
			UNITTEST_EQUALS(string, "this is a test of the emergency");
			UNITTEST_EQUALS(string.getLength(), 31u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
		}

	//---	String& String::operator+=(const char *str); ----------------------------------------------------------
		{	String string;
			string += nullptr;
			UNITTEST_EQUALS(string, "");
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
		}
		{	String string;
			string += "";
			UNITTEST_EQUALS(string, "");
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);
		}
		{	String string;
			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 100u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 200u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 300u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 2u + 50u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 400u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 2u + 50u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 500u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 2 + 50u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 600u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 3 + 100u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 700u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 3 + 100u);
		}

		{	String string("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 100u);
			UNITTEST_EQUALS(string.getSize(), 100u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 200u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC + 200u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 300u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC + 200u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 400u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC + 200u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 500u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 3u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 600u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 3u);

			string += "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			UNITTEST_EQUALS(string,
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
					"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
				);
			UNITTEST_EQUALS(string.getLength(), 700u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 3u);
		}

	//---	String& String::operator+=(const String& string); -----------------------------------------------------
		{	String string;
			string += String();
			UNITTEST_EQUALS(string, "");
			UNITTEST_EQUALS(string.getLength(), 0u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);

			string += String("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
			UNITTEST_EQUALS(string, String("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
			UNITTEST_EQUALS(string.getLength(), 100u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC);

			string += String("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
			UNITTEST_EQUALS(string,
							String("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
							"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890")
						);
		//note: this once generated an segfault error
			string += String("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvw");
			UNITTEST_EQUALS(string,
							 String("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
							 "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
							 "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvw")
						);
			String s;
			UNITTEST_EQUALS(string.getLength(), 301u);
			UNITTEST_EQUALS(string.getSize(), String::BUFFER_SIZE_INC * 2u + 51u);
		}

	//---	friend std::ostream& String::operator<<(std::ostream& stream, const String& string); ------------------
		{	String text("");
			std::ostringstream output;
			output << text;
			UNITTEST_EQUALS(output.str(), text);
		}
		{	String text("this is a test");
			std::ostringstream output;
			output << text;
			UNITTEST_EQUALS(output.str(), text);
		}
		{	String text;
			for ( uint i = 0; i < 256; i++ ) {
				text += "0123456789ABCEDF0123456789ABCEDF0123456789ABCEDF0123456789ABCEDF0123456789ABCEDF0123456789ABCEDF0123456789ABCEDF0123456789ABCEDF";
			}
			std::ostringstream output;
			output << text;
			UNITTEST_EQUALS(output.str(), text);
		}

	//---	friend String String::operator+(const char* str, const String& string); -------------------------------
		{
			UNITTEST_EQUALS("" + String("string"), "string");
			UNITTEST_EQUALS("test" + String(""), "test");
			UNITTEST_EQUALS("" + String(""), "");
			UNITTEST_EQUALS("test" + String("string"), "teststring");
		}

	//---	bool String::startsWith(const char * sub); -----------------------------------------------------------
		{
			UNITTEST_ASSERT(String("").startsWith(""));
			UNITTEST_ASSERT(String("abc").startsWith(""));
			UNITTEST_ASSERT(String("abc").startsWith("a"));
			UNITTEST_ASSERT(String("abcdef").startsWith("abc"));
			UNITTEST_ASSERT(!String("abcdef").startsWith("bc"));
		}

	//---	bool String::startsWith(const String& sub); -----------------------------------------------------------
		{
			UNITTEST_ASSERT(String("").startsWith(String("")));
			UNITTEST_ASSERT(String("abc").startsWith(String("")));
			UNITTEST_ASSERT(String("abc").startsWith(String("a")));
			UNITTEST_ASSERT(String("abcdef").startsWith(String("abc")));
			UNITTEST_ASSERT(!String("abcdef").startsWith(String("bc")));
		}

	//---	bool String::contains(const char* sub); -------------------------------------------------------------
		{
			UNITTEST_ASSERT(String("").contains(""));
			UNITTEST_ASSERT(String("abc").contains(""));
			UNITTEST_ASSERT(String("abc").contains("a"));
			UNITTEST_ASSERT(String("abc").contains("b"));
			UNITTEST_ASSERT(String("abcdef").contains("abc"));
			UNITTEST_ASSERT(String("abcdef").contains("bc"));
			UNITTEST_ASSERT(!String("abcdef").contains("bcb"));
			UNITTEST_ASSERT(String("abcdef").contains("def"));
			UNITTEST_ASSERT(!String("abcdef").contains("defg"));
		}

	//---	bool String::contains(const String& sub); -------------------------------------------------------------
		{
			UNITTEST_ASSERT(String("").contains(String("")));
			UNITTEST_ASSERT(String("abc").contains(String("")));
			UNITTEST_ASSERT(String("abc").contains(String("a")));
			UNITTEST_ASSERT(String("abc").contains(String("b")));
			UNITTEST_ASSERT(String("abcdef").contains(String("abc")));
			UNITTEST_ASSERT(String("abcdef").contains(String("bc")));
			UNITTEST_ASSERT(!String("abcdef").contains(String("bcb")));
			UNITTEST_ASSERT(String("abcdef").contains(String("def")));
			UNITTEST_ASSERT(!String("abcdef").contains(String("defg")));
		}

	//---	String& String::clear(void); -----------------------------------------------------------------------------
		{
			UNITTEST_EQUALS(String("").clear().getLength(), 0u);
			UNITTEST_EQUALS(String("").clear().getSize(), String("").getSize());
			UNITTEST_EQUALS(String("abc").clear().getLength(), 0u);
			UNITTEST_EQUALS(String("abc").clear().getSize(), String("").getSize());
		}

	//---	String String::serialize(void) const; ------------------------------------------------------------------
		{
			UNITTEST_EQUALS(String("").serialize(),
					"{ \"String\": "
						"{"
							"\"buffer\": \"\", "
							"\"length\": \"0\", "
							"\"size\": \"250\", "
							"\"compare_fn\": \"strcmp()\", "
							"\"strsub_fn\": \"strsub()\", "
							"\"strstr_fn\": \"strstr()\""
						"}"
					"}");
			UNITTEST_EQUALS((String("") + "1234567890").serialize(),
					"{ \"String\": "
						"{"
							"\"buffer\": \"1234567890\", "
							"\"length\": \"10\", "
							"\"size\": \"10\", "
							"\"compare_fn\": \"strcmp()\", "
							"\"strsub_fn\": \"strsub()\", "
							"\"strstr_fn\": \"strstr()\""
						"}"
					"}");
			UNITTEST_EQUALS(String("abc").serialize(),
					"{ \"String\": "
						"{"
							"\"buffer\": \"abc\", "
							"\"length\": \"3\", "
							"\"size\": \"3\", "
							"\"compare_fn\": \"strcmp()\", "
							"\"strsub_fn\": \"strsub()\", "
							"\"strstr_fn\": \"strstr()\""
						"}"
					"}");
			UNITTEST_EQUALS((String("abc") + "1234567890").serialize(),
					"{ \"String\": "
						"{"
							"\"buffer\": \"abc1234567890\", "
							"\"length\": \"13\", "
							"\"size\": \"13\", "
							"\"compare_fn\": \"strcmp()\", "
							"\"strsub_fn\": \"strsub()\", "
							"\"strstr_fn\": \"strstr()\""
						"}"
					"}");
		}
			//---	String String::toString(void) const; ------------------------------------------------------------------
				{
					UNITTEST_EQUALS(String("").toString(), "");
					UNITTEST_EQUALS((String("") + "1234567890").toString(), "1234567890");
					UNITTEST_EQUALS(String("abc").toString(), "abc");
					UNITTEST_EQUALS((String("abc") + "1234567890").toString(), "abc1234567890");
				}

	//---	String String::strip(void) const;
		{
			UNITTEST_EQUALS(String("").strip(), "");
			UNITTEST_EQUALS(String(" ").strip(), "");
			UNITTEST_EQUALS(String(" \n\t").strip(), "");
			UNITTEST_EQUALS(String("    \n\t").strip(), "");
			UNITTEST_EQUALS(String("abc").strip(), "abc");
			UNITTEST_EQUALS(String(" abc").strip(), "abc");
			UNITTEST_EQUALS(String(" \n\tabc").strip(), "abc");
			UNITTEST_EQUALS(String("abc ").strip(), "abc");
			UNITTEST_EQUALS(String("abc \n\t").strip(), "abc");
			UNITTEST_EQUALS(String(" \n\tabc \n\t").strip(), "abc");
			UNITTEST_EQUALS(String(" \n\tabc \n\t").strip(), "abc");
			UNITTEST_EQUALS(String("ab c").strip(), "ab c");
			UNITTEST_EQUALS(String(" ab c").strip(), "ab c");
			UNITTEST_EQUALS(String(" \t\nab c").strip(), "ab c");
			UNITTEST_EQUALS(String("ab c ").strip(), "ab c");
			UNITTEST_EQUALS(String("ab c \t\n").strip(), "ab c");
			UNITTEST_EQUALS(String(" ab c ").strip(), "ab c");
			UNITTEST_EQUALS(String(" \t\nab \n\tc \n\t").strip(), "ab \n\tc");
		}

	//---	std::vector<String> String::split(const String& needle);
		{	std::vector<String> strings;
			String test="The is a test of emergency broadcast system. This is only a test.";
			strings = test.split(String("xxx"));
			UNITTEST_EQUALS(strings.size(), 1u);
			UNITTEST_EQUALS(strings[0], test);

			strings = test.split(String("emer"));
			UNITTEST_EQUALS(strings.size(), 2u);
			UNITTEST_EQUALS(strings[0], String("The is a test of "));
			UNITTEST_EQUALS(strings[1], String("gency broadcast system. This is only a test."));

			strings = test.split(String("."));
			UNITTEST_EQUALS(strings.size(), 3u);
			UNITTEST_EQUALS(strings[0], String("The is a test of emergency broadcast system"));
			UNITTEST_EQUALS(strings[1], String(" This is only a test"));
			UNITTEST_EQUALS(strings[2], String(""));

			strings = test.split(String("The"));
			UNITTEST_EQUALS(strings.size(), 2u);
			UNITTEST_EQUALS(strings[0], String(""));
			UNITTEST_EQUALS(strings[1], String(" is a test of emergency broadcast system. This is only a test."));

			strings = test.split(String("T"));
			UNITTEST_EQUALS(strings.size(), 3u);
			UNITTEST_EQUALS(strings[0], String(""));
			UNITTEST_EQUALS(strings[1], String("he is a test of emergency broadcast system. "));
			UNITTEST_EQUALS(strings[2], String("his is only a test."));

			strings = test.split(String(" "));
			UNITTEST_EQUALS(strings.size(), 13u);
			UNITTEST_EQUALS(strings[0], String("The"));
			UNITTEST_EQUALS(strings[1], String("is"));
			UNITTEST_EQUALS(strings[2], String("a"));
			UNITTEST_EQUALS(strings[3], String("test"));
			UNITTEST_EQUALS(strings[4], String("of"));
			UNITTEST_EQUALS(strings[5], String("emergency"));
			UNITTEST_EQUALS(strings[6], String("broadcast"));
			UNITTEST_EQUALS(strings[7], String("system."));
			UNITTEST_EQUALS(strings[8], String("This"));
			UNITTEST_EQUALS(strings[9], String("is"));
			UNITTEST_EQUALS(strings[10], String("only"));
			UNITTEST_EQUALS(strings[11], String("a"));
			UNITTEST_EQUALS(strings[12], String("test."));
		}

	//---	std::vector<String> String::split(const char *needle);
		{	std::vector<String> strings;
			String test="The is a test of emergency broadcast system. This is only a test.";
			strings = test.split("xxx");
			UNITTEST_EQUALS(strings.size(), 1u);
			UNITTEST_EQUALS(strings[0], test);

			strings = test.split("emer");
			UNITTEST_EQUALS(strings.size(), 2u);
			UNITTEST_EQUALS(strings[0], String("The is a test of "));
			UNITTEST_EQUALS(strings[1], String("gency broadcast system. This is only a test."));

			strings = test.split(".");
			UNITTEST_EQUALS(strings.size(), 3u);
			UNITTEST_EQUALS(strings[0], String("The is a test of emergency broadcast system"));
			UNITTEST_EQUALS(strings[1], String(" This is only a test"));
			UNITTEST_EQUALS(strings[2], String(""));

			strings = test.split("The");
			UNITTEST_EQUALS(strings.size(), 2u);
			UNITTEST_EQUALS(strings[0], String(""));
			UNITTEST_EQUALS(strings[1], String(" is a test of emergency broadcast system. This is only a test."));

			strings = test.split("T");
			UNITTEST_EQUALS(strings.size(), 3u);
			UNITTEST_EQUALS(strings[0], String(""));
			UNITTEST_EQUALS(strings[1], String("he is a test of emergency broadcast system. "));
			UNITTEST_EQUALS(strings[2], String("his is only a test."));

			strings = test.split(" ");
			UNITTEST_EQUALS(strings.size(), 13u);
			UNITTEST_EQUALS(strings[0], String("The"));
			UNITTEST_EQUALS(strings[1], String("is"));
			UNITTEST_EQUALS(strings[2], String("a"));
			UNITTEST_EQUALS(strings[3], String("test"));
			UNITTEST_EQUALS(strings[4], String("of"));
			UNITTEST_EQUALS(strings[5], String("emergency"));
			UNITTEST_EQUALS(strings[6], String("broadcast"));
			UNITTEST_EQUALS(strings[7], String("system."));
			UNITTEST_EQUALS(strings[8], String("This"));
			UNITTEST_EQUALS(strings[9], String("is"));
			UNITTEST_EQUALS(strings[10], String("only"));
			UNITTEST_EQUALS(strings[11], String("a"));
			UNITTEST_EQUALS(strings[12], String("test."));
		}

	//---	bool String::isEmpty(void) const; ---------------------------------------------------------------------
		{
			UNITTEST_ASSERT(String("").isEmpty());
			UNITTEST_ASSERT(!String("abc").isEmpty());
		}

	//---	uint String::getLength(void) const; -------------------------------------------------------------------
		{
			UNITTEST_EQUALS(String("").getLength(), 0u);
			UNITTEST_EQUALS(String("a").getLength(), 1u);
			UNITTEST_EQUALS(String("1234567890").getLength(), 10u);
		}

	//---	const char* String::getChars(void) const; -------------------------------------------------------------
		{
			UNITTEST_EQUALS(strcmp(String("").getChars(), ""), 0);
			UNITTEST_EQUALS(strcmp(String("a").getChars(), "a"), 0);
			UNITTEST_EQUALS(strcmp(String("1234567890").getChars(), "1234567890"), 0);
		}

	//---	void String::getChars(bool ignore); -------------------------------------------------------------
		{	String string("abcdefg");
			UNITTEST_ASSERT(string == "abcdefg");
			UNITTEST_ASSERT(string != "ABCDEFG");
			UNITTEST_ASSERT(string != "aBcDefg");
			UNITTEST_ASSERT(string.startsWith("abc"));
			UNITTEST_ASSERT(!string.startsWith("ABC"));
			UNITTEST_ASSERT(!string.startsWith("aBc"));
			UNITTEST_ASSERT(!string.startsWith("def"));
			UNITTEST_ASSERT(!string.startsWith("DEF"));
			UNITTEST_ASSERT(!string.startsWith("Def"));
			UNITTEST_ASSERT(string.contains("def"));
			UNITTEST_ASSERT(!string.contains("DEF"));
			UNITTEST_ASSERT(!string.contains("Def"));

			string.enableIgnoreCase(true);
			UNITTEST_ASSERT(string == "abcdefg");
			UNITTEST_ASSERT(string == "ABCDEFG");
			UNITTEST_ASSERT(string == "aBcDefg");
			UNITTEST_ASSERT(string.startsWith("abc"));
			UNITTEST_ASSERT(string.startsWith("ABC"));
			UNITTEST_ASSERT(string.startsWith("aBc"));
			UNITTEST_ASSERT(string.contains("abc"));
			UNITTEST_ASSERT(string.contains("ABC"));
			UNITTEST_ASSERT(string.contains("aBc"));
			UNITTEST_ASSERT(string.contains("def"));
			UNITTEST_ASSERT(string.contains("DEF"));
			UNITTEST_ASSERT(string.contains("Def"));
		}
	//---	String *clone(void) const;
		{

		}
	//---	static bool String::strsub(const char *str, const char *sub); -----------------------------------------
		{
			UNITTEST_ASSERT(String::strsub("", ""));
			UNITTEST_ASSERT(String::strsub("abc", ""));
			UNITTEST_ASSERT(String::strsub("abc", "a"));
			UNITTEST_ASSERT(String::strsub("abc", "abc"));
			UNITTEST_ASSERT(!String::strsub("abc", "abcd"));
			UNITTEST_ASSERT(!String::strsub("abc", "bc"));
			UNITTEST_ASSERT(!String::strsub("abc", "A"));
			UNITTEST_ASSERT(!String::strsub("abc", "ABc"));
		}

	//---	static bool String::strcasesub(const char *str, const char *sub); ----------------------------------------
		{
			UNITTEST_ASSERT(String::strcasesub("", ""));
			UNITTEST_ASSERT(String::strcasesub("abc", ""));
			UNITTEST_ASSERT(String::strcasesub("abc", "a"));
			UNITTEST_ASSERT(String::strcasesub("abc", "abc"));
			UNITTEST_ASSERT(!String::strcasesub("abc", "abcd"));
			UNITTEST_ASSERT(!String::strcasesub("abc", "bc"));
			UNITTEST_ASSERT(String::strcasesub("abc", "A"));
			UNITTEST_ASSERT(String::strcasesub("abc", "ABc"));
		}

	//---	static String String::toString(long long number, uint base = 10); -------------------------------------
		{
			UNITTEST_EQUALS(String::toString(1000), "1000");
			UNITTEST_EQUALS(String::toString(-1000), "-1000");
			UNITTEST_EQUALS(String::toString(0), "0");
			UNITTEST_EQUALS(String::toString(0x0, 16), "0");
			UNITTEST_EQUALS(String::toString(0xFFFF, 16), "FFFF");
			UNITTEST_EQUALS(String::toString(0x0123456789ABCDEF, 16), "123456789ABCDEF");
			UNITTEST_EQUALS(String::toString(01234567, 8), "1234567");
			UNITTEST_EQUALS(String::toString(0x4321, 2), "100001100100001");
		}

	//---	static String String::formatString(const String& fmt, ...); -------------------------------------------
		{	String string = String::formatString("this %s %d", "is a test", 80);
			UNITTEST_EQUALS(string, "this is a test 80");
		}

	//---	String String::hexDump(void) const; -------------------------------------------------------------------
		{	String sample_string(
					"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwx"
					"yzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345"
					"6789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCD"
					"EFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ab"
				);
			String expected_results(
					"0000: 30 31 32 33  34 35 36 37 | 38 39 61 62  63 64 65 66   0123456789abcdef   \n"
					"0010: 67 68 69 6A  6B 6C 6D 6E | 6F 70 71 72  73 74 75 76   ghijklmnopqrstuv   \n"
					"0020: 77 78 79 7A  41 42 43 44 | 45 46 47 48  49 4A 4B 4C   wxyzABCDEFGHIJKL   \n"
					"0030: 4D 4E 4F 50  51 52 53 54 | 55 56 57 58  59 5A 30 31   MNOPQRSTUVWXYZ01   \n"
					"0040: 32 33 34 35  36 37 38 39 | 61 62 63 64  65 66 67 68   23456789abcdefgh   \n"
					"0050: 69 6A 6B 6C  6D 6E 6F 70 | 71 72 73 74  75 76 77 78   ijklmnopqrstuvwx   \n"
					"0060: 79 7A 41 42  43 44 45 46 | 47 48 49 4A  4B 4C 4D 4E   yzABCDEFGHIJKLMN   \n"
					"0070: 4F 50 51 52  53 54 55 56 | 57 58 59 5A  30 31 32 33   OPQRSTUVWXYZ0123   \n"
					"0080: 34 35 36 37  38 39 61 62 | 63 64 65 66  67 68 69 6A   456789abcdefghij   \n"
					"0090: 6B 6C 6D 6E  6F 70 71 72 | 73 74 75 76  77 78 79 7A   klmnopqrstuvwxyz   \n"
					"00A0: 41 42 43 44  45 46 47 48 | 49 4A 4B 4C  4D 4E 4F 50   ABCDEFGHIJKLMNOP   \n"
					"00B0: 51 52 53 54  55 56 57 58 | 59 5A 30 31  32 33 34 35   QRSTUVWXYZ012345   \n"
					"00C0: 36 37 38 39  61 62 63 64 | 65 66 67 68  69 6A 6B 6C   6789abcdefghijkl   \n"
					"00D0: 6D 6E 6F 70  71 72 73 74 | 75 76 77 78  79 7A 41 42   mnopqrstuvwxyzAB   \n"
					"00E0: 43 44 45 46  47 48 49 4A | 4B 4C 4D 4E  4F 50 51 52   CDEFGHIJKLMNOPQR   \n"
					"00F0: 53 54 55 56  57 58 59 5A | 30 31 32 33  34 35 36 37   STUVWXYZ01234567   \n"
					"0100: 38 39 61 62  63 64 65 66 | 67 68 69 6A  6B 6C 6D 6E   89abcdefghijklmn   \n"
					"0110: 6F 70 71 72  73 74 75 76 | 77 78 79 7A  41 42 43 44   opqrstuvwxyzABCD   \n"
					"0120: 45 46 47 48  49 4A 4B 4C | 4D 4E 4F 50  51 52 53 54   EFGHIJKLMNOPQRST   \n"
					"0130: 55 56 57 58  59 5A 30 31 | 32 33 34 35  36 37 38 39   UVWXYZ0123456789   \n"
					"0140: 61 62 63 64  65 66 67 68 | 69 6A 6B 6C  6D 6E 6F 70   abcdefghijklmnop   \n"
					"0150: 71 72 73 74  75 76 77 78 | 79 7A 41 42  43 44 45 46   qrstuvwxyzABCDEF   \n"
					"0160: 47 48 49 4A  4B 4C 4D 4E | 4F 50 51 52  53 54 55 56   GHIJKLMNOPQRSTUV   \n"
					"0170: 57 58 59 5A  30 31 32 33 | 34 35 36 37  38 39 61 62   WXYZ0123456789ab   \n"
					);
			UNITTEST_EQUALS(sample_string.hexDump(), expected_results);

			String full_ascii_expected_results(
					"0000: 01 02 03 04  05 06 07 08 | 09 0A 0B 0C  0D 0E 0F 10   ................   \n"
					"0010: 11 12 13 14  15 16 17 18 | 19 1A 1B 1C  1D 1E 1F 20   ...............    \n"
					"0020: 21 22 23 24  25 26 27 28 | 29 2A 2B 2C  2D 2E 2F 30   !\"#$%&'()*+,-./0   \n"
					"0030: 31 32 33 34  35 36 37 38 | 39 3A 3B 3C  3D 3E 3F 40   123456789:;<=>?@   \n"
					"0040: 41 42 43 44  45 46 47 48 | 49 4A 4B 4C  4D 4E 4F 50   ABCDEFGHIJKLMNOP   \n"
					"0050: 51 52 53 54  55 56 57 58 | 59 5A 5B 5C  5D 5E 5F 60   QRSTUVWXYZ[\\]^_`   \n"
					"0060: 61 62 63 64  65 66 67 68 | 69 6A 6B 6C  6D 6E 6F 70   abcdefghijklmnop   \n"
					"0070: 71 72 73 74  75 76 77 78 | 79 7A 7B 7C  7D 7E 7F 80   qrstuvwxyz{|}~.   \n"
					"0080: 81 82 83 84  85 86 87 88 | 89 8A 8B 8C  8D 8E 8F 90   ................   \n"
					"0090: 91 92 93 94  95 96 97 98 | 99 9A 9B 9C  9D 9E 9F A0   ................   \n"
					"00A0: A1 A2 A3 A4  A5 A6 A7 A8 | A9 AA AB AC  AD AE AF B0   ................   \n"
					"00B0: B1 B2 B3 B4  B5 B6 B7 B8 | B9 BA BB BC  BD BE BF C0   ................   \n"
					"00C0: C1 C2 C3 C4  C5 C6 C7 C8 | C9 CA CB CC  CD CE CF D0   ................   \n"
					"00D0: D1 D2 D3 D4  D5 D6 D7 D8 | D9 DA DB DC  DD DE DF E0   ................   \n"
					"00E0: E1 E2 E3 E4  E5 E6 E7 E8 | E9 EA EB EC  ED EE EF F0   ................   \n"
					"00F0: F1 F2 F3 F4  F5 F6 F7 F8 | F9 FA FB FC  FD FE         ..............     \n"
				);
			char tmps[255];
			for ( uint i = 0; i < sizeof(tmps) - 1; i++ ) {
				tmps[i] = i + 1;
			}
			tmps[sizeof(tmps) - 1] = 0;
			UNITTEST_EQUALS(String(tmps).hexDump(), full_ascii_expected_results);
		}

	//---   String escape_ize(void) const; ------------------------------------------------------------------------
		{
			String string = "";
			UNITTEST_EQUALS(string.escape_ize(), "");
			string = "abcd";
			UNITTEST_EQUALS(string.escape_ize(), "abcd");
			string = "\a\b\f\n\r\t\v";
			UNITTEST_EQUALS(string.escape_ize(), "\\a\\b\\f\\n\\r\\t\\v");
			char tmps[256 + 1];
			for ( uint i = 0; i < sizeof(tmps); i++ ) {
				tmps[i] = i;
			}
			tmps[0] = ' ';
			tmps[sizeof(tmps) - 1] = 0;
			const char *expect =
		//	 01   2   3   4   5   6   7 8 9 A B C D E   F
			" \\x01\\x02\\x03\\x04\\x05\\x06\\a\\b\\t\\n\\v\\f\\r\\x0E\\x0F"
			 "\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17\\x18\\x19\\x1A\\x1B\\x1C\\x1D\\x1E\\x1F"
		//	 0123456789ABCDEF
			" !\"#$%&'()*+,-./"
			"0123456789:;<=>?"
			"@ABCDEFGHIJKLMNO"
			"PQRSTUVWXYZ[\\]^_"
			"`abcdefghijklmno"
			"pqrstuvwxyz{|}~\\x7F"
			"\\x80\\x81\\x82\\x83\\x84\\x85\\x86\\x87\\x88\\x89\\x8A\\x8B\\x8C\\x8D\\x8E\\x8F"
			"\\x90\\x91\\x92\\x93\\x94\\x95\\x96\\x97\\x98\\x99\\x9A\\x9B\\x9C\\x9D\\x9E\\x9F"
			"\\xA0\\xA1\\xA2\\xA3\\xA4\\xA5\\xA6\\xA7\\xA8\\xA9\\xAA\\xAB\\xAC\\xAD\\xAE\\xAF"
			"\\xB0\\xB1\\xB2\\xB3\\xB4\\xB5\\xB6\\xB7\\xB8\\xB9\\xBA\\xBB\\xBC\\xBD\\xBE\\xBF"
			"\\xC0\\xC1\\xC2\\xC3\\xC4\\xC5\\xC6\\xC7\\xC8\\xC9\\xCA\\xCB\\xCC\\xCD\\xCE\\xCF"
			"\\xD0\\xD1\\xD2\\xD3\\xD4\\xD5\\xD6\\xD7\\xD8\\xD9\\xDA\\xDB\\xDC\\xDD\\xDE\\xDF"
			"\\xE0\\xE1\\xE2\\xE3\\xE4\\xE5\\xE6\\xE7\\xE8\\xE9\\xEA\\xEB\\xEC\\xED\\xEE\\xEF"
			"\\xF0\\xF1\\xF2\\xF3\\xF4\\xF5\\xF6\\xF7\\xF8\\xF9\\xFA\\xFB\\xFC\\xFD\\xFE\\xFF";
			UNITTEST_EQUALS(String(tmps).escape_ize(), expect);
		}

	//---	static String String::wideCharToString(ushort wchar); -------------------------------------------------
		{
			UNITTEST_EQUALS(String::wideCharToString('a'),    "   a");
			UNITTEST_EQUALS(String::wideCharToString('ab'),   "  ab");
			UNITTEST_EQUALS(String::wideCharToString('abc'),  " abc");
			UNITTEST_EQUALS(String::wideCharToString('abcd'), "abcd");
		}

	//--- friend  std::ostream& operator<<(std::ostream& stream, const String& string); ---------------------------
		{	std::ostringstream output;
			String text("this is a test");
			output << text;
			UNITTEST_EQUALS(output.str(), text);
		}
	}

//=== class Token ===================================================================================================================================
	void UnitTests::test_Token(void) {
	//--- Token::Token(void);
	//--- Token::Token(TokenType type);
	//--- Token::Token(const Token& token);
		{	Token token1;
			UNITTEST_EQUALS(token1.type, TokenType::eEmpty);
			UNITTEST_EQUALS(token1.text, "");
			UNITTEST_EQUALS(token1.whitespace, "");

			Token token2(token1);
			UNITTEST_EQUALS(token2.type, TokenType::eEmpty);
			UNITTEST_EQUALS(token2.text, "");
			UNITTEST_EQUALS(token2.whitespace, "");

			Token token3(TokenType::eAmpersand);
			UNITTEST_EQUALS(token3.type, TokenType::eAmpersand);
			UNITTEST_EQUALS(token3.text, "&");
			UNITTEST_EQUALS(token3.whitespace, "");

			Token token4(token3);
			UNITTEST_EQUALS(token4.type, TokenType::eAmpersand);
			UNITTEST_EQUALS(token4.text, "&");
			UNITTEST_EQUALS(token4.whitespace, "");

	//--- Token& Token::operator=(const Token& token);
			token1 = token4;
			UNITTEST_EQUALS(token1.type, TokenType::eAmpersand);
			UNITTEST_EQUALS(token1.text, "&");
			UNITTEST_EQUALS(token1.whitespace, "");
		}

	//--- Token::Token(wchar type);
		{	Token token('>=');
			UNITTEST_EQUALS(token.type, TokenType::eGreaterEquals);
			UNITTEST_EQUALS(token.text, ">=");
			UNITTEST_EQUALS(token.whitespace, "");
		}

	//--- Token::Token(TokenType type, String text, String whitespace = "");
		{	Token token1('word', "asperger's");
			UNITTEST_EQUALS(token1.type, 'word');
			UNITTEST_EQUALS(token1.text, "asperger's");
			UNITTEST_EQUALS(token1.whitespace, "");

			Token token2(token1);
			UNITTEST_EQUALS(token2.type, 'word');
			UNITTEST_EQUALS(token2.text, "asperger's");
			UNITTEST_EQUALS(token2.whitespace, "");
		}

	//--- bool Token::isEOF(void);
		{	Token token(TokenType::eEOF);
			UNITTEST_ASSERT(token.isEOF());
		}

	//--- bool Token::operator==(const Token& token) const;
		{	Token token1('word', "asperger's");
			Token token2(token1);
			UNITTEST_ASSERT(token1 == token1);
		}

	//--- bool Token::operator==(TokenType type) const;
		{	Token token('word', "asperger's");
			UNITTEST_ASSERT(token == 'word');
		}

	//--- bool Token::operator==(const String& text) const;
		{	Token token('word', "asperger's");
			UNITTEST_ASSERT(token == "asperger's");
		}

	//--- bool Token::operator==(int value) const;
		{	Token token(TokenType::eAmpersand, "&");
			UNITTEST_ASSERT(token == '&');
		}

	//--- bool Token::operator!=(const Token& token) const;
		{	Token token('word', "asperger's");
			UNITTEST_ASSERT(token != Token(TokenType::eNumber, "asperger's"));
		}

	//--- bool Token::operator!=(TokenType type) const;
		{	Token token('word', "asperger's");
			UNITTEST_ASSERT(token != TokenType::eNumber);
		}

	//--- friend bool Token::operator==(int value, Token token);
		{
			UNITTEST_ASSERT(TokenType::eAmpersand == Token(TokenType::eAmpersand));
		}

	//--- static String Token::wideCharToString(wchar wide_char);
		{
			UNITTEST_EQUALS(Token::wideCharToString('word'), "word");
			UNITTEST_EQUALS(Token::wideCharToString('erp'), " erp");
		}

	//--- virtual String Token::toString(void) const;
		{
			UNITTEST_EQUALS(Token('str', "\"this is a test\"").toString(), "\"this is a test\"");
		}

	//--- virtual String Token::serialize(void) const;
		{
			UNITTEST_EQUALS(Token('str', "\"this is a test\"").serialize(), "{ \"Token\": {\"type\": \"' str'\", \"text\": \"\"this is a test\"\", \"whitespace\": \"\"}}");
		}

	//--- bool Token::operator<(const Token& token) const;
		{
			UNITTEST_ASSERT(Token('a') < Token('b'));
			UNITTEST_ASSERT(Token(TokenType::eEOF) < Token('b'));
			UNITTEST_ASSERT(Token('abc') < Token('abcd'));
		}

	//---	static bool Token::isWordStart(int c);
	//---	static bool Token::isWord(int c);
		{
			for ( int i = ' '; i < 128; i++ ) {
				if ( ('A' <= i  &&  i <= 'Z')  ||  ('a' <= i  &&  i <= 'z')  || i == '_' ) {
					UNITTEST_ASSERT(Token::isWordStart(i));
				}
				if ( ('A' <= i  &&  i <= 'Z')  ||  ('a' <= i  &&  i <= 'z')  || i == '_'  ||  ('0' <= i  &&  i <= '9') ) {
					UNITTEST_ASSERT(Token::isWord(i));
				}
				if ( !( ('A' <= i  &&  i <= 'Z')  ||  ('a' <= i  &&  i <= 'z')  || i == '_'  ||  ('0' <= i  &&  i <= '9') ) ){
					UNITTEST_ASSERT(!Token::isWord(i));
				}
			}
		}

	//--- friend std::ostream& Token::operator<<(std::ostream& ostream, const Token& token);
		{	std::ostringstream output;
			output << Token('word', "emergency", "//**/");
			UNITTEST_EQUALS(output.str(), "emergency");
		}

	}

//=== class StringStream ============================================================================================================================
	void UnitTests::test_StringStream(void) {
	//---	virtual int  back(void);
	//---	virtual int  current(void);
	//---	virtual int  next(void);
		{	cchar *test_string = "`~!@#$%^&*()-=+[{]}\\|;:'\",<.>/?0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ _";
			Stream *stream = new StringStream(test_string);
			UNITTEST_EQUALS(stream->next(), '`');
			UNITTEST_EQUALS(stream->next(), '~');
			UNITTEST_EQUALS(stream->next(), '!');
			UNITTEST_EQUALS(stream->next(), '@');
			UNITTEST_EQUALS(stream->current(), '@');
			UNITTEST_EQUALS(stream->next(), '#');
			UNITTEST_EQUALS(stream->next(), '$');
			UNITTEST_EQUALS(stream->next(), '%');
			UNITTEST_EQUALS(stream->next(), '^');
			UNITTEST_EQUALS(stream->next(), '&');
			UNITTEST_EQUALS(stream->next(), '*');
			UNITTEST_EQUALS(stream->next(), '(');
			UNITTEST_EQUALS(stream->next(), ')');
			UNITTEST_EQUALS(stream->back(), '('); // <-- back()
			UNITTEST_EQUALS(stream->next(), ')');
			UNITTEST_EQUALS(stream->next(), '-');
			UNITTEST_EQUALS(stream->next(), '=');
			UNITTEST_EQUALS(stream->next(), '+');
			UNITTEST_EQUALS(stream->next(), '[');
			UNITTEST_EQUALS(stream->next(), '{');
			UNITTEST_EQUALS(stream->current(), '{');
			UNITTEST_EQUALS(stream->next(), ']');
			UNITTEST_EQUALS(stream->next(), '}');
			UNITTEST_EQUALS(stream->next(), '\\');
			UNITTEST_EQUALS(stream->next(), '|');
			UNITTEST_EQUALS(stream->next(), ';');
			UNITTEST_EQUALS(stream->next(), ':');
			UNITTEST_EQUALS(stream->next(), '\'');
			UNITTEST_EQUALS(stream->next(), '"');
			UNITTEST_EQUALS(stream->next(), ',');
			UNITTEST_EQUALS(stream->next(), '<');
			UNITTEST_EQUALS(stream->next(), '.');
			UNITTEST_EQUALS(stream->next(), '>');
			UNITTEST_EQUALS(stream->next(), '/');
			UNITTEST_EQUALS(stream->next(), '?');
			UNITTEST_EQUALS(stream->next(), '0');
			UNITTEST_EQUALS(stream->next(), '1');
			UNITTEST_EQUALS(stream->next(), '2');
			UNITTEST_EQUALS(stream->next(), '3');
			UNITTEST_EQUALS(stream->next(), '4');
			UNITTEST_EQUALS(stream->next(), '5');
			UNITTEST_EQUALS(stream->next(), '6');
			UNITTEST_EQUALS(stream->next(), '7');
			UNITTEST_EQUALS(stream->next(), '8');
			UNITTEST_EQUALS(stream->next(), '9');
			UNITTEST_EQUALS(stream->current(), '9');
			UNITTEST_EQUALS(stream->next(), 'a');
			UNITTEST_EQUALS(stream->next(), 'b');
			UNITTEST_EQUALS(stream->next(), 'c');
			UNITTEST_EQUALS(stream->next(), 'd');
			UNITTEST_EQUALS(stream->current(), 'd');
			UNITTEST_EQUALS(stream->next(), 'e');
			UNITTEST_EQUALS(stream->next(), 'f');
			UNITTEST_EQUALS(stream->next(), 'g');
			UNITTEST_EQUALS(stream->next(), 'h');
			UNITTEST_EQUALS(stream->next(), 'i');
			UNITTEST_EQUALS(stream->next(), 'j');
			UNITTEST_EQUALS(stream->next(), 'k');
			UNITTEST_EQUALS(stream->next(), 'l');
			UNITTEST_EQUALS(stream->next(), 'm');
			UNITTEST_EQUALS(stream->next(), 'n');
			UNITTEST_EQUALS(stream->next(), 'o');
			UNITTEST_EQUALS(stream->next(), 'p');
			UNITTEST_EQUALS(stream->next(), 'q');
			UNITTEST_EQUALS(stream->next(), 'r');
			UNITTEST_EQUALS(stream->next(), 's');
			UNITTEST_EQUALS(stream->next(), 't');
			UNITTEST_EQUALS(stream->next(), 'u');
			UNITTEST_EQUALS(stream->next(), 'v');
			UNITTEST_EQUALS(stream->next(), 'w');
			UNITTEST_EQUALS(stream->next(), 'x');
			UNITTEST_EQUALS(stream->next(), 'y');
			UNITTEST_EQUALS(stream->next(), 'z');
			stream->setBookmark();
			UNITTEST_EQUALS(stream->next(), 'A');
			UNITTEST_EQUALS(stream->next(), 'B');
			UNITTEST_EQUALS(stream->next(), 'C');
			UNITTEST_EQUALS(stream->next(), 'D');
			UNITTEST_EQUALS(stream->next(), 'E');
			UNITTEST_EQUALS(stream->next(), 'F');
			UNITTEST_EQUALS(stream->next(), 'G');
			UNITTEST_EQUALS(stream->next(), 'H');
			UNITTEST_EQUALS(stream->next(), 'I');
			UNITTEST_EQUALS(stream->next(), 'J');
			UNITTEST_EQUALS(stream->next(), 'K');
			UNITTEST_EQUALS(stream->next(), 'L');
			UNITTEST_EQUALS(stream->next(), 'M');
			UNITTEST_EQUALS(stream->next(), 'N');
			UNITTEST_EQUALS(stream->next(), 'O');
			UNITTEST_EQUALS(stream->next(), 'P');
			UNITTEST_EQUALS(stream->next(), 'Q');
			UNITTEST_EQUALS(stream->next(), 'R');
			UNITTEST_EQUALS(stream->next(), 'S');
			UNITTEST_EQUALS(stream->next(), 'T');
			UNITTEST_EQUALS(stream->next(), 'U');
			UNITTEST_EQUALS(stream->next(), 'V');
			UNITTEST_EQUALS(stream->next(), 'W');
			UNITTEST_EQUALS(stream->next(), 'X');
			UNITTEST_EQUALS(stream->next(), 'Y');
			UNITTEST_EQUALS(stream->next(), 'Z');
			UNITTEST_EQUALS(stream->next(), ' ');
			UNITTEST_EQUALS(stream->next(), '_');
			stream->recallBookmark();
			UNITTEST_EQUALS(stream->next(), 'z');
			stream->recallBookmark();
			UNITTEST_EQUALS(stream->current(), '`');
			stream->recallBookmark();
			UNITTEST_EQUALS(stream->next(), '`');
			delete stream;
		}

	//--- StringStream::StringStream(const String& string): string(string) {}
		{
			UNITTEST_EQUALS(StringStream(String("unit test")).current(), 'u');
			UNITTEST_EQUALS(StringStream(String("test")).next(), 't');
			UNITTEST_EQUALS(StringStream(String()).next(), -1);
		}

	//--- virtual int StringStream::back(void);
	//--- virtual int StringStream::next(void);
	//--- virtual int StringStream::current(void);
		{	Stream *stream = new StringStream("123\n456");
			UNITTEST_EQUALS(stream->getLineNumber(), 1u);
			UNITTEST_EQUALS(stream->current(), '1');
			UNITTEST_EQUALS(stream->next(), '2');
			UNITTEST_EQUALS(stream->back(), '1');
			UNITTEST_EQUALS(stream->current(), '1');
			UNITTEST_EQUALS(stream->next(), '2');
			UNITTEST_EQUALS(stream->next(), '3');
			UNITTEST_EQUALS(stream->getLineNumber(), 1u);
			UNITTEST_EQUALS(stream->next(), '\n');
			UNITTEST_EQUALS(stream->getLineNumber(), 2u);
			UNITTEST_EQUALS(stream->back(), '3');
			UNITTEST_EQUALS(stream->getLineNumber(), 1u);
			UNITTEST_EQUALS(stream->next(), '\n');
			UNITTEST_EQUALS(stream->next(), '4');
			UNITTEST_EQUALS(stream->getLineNumber(), 2u);
			UNITTEST_EQUALS(stream->back(), '\n');
			UNITTEST_EQUALS(stream->getLineNumber(), 2u);
			UNITTEST_EQUALS(stream->back(), '3');
			UNITTEST_EQUALS(stream->getLineNumber(), 1u);
			delete stream;
		}

	//--- virtual int StringStream::peek(uint ahead = 0) const;
		{	Stream *stream = new StringStream("123\n456");
			UNITTEST_EQUALS(stream->peek(), '1');
			UNITTEST_EQUALS(stream->peek(1), '2');
			UNITTEST_EQUALS(stream->next(), '1');
			UNITTEST_EQUALS(stream->peek(2), '3');
		}

	//--- virtual bool StringStream::peek(const String& seed, bool consume = false); // <-- method not 'const' b/c revises object if "skip_seed" true
		{	String string("This is a test of the emergency broadcast system. This is only a test.");
			Stream *stream = new StringStream(string);
			UNITTEST_ASSERT(!stream->peek(""));
			UNITTEST_ASSERT(!stream->peek("this"));
			UNITTEST_ASSERT(stream->peek("This"));
			UNITTEST_ASSERT(stream->peek("This", true));
			UNITTEST_ASSERT(stream->peek(" is"));
			delete stream;

			string.enableIgnoreCase(true);
			stream = new StringStream(string);
			UNITTEST_ASSERT(stream->peek("tHIS"));
			UNITTEST_ASSERT(stream->peek("This"));
			UNITTEST_ASSERT(stream->peek("this", true));
			UNITTEST_ASSERT(stream->peek(" is"));
		}

	//--- virtual bool StringStream::isEOF(void) const;
		{	StringStream *stream = new StringStream("");
			UNITTEST_ASSERT(stream->isEOF());
			delete stream;

			stream = new StringStream("t");
			UNITTEST_ASSERT(!stream->isEOF());
			stream->next();
			stream->next();
			UNITTEST_ASSERT(stream->isEOF());
			delete stream;
		}

	//--- virtual void StringStream::skip(uint chars = 1);
		{	StringStream *stream = new StringStream("");
			stream->skip();
			delete stream;

			stream = new StringStream("testing");
			stream->skip();
			UNITTEST_EQUALS(stream->current(), 'e');
			UNITTEST_EQUALS(stream->next(), 's');
			stream->skip(0);
			UNITTEST_EQUALS(stream->current(), 's');
			stream->skip(2);
			UNITTEST_EQUALS(stream->current(), 'i');
			stream->skip(3);
			UNITTEST_EQUALS(stream->current(), EOF);
			delete stream;
		}

	//--- virtual uint StringStream::getLength(void) const;
		{	StringStream *stream = new StringStream("");
			UNITTEST_EQUALS(stream->getLength(), 0u);
			delete stream;

			stream = new StringStream("testing");
			UNITTEST_EQUALS(stream->getLength(), 7u);
			delete stream;
		}

	//--- virtual uint StringStream::getPosition(void) const;
		{	StringStream *stream = new StringStream("");
			UNITTEST_EQUALS(stream->getPosition(), 0u);
			delete stream;

			stream = new StringStream("testing");
			stream->current();
			UNITTEST_EQUALS(stream->getPosition(), 0u);
			stream->current();
			UNITTEST_EQUALS(stream->getPosition(), 0u);
			stream->next();
			UNITTEST_EQUALS(stream->getPosition(), 1u);
			stream->skip(10);
			UNITTEST_EQUALS(stream->getPosition(), 7u);
			delete stream;
		}

	//--- virtual int StringStream::operator*(void);
	//--- virtual int StringStream::operator++(void);
	//--- virtual int StringStream::operator--(void);
		{	StringStream stream1("");
			UNITTEST_EQUALS(*stream1, EOF);

			StringStream stream2("testing");
			UNITTEST_EQUALS(*stream2, 't');
			UNITTEST_EQUALS(*stream2, 't');
			UNITTEST_EQUALS(++stream2, 'e');
			UNITTEST_EQUALS(*stream2, 'e');
			UNITTEST_EQUALS(--stream2, 't');
			UNITTEST_EQUALS(--stream2, 't');
		}

	//--- virtual String StringStream::toString(void) const;
		{	StringStream stream("");
			UNITTEST_EQUALS(stream.toString(), "[[]StringStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
		}
		{	StringStream stream("this is a test");
			UNITTEST_EQUALS(stream.toString(), "[[]StringStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
			stream.next();
			stream.next();
			UNITTEST_EQUALS(stream.toString(), "[[]StringStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
		}
	//--- friend std::ostream& StringStream::operator<<(std::ostream& stream, const StringStream& string)
		{	std::ostringstream output;
			String text("this is a test");
			StringStream stream(text);
			output << stream;
			UNITTEST_EQUALS(output.str(), "[[]StringStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
		}
	}

//=== class TokenStream =============================================================================================================================
	void UnitTests::test_TokenStream(void) {
	//---	TokenStream(Stream *stream);
	//---	virtual ~TokenStream(void);
	//---	virtual Token current(void);
	//---	virtual Token next(void);
	//---	virtual bool  isEOF(void);
		{	cchar *test_string_doublequote = "\"test string\"";
			TokenStream stream(test_string_doublequote);
			UNITTEST_EQUALS(stream.next(), Token(TokenType::eString, test_string_doublequote));
		}
		{ 	TokenStream stream1("LastModifiedDateTime\"05/07_2010 21:36:12.0000\"");
			UNITTEST_EQUALS(stream1.next(), Token(TokenType::eWord, "LastModifiedDateTime"));
			UNITTEST_EQUALS(stream1.next(), Token(TokenType::eString, "\"05/07_2010 21:36:12.0000\""));
			TokenStream stream2("word\"test string\"12345");
			UNITTEST_EQUALS(stream2.next(), Token(TokenType::eWord, "word"));
			UNITTEST_EQUALS(stream2.next(), Token(TokenType::eString, "\"test string\""));
			UNITTEST_EQUALS(stream2.next(), Token(TokenType::eNumber, "12345"));
			TokenStream stream3("!\"test string\"&");
			UNITTEST_EQUALS(stream3.next(), Token(TokenType::eBang, "!"));
			UNITTEST_EQUALS(stream3.next(), Token(TokenType::eString, "\"test string\""));
			UNITTEST_EQUALS(stream3.next(), Token(TokenType::eAmpersand, "&"));
		}
		{ 	TokenStream stream1("word'test string'");
			UNITTEST_EQUALS(stream1.next(), Token(TokenType::eWord, "word"));
			UNITTEST_EQUALS(stream1.next(), Token(TokenType::eString, "'test string'"));
			TokenStream stream2("word'test string'12345");
			UNITTEST_EQUALS(stream2.next(), Token(TokenType::eWord, "word"));
			UNITTEST_EQUALS(stream2.next(), Token(TokenType::eString, "'test string'"));
			UNITTEST_EQUALS(stream2.next(), Token(TokenType::eNumber, "12345"));
			TokenStream stream3("!'test string'&");
			UNITTEST_EQUALS(stream3.next(), Token(TokenType::eBang, "!"));
			UNITTEST_EQUALS(stream3.next(), Token(TokenType::eString, "'test string'"));
			UNITTEST_EQUALS(stream3.next(), Token(TokenType::eAmpersand, "&"));
		}
		{
			cchar *test_string =
					"`~!!@#$%^&*()-=+[{]}\\|;:,<..>/?"
					" 0 1 2 3 4 5 6 7 8 9"
					" a b c d e f g h i j k l m n o p q r s t u v w x y z"
					" A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"
					" _";
			TokenStream *stream = new TokenStream(new StringStream(test_string));
			UNITTEST_EQUALS(stream->current(), Token(TokenType::eBackQuote, "`"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eTilde, "~"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eExclamationPoint, "!"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eBang, "!"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eAt, "@"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eHash, "#"));
			UNITTEST_EQUALS(stream->current(), Token(TokenType::eHash, "#"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eDollar, "$"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::ePercent, "%"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eCircumflex, "^"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eAmpersand, "&"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eAsterix, "*"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eLeftParen, "("));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eRightParen, ")"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eHyphen, "-"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eEquals, "="));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::ePlus, "+"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eLeftBracket, "["));
			UNITTEST_EQUALS(stream->current(), Token(TokenType::eLeftBracket, "["));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eLeftBrace, "{"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eRightBracket, "]"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eRightBrace, "}"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eBackslash, "\\"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eBar, "|"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eSemicolon, ";"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eColon, ":"));
//			UNITTEST_EQUALS(stream->next(), Token(TokenType::eSingleQuote, "'"));
//			UNITTEST_EQUALS(stream->next(), Token(TokenType::eDoubleQuote, "\""));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eComma, ","));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eLeftAngleBracket, "<"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::ePeriod, "."));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eDot, "."));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eRightAngleBracket, ">"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eSlash, "/"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eQuestionMark, "?"));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "0", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "1", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "2", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "3", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "4", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "5", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "6", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "7", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "8", " "));
			UNITTEST_EQUALS(stream->next(), Token(TokenType::eNumber, "9", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "a", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "b", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "c", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "d", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "e", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "f", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "g", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "h", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "i", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "j", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "k", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "l", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "m", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "n", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "o", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "p", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "q", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "r", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "s", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "t", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "u", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "v", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "w", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "x", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "y", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "z", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "A", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "B", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "C", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "D", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "E", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "F", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "G", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "H", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "I", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "J", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "K", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "L", " "));
			stream->setBookmark();
			UNITTEST_EQUALS(stream->next(), Token('word', "M", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "N", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "O", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "P", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "Q", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "R", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "S", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "T", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "U", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "V", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "W", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "X", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "Y", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "Z", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "_", " "));
			UNITTEST_ASSERT(stream->next().isEOF());
			stream->recallBookmark();
			UNITTEST_EQUALS(stream->current(), Token('word', "M", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "N", " "));
			UNITTEST_EQUALS(stream->next(), Token('word', "O", " "));
			stream->recallBookmark(); // <-- recall*() without a set*() resets to the beginning of the stream!
			UNITTEST_EQUALS(stream->current(), Token(TokenType::eBackQuote, "`"));
			delete stream;
		}

	//---	TokenStream(const String& string);
	//---	void TokenStream::scoopString(void);
		{	TokenStream *stream = new TokenStream("'hi'");
			UNITTEST_EQUALS(stream->scoopString(""), Token(TokenType::eChar, "'hi'"));
			delete stream;

			stream = new TokenStream("'hi");
			UNITTEST_EQUALS(stream->scoopString(""), Token(TokenType::eChar, "'hi"));
			delete stream;

			stream = new TokenStream("'\"hi'");
			UNITTEST_EQUALS(stream->scoopString(""), Token(TokenType::eChar, "'\"hi'"));
			delete stream;
		}
		{	TokenStream *stream = new TokenStream("\"hi\"");
			UNITTEST_EQUALS(stream->scoopString(""), Token('str', "\"hi\""));
			delete stream;

			stream = new TokenStream("\"hi");
			UNITTEST_EQUALS(stream->scoopString(""), Token('str', "\"hi"));
			delete stream;

			stream = new TokenStream("\"'hi\"");
			UNITTEST_EQUALS(stream->scoopString(""), Token('str', "\"'hi\""));
			delete stream;
		}

	//---	void TokenStream::scoopWord(void);
		{	TokenStream stream("");
			UNITTEST_EQUALS(stream.scoopWord(""), TokenType::eEOF);
		}
		{	TokenStream stream("boingus");
			UNITTEST_EQUALS(stream.scoopWord(""), Token('word', "boingus", ""));
		}
		{	TokenStream stream("boingus ");
			UNITTEST_EQUALS(stream.scoopWord(""), Token('word', "boingus", ""));
		}
		{	TokenStream stream("boingus>");
			UNITTEST_EQUALS(stream.scoopWord(""), Token('word', "boingus", ""));
		}

	//---	void TokenStream::scoopDigits(char *str, uint& index, uint MAX);
		{	TextAccumulator acc;

			TokenStream("").scoopDigits(acc);
			UNITTEST_EQUALS(acc.toString(), "")

			acc.clear();
			TokenStream("1").scoopDigits(acc);
			UNITTEST_EQUALS(acc.toString(), "1")

			acc.clear();
			TokenStream("1 ").scoopDigits(acc);
			UNITTEST_EQUALS(acc.toString(), "1")

			acc.clear();
			TokenStream("1234567890").scoopDigits(acc);
			UNITTEST_EQUALS(acc.toString(), "1234567890")

			acc.clear();
			TokenStream(" 1 ").scoopDigits(acc);
			UNITTEST_EQUALS(acc.toString(), "")
		}

	//---	void TokenStream::scoopNumber(void);
		{	TokenStream stream("");
			stream.scoopNumber("");
			UNITTEST_ASSERT(stream.isEOF());
		}
		{	TokenStream stream("01234");
	//		UNITTEST_EQUALS(stream.scoopNumber(""), Token(TokenType::eNumber, "01234", ""));
			UNITTEST_EQUALS(stream.next(), Token(TokenType::eNumber, "01234", ""));
			UNITTEST_ASSERT(stream.next().isEOF());
		}

	//---	bool TokenStream::scoopSpace(void);
		{	TokenStream stream(" \t\n\r\f1234");
			String spaces = stream.scoopSpace();
			UNITTEST_EQUALS(stream.scoopNumber(spaces), Token(TokenType::eNumber, "1234", " \t\n\r\f"));
			UNITTEST_ASSERT(stream.next().isEOF());
		}
		{	TokenStream stream("0 1234\t1111");
			String spaces = stream.scoopSpace();
			UNITTEST_EQUALS(stream.scoopNumber(spaces), Token(TokenType::eNumber, "0", ""));
			spaces = stream.scoopSpace();
			UNITTEST_EQUALS(spaces, " ");
			UNITTEST_EQUALS(stream.scoopNumber(spaces), Token(TokenType::eNumber, "1234", " "));
			spaces = stream.scoopSpace();
			UNITTEST_EQUALS(spaces, "\t");
			UNITTEST_EQUALS(stream.scoopNumber(spaces), Token(TokenType::eNumber, "1111", "\t"));
		}
		{	TokenStream stream("1234.");
			UNITTEST_EQUALS(stream.scoopNumber(""), Token(TokenType::eNumber, "1234", ""));
		}
		{	TokenStream stream("1234e");
			UNITTEST_EQUALS(stream.scoopNumber(""), Token(TokenType::eNumber, "1234", ""));
		}

	//---	bool TokenStream::isEOF(void);
		{	TokenStream stream("");
			UNITTEST_ASSERT(stream.isEOF());
		}

	//---	Token TokenStream::current(void); -------------------------------------------------------------------------------------
	//---	Token TokenStream::next(void);
		{	TokenStream stream("");
			UNITTEST_EQUALS(stream.current(), Token(TokenType::eEOF));
		}
		{	TokenStream stream("abc");
			UNITTEST_EQUALS(stream.current(), Token('word', "abc"));
			UNITTEST_EQUALS(stream.current(), Token('word', "abc"));
		}

	//---	String TokenStream::toString(void) const; -----------------------------------------------------------------------------
		{	TokenStream stream1("");
			UNITTEST_PATTERN(stream1.toString(), "[[]TokenStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
			stream1.current();
			UNITTEST_PATTERN(stream1.toString(), "[[]TokenStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
			TokenStream stream2("abc");
			stream2.current();
			UNITTEST_PATTERN(stream2.toString(), "[[]TokenStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
		}

	//---	String TokenStream::serialize(void) const; -----------------------------------------------------------------------------
		{	TokenStream stream1("");
			UNITTEST_EQUALS(stream1.serialize(), "{ \"TokenStream\": {\"current_token\": { \"Token\": {\"type\": \"'NULL'\", \"text\": \"\", \"whitespace\": \"\"}}, \"stream\": { \"StringStream\": { \"position\": \"0\", \"bookmark\": \"0\" } }}}");
			stream1.current();
			UNITTEST_EQUALS(stream1.serialize(), "{ \"TokenStream\": {\"current_token\": { \"Token\": {\"type\": \"' EOF'\", \"text\": \"EOF\", \"whitespace\": \"\"}}, \"stream\": { \"StringStream\": { \"position\": \"0\", \"bookmark\": \"0\" } }}}");
			TokenStream stream2("abc");
			stream2.current();
			UNITTEST_EQUALS(stream2.serialize(), "{ \"TokenStream\": {\"current_token\": { \"Token\": {\"type\": \"'word'\", \"text\": \"abc\", \"whitespace\": \"\"}}, \"stream\": { \"StringStream\": { \"position\": \"3\", \"bookmark\": \"0\" } }}}");
		}

	//---	void TokenStream::mustBe(std::set<Token> tokens); ---------------------------------------------------------------------
		{	String tag_tokenStream_test1("<my_tag>");
			TokenStream stream(new StringStream(tag_tokenStream_test1));

		//--- Positive: looking for '<'
			try {
				Token token = stream.mustBe(std::set<Token>{Token(TokenType::eLeftAngleBracket), Token(TokenType::eLeftBrace)});
				PASS();
				UNITTEST_EQUALS(token, Token(TokenType::eLeftAngleBracket));

			} catch ( const String& err ) {
				FAIL("Should not have thrown!");
			}

		//--- Positive: looking for eWord:"my_tag"
			try {
				Token token = stream.mustBe(std::set<Token>{Token(TokenType::eWord)});
				PASS();
				UNITTEST_EQUALS(token, Token(TokenType::eWord, "my_tag"));
			} catch ( const String& err ) {
				FAIL("Should have thrown because we're looking at text!");
			}

		//--- Positive: looking for '>'
			try {
				Token token = stream.mustBe(std::set<Token>{Token(TokenType::eRightAngleBracket), Token(TokenType::eRightBrace)});
				PASS();
				UNITTEST_EQUALS(token, Token(TokenType::eRightAngleBracket));

			} catch ( const String& err ) {
				FAIL("Should not have thrown!");
			}

		//--- Negative
			try {
				Token token = stream.mustBe(std::set<Token>{Token(TokenType::eLeftAngleBracket)});
				FAIL("Should have thrown because we're now at EOT! Instead seeing: " + token.toString());

			} catch ( const String& err ) {
				PASS();
			}
		}

	//---	void TokenStream::mustBe(const Token& token); -------------------------------------------------------------------------
		{	String tag_tokenStream_test1("<t>");
			TokenStream stream(new StringStream(tag_tokenStream_test1));
			try {
				stream.mustBe(Token(TokenType::eLeftAngleBracket));
				PASS();
			} catch ( const String& err ) {
				std::cerr << err << std::endl;
				FAIL("Should not have thrown!");
			}

			try {
				stream.mustBe(Token(TokenType::eLeftAngleBracket));
				FAIL("Should have thrown because we're now looking at text!");
			} catch ( const String& err ) {
				PASS();
			}

			try {
				stream.mustBe(Token(TokenType::eWord));
				PASS();
			} catch ( const String& err ) {
				std::cerr << err << std::endl;
				String msg = String::formatString("Should have thrown because we're looking at 'text' (actually seeing %s)!", stream.current().toString().getChars());
				FAIL(msg.getChars());
			}
		}

	//---	bool TokenStream::mayBe(std::set<Token> tokens);
		{	String tag_tokenStream_test1("<t>");
			TokenStream stream(new StringStream(tag_tokenStream_test1));
			UNITTEST_EQUALS(stream.mayBe(std::set<Token>{Token(TokenType::eLeftAngleBracket)}), Token(TokenType::eLeftAngleBracket));
			UNITTEST_EQUALS(stream.mayBe(std::set<Token>{Token(TokenType::eWord)}), Token(TokenType::eWord, "t"));
			UNITTEST_EQUALS(stream.mayBe(std::set<Token>{Token(TokenType::eRightAngleBracket)}), Token(TokenType::eRightAngleBracket));
		}
		{	String tag_tokenStream_test1("<t>");
			TokenStream stream(new StringStream(tag_tokenStream_test1));
			UNITTEST_EQUALS(stream.mayBe(std::set<Token>{Token(TokenType::eLeftParen)}), Token());
		}

	//---	bool TokenStream::mayBe(const Token& token);
		{	String tag_tokenStream_test1("<t>");
			TokenStream stream(new StringStream(tag_tokenStream_test1));
			UNITTEST_EQUALS(stream.mayBe(Token(TokenType::eLeftAngleBracket)), Token(TokenType::eLeftAngleBracket));
			UNITTEST_EQUALS(stream.mayBe(Token(TokenType::eWord)), Token(TokenType::eWord, "t"));
			UNITTEST_EQUALS(stream.mayBe(Token(TokenType::eRightAngleBracket)), Token(TokenType::eRightAngleBracket));
		}
		{	String tag_tokenStream_test1("<t>");
			TokenStream stream(new StringStream(tag_tokenStream_test1));
			UNITTEST_EQUALS(stream.mayBe(Token(TokenType::eNumber)), Token());
		}

	//---	friend std::ostream& TokenStream::operator<<(std::ostream& stream, const TokenStream& tokens);
		{	std::ostringstream output;
			String text("this is a test");
			TokenStream stream(text);
			while ( !stream.isEOF() ) {
				stream.next();
				Token token = stream.current();
				output << token.whitespace << token.text;
			}
			UNITTEST_EQUALS(output.str(), text);
		}
	}

//=== class CppTokenStream ==========================================================================================================================
	void UnitTests::test_CppTokenStream(void) {
	//---	virtual Token CppTokenizer::next(void);
		{	Token token;
			CppTokenStream *cpp_stream;

			cpp_stream = new CppTokenStream(new StringStream("//\n"));
			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token(TokenType::eEOF, "", "//\n"));
			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;

			cpp_stream = new CppTokenStream(new StringStream("//this is a test\n"));
			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token(TokenType::eEOF, "", "//this is a test\n"));
			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;

			cpp_stream = new CppTokenStream(new StringStream("/**//*this is a test*/  \t\f\n "));
			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token(TokenType::eEOF, "", "/**//*this is a test*/  \t\f\n "));
			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;
		}
	//---	virtual Token  scoopWord(const String& spaces) override;
		{	Token token;
			CppTokenStream *cpp_stream;

			cpp_stream = new CppTokenStream(new StringStream("t s1 rest quest123 pest_123a __t __"));
			token = cpp_stream->current();
			UNITTEST_EQUALS(token, Token('word', "t", ""));

			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token('word', "s1", " "));

			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token('word', "rest", " "));

			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token('word', "quest123", " "));

			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token('word', "pest_123a", " "));

			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token('word', "__t", " "));

			token = cpp_stream->next();
			UNITTEST_EQUALS(token, Token('word', "__", " "));

			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;
		}
		{	CppTokenStream cpp_stream(new StringStream("a&&=b+c"));
			UNITTEST_EQUALS(cpp_stream.next(), Token('word', "a"));
			UNITTEST_EQUALS(cpp_stream.next(), Token('&&='));
			UNITTEST_EQUALS(cpp_stream.next(), Token('word', "b"));
			UNITTEST_EQUALS(cpp_stream.next(), Token('+'));
			UNITTEST_EQUALS(cpp_stream.next(), Token('word', "c"));
			UNITTEST_ASSERT(cpp_stream.next().isEOF());
		}
	//---	virtual String scoopSpace(void) override;
		{	CppTokenStream stream1(new StringStream("this test"));
			UNITTEST_EQUALS(stream1.next(), Token('word', "this"));
			UNITTEST_EQUALS(stream1.next(), Token('word', "test", " "));
			UNITTEST_ASSERT(stream1.next().isEOF());

			CppTokenStream stream2(new StringStream("this /**/test"));
			UNITTEST_EQUALS(stream2.next(), Token('word', "this"));
			UNITTEST_EQUALS(stream2.next(), Token('word', "test", " /**/"));
			UNITTEST_ASSERT(stream2.next().isEOF());

			CppTokenStream stream3(new StringStream("word//msg\ntest"));
			UNITTEST_EQUALS(stream3.next(), Token('word', "word"));
			UNITTEST_EQUALS(stream3.next(), Token('word', "test", "//msg\n"));
			UNITTEST_ASSERT(stream3.next().isEOF());
		}
		{	String cpp_text("void test_CppTokenizer(void) {\n"
							"//---	virtual Token CppTokenizer::next(void);\n"
							"	{\n"
							"\n"
							"/**/	CppTokenStream *cpp_stream = new CppTokenStream(new StringStream(\"//\\n\"));\n"
							"		Token token = cpp_stream->next();\n"
							"		float counter = 12345.7890e+432;\n"
							"\n");
			CppTokenStream stream(new StringStream(cpp_text));
			String cpp_text_tokens;
			while ( !stream.isEOF() ) {
				Token token = stream.next();
				cpp_text_tokens += (token.whitespace + token.text);
			}
			UNITTEST_EQUALS(cpp_text_tokens, cpp_text);
		}
		{	CppTokenStream *cpp_stream = new CppTokenStream(new StringStream("()[]{}\\;,?`@$#"));
			UNITTEST_EQUALS(cpp_stream->current(), Token(TokenType::eLeftParen, "("));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eRightParen, ")"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eLeftBracket, "["));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eRightBracket, "]"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eLeftBrace, "{"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eRightBrace, "}"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBackslash, "\\"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eSemicolon, ";"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eComma, ","));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eArithIf, "?"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBackQuote, "`"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eAt, "@"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eDollar, "$"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eHash, "#"));
			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;
		}
		{	CppTokenStream *cpp_stream = new CppTokenStream(new StringStream("....~=~!=!%=%^=^&&=&&-=&=&->--- "));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eElipsis, "..."));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::ePeriod, "."));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitNotAssign, "~="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitNot, "~"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eNotEquals, "!="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBoolNot, "!"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eModulusAssign, "%="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eModulus, "%"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitXorAssign, "^="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitXor, "^"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBoolAndAssign, "&&="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBoolAnd, "&&"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eMinusAssign, "-="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitAndAssign, "&="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitAnd, "&"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eDereference, "->"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eDecrement, "--"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eMinus, "-"));
			delete cpp_stream;
		}
		{	CppTokenStream *cpp_stream = new CppTokenStream(new StringStream("===+=+++||=|||=| "));
			UNITTEST_EQUALS(cpp_stream->current(), Token(TokenType::eEqualsTo, "=="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eEquals, "="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::ePlusAssign, "+="));
			UNITTEST_EQUALS(cpp_stream->current(), Token(TokenType::eSumAssign, "+="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eIncrement, "++"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eSum, "+"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBoolOrAssign, "||="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBoolOr, "||"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitOrAssign, "|="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eBitOr, "|"));
			delete cpp_stream;
		}
		{	CppTokenStream *cpp_stream = new CppTokenStream(new StringStream("<<=<<<=<>>=>>>=> "));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eShiftLeftAssign, "<<="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eShiftLeft, "<<"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eLessEquals, "<="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eLessThan, "<"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eShiftRightAssign, ">>="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eShiftRight, ">>"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eGreaterEquals, ">="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eGreaterThan, ">"));
			delete cpp_stream;
		}
		{	CppTokenStream *cpp_stream = new CppTokenStream(new StringStream("::/=*=*:/ "));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eScope, "::"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eDivideAssign, "/="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eMultiplyAssign, "*="));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eMultiply, "*"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eColon, ":"));
			UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eDivide, "/"));
			delete cpp_stream;
		}

	//---	virtual void CppTokenizer::scoopSpace(void);
	//---	virtual void CppTokenizer::scoopNumber(void);
		{	bool first = true;
			String integer_numbers =
					"0 1 2 3 4 5 6 7 8 9 01 11 21 31 41 51 61 71 81 91 1234567890 "	// <-- base₁₀
					"0x0 0x1234 0x0123456789ABCDEF 0x0123456789abcdef 0xABCDEF "	// <-- base₁₆
					"0123 0567 01234567"											// <-- base₈
					;
			std::vector<String> numbers_vector = integer_numbers.split(" ");
			CppTokenStream *cpp_stream = new CppTokenStream(new StringStream(integer_numbers));
			for ( String number : numbers_vector ) {
				UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eNumber, number, (first? "": " ")));
				first = false;
			}
			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;
		}
		{	bool first = true;
			String float_numbers =
					"9876.1 9876.1234 "

					"1e2 1E2 9876e1 9876E1 "
					"1e-2 1E-2 9876e-1 9876E-1 "
					"1e+2 1E+2 9876e+1 9876E+1 "

					"1e1111 1E1111 9876e1111 9876E1111 "
					"1e-1111 1E-1111 9876e-1111 9876E-1111 "
					"1e+1111 1E+1111 9876e+1111 9876E+1111 "

					"1.1e1 1.1E1 9876.1234e1 9876.1234E1 "
					"1.1e-1 1.1E-1 9876.1234e-1 9876.1234E-1 "
					"1.1e+1 1.1E+1 9876.1234e+1 9876.1234E+1 "

					"1.1e1111 1.1E1111 9876.1234e1111 9876.1234E1111 "
					"1.1e-1111 1.1E-1111 9876.1234e-1111 9876.1234E-1111 "
					"1.1e+1111 1.1E+1111 9876.1234e+1111 9876.1234E+1111"
					;

			std::vector<String> numbers_vector = float_numbers.split(" ");
			CppTokenStream *cpp_stream = new CppTokenStream(new StringStream(float_numbers));
			for ( String number : numbers_vector ) {
				UNITTEST_EQUALS(cpp_stream->next(), Token(TokenType::eFloat, number, (first? "": " ")));
				first = false;
			}
			UNITTEST_ASSERT(cpp_stream->next().isEOF());
			delete cpp_stream;
		}

	//---			virtual String CppTokenStream::toString(void) const override;
	//--- friend 	std::ostream& operator<<(std::ostream& stream, const XmlTokenStream& tokens);
		{	cchar *text =	"	Token TokenStream::scoopNumber(const String& spaces) {\n"
							"		TokenType type = TokenType::eEOF;\n"
							"		TextAccumulator acc;\n"
							"		if ( stream->current() != EOF ) {\n"
							"			while ( isdigit(stream->current()) ) {\n"
							"				acc += stream->current();\n"
							"				stream->next();\n"
							"			}\n"
							"			type = TokenType::eNumber;\n"
							"		}\n"
							"		current_token = Token(type, acc.toString(), spaces);\n"
							"		return current_token;\n"
							"	}\n";
			CppTokenStream stream(text);
			std::ostringstream output;
			while ( !stream.isEOF() ) {
				stream.next();
				output << stream.current().whitespace << stream.current().text;
			}
			UNITTEST_EQUALS(output.str(), text);
		}
	}

//=== class XmlTokenStream ==========================================================================================================================
	void UnitTests::test_XmlTokenStream(void) {
	//---	XmlTokenStream(Stream *stream);
	//---	XmlTokenStream(const String& text);
	//---	virtual Token XmlTokenizer::next(void) override;
		{	XmlTokenStream *xml_stream = new XmlTokenStream(new StringStream("<xml>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<xml>"));
			delete xml_stream;
		}
		{	XmlTokenStream *xml_stream = new XmlTokenStream(new StringStream("<xml><xml/></xml>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<xml>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eAtomTag, "<xml/>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</xml>"));
			delete xml_stream;
		}
		{	XmlTokenStream *xml_stream = new XmlTokenStream(new StringStream("<xml attrib=\"value\">this is a test <xml attrib='value'/></xml>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<xml attrib=\"value\">"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "this is a test "));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eAtomTag, "<xml attrib='value'/>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</xml>"));
			delete xml_stream;
		}

		static const String xml_tests(
				"<person id=\"1\">\n"
				"	<first>name</first>\n"
				"	<last>surname</last>\n"
				"	<address>\n"
				"		<street attrib=\"tests\" attrib1=\"tests1\" attrib2=\"tests2\">20 Evergreen Rd.</street>\n"
				"		<city>Vernon</city>\n"
				"		<state>Connecticut, USA</state>\n"
				"		<phone type=\"touchtone\" network=\"POTS\"/>\n"
				"	</address>\n"
				"</person>"
			);
		{	XmlTokenStream *xml_stream = new XmlTokenStream(xml_tests);
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<person id=\"1\">"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<first>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "name"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</first>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<last>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "surname"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</last>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<address>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<street attrib=\"tests\" attrib1=\"tests1\" attrib2=\"tests2\">"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "20 Evergreen Rd."));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</street>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<city>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "Vernon"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</city>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eStartTag, "<state>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "Connecticut, USA"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</state>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eAtomTag, "<phone type=\"touchtone\" network=\"POTS\"/>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n\t"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</address>"));
			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eText, "\n"));

			UNITTEST_EQUALS(xml_stream->next(), Token(TokenType::eEndTag, "</person>"));
			delete xml_stream;
		}

	//---	virtual String XmlTokenStream::serialize(void) const override;
		{
			UNITTEST_EQUALS(XmlTokenStream(xml_tests).serialize(),
					"{ "
						"current_token={'NULL': \"\", \"\"}\","
						"stream=\"[position=0, " + xml_tests + "\" }");
		}
	//---	virtual String XmlTokenStream::toString(void) const override;
		{
			UNITTEST_PATTERN(XmlTokenStream(xml_tests).toString(), "[[]XmlTokenStream: [0-9A-Fa-f][0-9A-Fa-f]*[]]");
		}
	//--- friend 	std::ostream& operator<<(std::ostream& stream, const XmlTokenStream& tokens);
		{
		}
	}

//=== class Xml =====================================================================================================================================
	/* NOTES:
	 * 		1) XML allows single or double quote for attributes.
	 * 		2) To test the attributes, they MUST be in alphabetical order because of the way std::map<> stores them. It's tempting to rewrite it
	 * 			for this reason.
	 */
	void UnitTests::test_Xml(void) {
	//---	Xml::Xml(TagTokenStream& stream)
	//---	virtual String Xml::toString(void) const override;
		{
			try {
				cchar *str = "<tag/>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), "<tag/>")
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str = "<tag attribute1=\"text1\"/>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), str);
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str = "<tag attribute1=\"text2\" attribute2=\"text2\" attribute3=\"text3\" attribute4=\"text4\"/>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), str);
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str = "<tag></tag>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), "<tag/>")
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str =
					"<tag_a>"
						"this is a "
						"<tag_b>amaricious, "
							"<tag>duplicitous</tag> "
						"</tag_b>"
						"test"
					"</tag_a>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), str);
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str = "<tag attribute1=\"text2\" attribute2=\"text2\" attribute3=\"text3\" attribute4=\"text4\"></tag>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), "<tag attribute1=\"text2\" attribute2=\"text2\" attribute3=\"text3\" attribute4=\"text4\"/>");
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str =
					"<Photo orientation='portrait' type='JPEG' uid='1234-12345678901234567890'>"
						"<filename>20240102-2345.jpeg</filename>"
						"<caption>Mary and Chris on beach <i>with a <u>concealed carry</u></i> rabbit</caption>"
					"</Photo>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), str);
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}

			try {
				cchar *str =
					"<CallList base_index=\"1\" index=\"1\" max=\"201\" timestamp=\"05/07/2010 21:36:12.0000\" type=\"Address\">"
						"<Item id=\"9843\" uuid=\"1234567890123-1234-0000-000000000000\">"
							"<Name>"
								"<First timestamp='05/07/2010 21:36:12.0000'>George</First>"
								"<Last>Lewis</Last>"
							"</Name>"
							"<Properties>"
								"<Property dataType=\"Bool\" key=\"fav\">true</Property>"
								"<Property dataType=\"String\" key=\"language\">Finnish</Property>"
							"</Properties>"
							"<PhoneNumberList>"
								"<PhoneNumber type=\"Home\">8015551212</PhoneNumber> uuid=\"1234567890123-1234-0001-000000000000\""
								"<PhoneNumber type=\"Cell\">2035551212</PhoneNumber> uuid=\"1234567890123-1234-0001-000000000001\""
							"</PhoneNumberList>"
						"</Item>"
					"</CallList>";
				Xml* xml = Xml::parse(new XmlTokenStream(str));
				UNITTEST_EQUALS(xml->toString(), str);
				delete xml;

			} catch ( const String& error ) {
				FAIL("Should not have got this exception: \"" + error + "\"");
			}
		}

	//---	Xml::Xml(const String& name)
		{
			Xml xml("namesless");
			UNITTEST_EQUALS(xml.name, "namesless");
			UNITTEST_EQUALS(xml.elements.size(), 0u);
			UNITTEST_EQUALS(xml.attributes.size(), 0u);
		}

	//---	Xml::Xml(XmlTokenStream xml_stream)
//		{
//			Xml xml(XmlTokenStream(xml_tests));
//			xml.
//		}

	//---	std::vector<Xml*> Xml::operator[](const String& name)
		{
	//TODO
		}

	//---	String Xml::operator()(const String& key)
		{	Xml xml("person");
			xml("height") = "163cm";
			UNITTEST_EQUALS(xml("height"), "163cm");
			xml("height") = "60\"";
			UNITTEST_EQUALS(xml("height"), "60\"");
		}

	//---	Xml* Xml::collectAttributes(const Token& token, Xml* xml = nullptr);
		{
	//TODO
		}

	//---	Xml* Xml::parse(XmlTokenStream xml_stream);
		{
	//TODO
		}

	//---	virtual const char* Xml::getChars(void)
		{
	//TODO
		}
	//---	std::vector<Xml*> operator[](const String& name);
		{

		}

	//---
		{
	//TODO
	//		std::ostringstream output;
	//		String text("this is a test");
	//		TokenStream stream(text);
	//		while ( !stream.isEOF() ) {
	//			stream.next();
	//			output << stream.current();
	//		}
	//		UNITTEST_EQUALS(output.str(), text);
		}
	}

//=== Main ==========================================================================================================================================
	void UnitTests::main(void) {
		void (*test_fns[])(void) = {
				test_TextAccumulator,
				test_String,
				test_Token,
				test_StringStream,
				test_TokenStream,
				test_CppTokenStream,
				test_XmlTokenStream,
				test_Xml,
				nullptr
		};

		for ( void (*test_fn)(void) : test_fns ) {
			if ( test_fn != nullptr ) {
				try {
					test_fn();
				} catch ( const String& msg ) {
					std::cerr << "\x1B[31m" << msg << "\x1B[0m";
				}
			} else {
				break;
			}
		}

		(tests_that_failed > 0? std::cerr: std::cout) << "DONE! " << tests_that_failed << "/" << test_number << " tests failed." << std::endl;
	}
/*
int fn(void) {
	if ( arg ) {
		command();
	} else {
		command();
	}

	if ( arg )
		command();
	else
		command();

	for ( int i = 0; i < 10; i++ ) {
		command();
	}

	for ( int i : is )
		command();

	while ( test ) {
		command();
	}
	while ( test )
		command();

	do {
		command();
	} while ( test );
	do
		command();
	while (test);

	switch (c) {
		case arg:
		default:
			break;
	}
		return arg;
}
void *fn(int a, int b, char *c[]) {}
char[] klass::fn(set::assortment& ac) {}
templat<junk>
*/

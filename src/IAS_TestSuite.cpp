#include <regex.h>
#include <iostream>
#include "AsciiVT200.hpp"
#include "IAS_TestSuite.hpp"
unsigned int test_number = 0;
unsigned int tests_that_passed = 0;
unsigned int tests_that_failed = 0;

static String autoToString(auto var) {
	std::ostringstream output;
	output << var;
	return output.str();
}

	void IAS_pass(const char *src_filename, const char *method, uint lineno) {
		test_number++;
		std::cout << std::flush
			<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: PASSED."
			<< std::endl
			<< std::flush;
	}
	void IAS_fail(const char *src_filename, const char *method, uint lineno, const String& msg) {
		tests_that_failed++;
		std::cerr << std::flush
			<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
			<< AsciiVT200::redForeground
			<< " FAILED -- " << msg
			<< AsciiVT200::resetTerminal
			<< std::endl
			<< std::flush;
	}

	void IAS_unittest_pattern(const char *src_filename, const char *method, uint lineno, const char *str_output_to_test, const char *str_expected, const String& output_to_test, const char* regex_text) {
		test_number++;

		regex_t regex_exec;
		regmatch_t pmatch[1];

		if ( regcomp(&regex_exec, regex_text, REG_NEWLINE) == 0 ) {

			if ( regexec(&regex_exec, output_to_test.getChars(), 1, pmatch, 0) == 0 ) {
				tests_that_passed++;
				std::cout << std::flush
					<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
//					<< "expression: " << (strchr(output_to_test.escape_ize().getChars(), '\n')? '\n': ' ') << output_to_test << "... PASSED.";
					<< "expression: " << output_to_test.escape_ize() << "... PASSED.";
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
					<< AsciiVT200::redForeground << (strchr(regex_text, '\n')? "\n": "") << regex_text << AsciiVT200::resetTerminal
					<< "] but got: ["
					<< AsciiVT200::redForeground << (strchr(output_to_test.escape_ize().getChars(), '\n')? "\n": "") << output_to_test << AsciiVT200::resetTerminal
					<< "]";
				std::cout
					<< std::endl
					<< std::flush;
			}

		} else {
			//TODO
		}
	}

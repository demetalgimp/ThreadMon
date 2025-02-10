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
	void IAS_unittest_assert(const char *src_filename, const char *method, uint lineno, const char *str_to_test, const auto to_test) {
		test_number++;

		if ( to_test ) {
			tests_that_passed++;
			std::cout << std::flush
				<< "Test #" << test_number << " ["	<< src_filename << ":" << method << ":" << lineno << "]: "
	//			<< "expression: " << (strchr(to_test, '\n')? '\n': ' ') << to_test << "... PASSED."
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

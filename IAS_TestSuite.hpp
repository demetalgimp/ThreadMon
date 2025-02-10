#ifndef __IAS_TESTSUITE__
#define __IAS_TESTSUITE__
#include "String.hpp"

	extern unsigned int test_number;
	extern unsigned int tests_that_passed;
	extern unsigned int tests_that_failed;

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

	#define PROTECT(...) \
		{\
			pid_t pid; \
			if ( !(pid = fork()) ) { \
				__VA_ARGS__; \
			} else { \
				int result; \
				waitpid(pid, &result, 0); \
			} \
		}

void IAS_pass(const char *src_filename, const char *method, uint lineno);
void IAS_fail(const char *src_filename, const char *method, uint lineno, const String& msg);
void IAS_unittest_assert(const char *src_filename, const char *method, uint lineno, const char *str_to_test, const auto to_test);
void IAS_unittest_equals(const char *src_filename, const char *method, uint lineno, const char *str_output_to_test, const char *str_expected, const auto output_to_test, const auto expected);

#endif

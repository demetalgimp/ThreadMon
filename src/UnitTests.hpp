//editor: tab=4
/*
 * UnitTests.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef UNITTESTS_HPP_
#define UNITTESTS_HPP_

class UnitTests {
	typedef const char cchar;
	typedef unsigned int uint;

	private:
		static void test_TextAccumulator(void);
		static void test_String(void);
		static void test_Token(void);
		static void test_StringStream(void);
		static void test_TokenStream(void);
		static void test_CppTokenStream(void);
		static void test_XmlTokenStream(void);
		static void test_XmlTagTokenStream(void);
		static void test_Xml(void);

	public:
		static void main(void);
};

#endif /* UNITTESTS_HPP_ */

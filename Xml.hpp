//editor: tab=4
/*
 * Xml.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef XML_HPP_
#define XML_HPP_

#include <map>
#include <set>
#include <vector>
#include "String.hpp"
#include "TokenStream.hpp"
#include "XmlTokenStream.hpp"

	class Xml: public Klass {
		friend class UnitTests;

		String name;
		std::vector<Klass*> elements;
		std::map<String, String> attributes;

		public:
			Xml(void) {}
			Xml(const String& name): name(name) {}
			virtual ~Xml(void) {
				for ( Klass *element : elements ) {
					delete element;
				}
			}

		public:
			static void collect_tag_pieces(const Token& token, const String& tag_text, Xml *xml);
			static Xml* parse(XmlTokenStream *xml_stream);
			std::vector<Xml*> operator[](const String& name);
			String operator()(const String& key);

		public:
	//		Xml* collectAttributes(const Token& token, Xml* xml = nullptr);
	//		Xml* parseTag(XmlTokenStream xml_stream);

			virtual const char* getChars(void) const {
				return "Xml tree";
			}
			virtual String toString(void) const override;
	};

#endif /* XML_HPP_ */


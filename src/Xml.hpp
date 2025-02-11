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
		bool builder = false;

		public:
/*tested*/	Xml(void): builder(true) {}
/*tested*/	Xml(const String& name): name(name), builder(true) {}
/*tested*/	virtual ~Xml(void) {
				for ( auto *element : elements ) {
					delete element;
				}
			}

		private:
/*??*/		std::vector<Xml*>* findPath(const std::vector<String>& name_path) const;
/*??*/		static void collect_tag_pieces(const Token& token, Xml *xml);

		public:
/*tested*/	static Xml* parse(XmlTokenStream *xml_stream);
/*??*/		std::vector<Xml*>* operator[](const String& name);
/*tested*/	String& operator()(const String& key);

		public: // Overrides
/*??*/		virtual Klass *clone(void) const override		{ return new Xml(*this); }
/*tested*/	virtual String toString(void) const override;
/*??*/		virtual String serialize(void) const override 	{ return ""; }
	};

#endif /* XML_HPP_ */


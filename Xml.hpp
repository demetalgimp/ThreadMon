//editor: tab=4
/*
 * Xml.hpp
 *
 *  Created on: Nov 19, 2024
 *      Author: swalton
 */

#ifndef XML_HPP_
#define XML_HPP_

#include <map>
#include <set>
#include <vector>
#include "String.hpp"
#include "TokenStream.hpp"

	class Xml: public Klass {
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
			static void collect_tag_pieces(const Token& token, const String& tag_text, Xml *xml) {
				TagTokenStream *tag_stream = new TagTokenStream(tag_text);
				tag_stream->mustBe(TokenType::eLeftAngleBracket);
				tag_stream->mustBe(TokenType::eWord);
				xml->name = tag_stream->current().text;
				while ( tag_stream->mayBe(TokenType::eWord) ) {
					String attrib_name = tag_stream->current().text;
					tag_stream->mustBe(TokenType::eEquals);
					tag_stream->mustBe(TokenType::eString);
					xml->attributes[attrib_name] = tag_stream->current().text;
				}
				if ( token.type == '<t>' ) {
					tag_stream->mustBe(TokenType::eRightAngleBracket);
				} else {
					tag_stream->mustBe(TokenType::eSlash);
					tag_stream->mustBe(TokenType::eRightAngleBracket);
				}
				delete tag_stream;
			}
			static Xml* parse(XmlTokenStream *xml_stream) {
				Xml *xml = new Xml();
				Token token = xml_stream->current();
				if ( token.type == '<t>' ) {
					collect_tag_pieces(token.type, token.text, xml);

					while ( xml_stream->next().type != '</t>' ) {
						if ( xml_stream->current().type == 'text' ) {
							xml->elements.push_back(new String(xml_stream->current().text));

						} else if ( xml_stream->current().type == '<t>'  ||  xml_stream->current().type == '<t/>') {
							xml->elements.push_back(parse(xml_stream));
						}
					}

				} else if ( token.type == '<t/>' ) {
					collect_tag_pieces(token.type, token.text, xml);

				} else if ( token.type == '</t>' ) {
					TagTokenStream *stream = new TagTokenStream(token.text);
					stream->mustBe(TokenType::eLeftAngleBracket);
					stream->mustBe(TokenType::eSlash);
					stream->mustBe(TokenType::eWord);
					if ( stream->current().text != xml->name ) {
						throw String::formatString("Error[%s,%d]: [line#]: malformed XML: expecting name '%s' but instead got '%s'", __FILE__, __LINE__, xml->name.getChars(), stream->current().text.getChars());
					}
					delete stream;
				}
				return xml;
			}
			std::vector<Xml*> operator[](const String& name) {
				std::vector<Xml*> XMLs;
				for ( Klass *element: elements ) {
					Xml *xml = nullptr;
					if ( (xml = dynamic_cast<Xml*>(element)) != nullptr ) {
						if ( xml->name == name ) {
							XMLs.push_back(xml);
						}
					}
				}
				return XMLs;
			}
			String operator()(const String& key) {
				return attributes[key];
			}

		public:
	//		Xml* collectAttributes(const Token& token, Xml* xml = nullptr);
	//		Xml* parseTag(XmlTokenStream xml_stream);

			virtual const char* getChars(void) const {
				return "Xml tree";
			}
			virtual String toString(void) const override;
	};

#endif /* XML_HPP_ */


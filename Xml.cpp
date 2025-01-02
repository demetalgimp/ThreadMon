//editor: tab=4
/*
 * Xml.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#include "Xml.hpp"

	void Xml::collect_tag_pieces(const Token& token, const String& tag_text, Xml *xml) {
////		XmlTagTokenStream *tag_stream = new XmlTagTokenStream(tag_text);
//		TokenStream stream(tag_text);
//		stream.mustBe(TokenType::eLeftAngleBracket);
//		stream.mustBe(TokenType::eWord);
//		xml->name = stream.current().text;
//		while ( stream.mayBe(TokenType::eWord) ) {
//			String attrib_name = stream.current().text;
//			stream.mustBe(TokenType::eEquals);
//			stream.mustBe(TokenType::eString);
//			xml->attributes[attrib_name] = stream.current().text;
//		}
//		if ( token.type == '<t>' ) {
//			stream.mustBe(TokenType::eRightAngleBracket);
//		} else {
//			stream.mustBe(TokenType::eSlash);
//			stream.mustBe(TokenType::eRightAngleBracket);
//		}
	}

	Xml* Xml::parse(XmlTokenStream *xml_stream) {
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
//			XmlTagTokenStream *stream = new XmlTagTokenStream(token.text);
//			stream->mustBe(TokenType::eLeftAngleBracket);
//			stream->mustBe(TokenType::eSlash);
//			stream->mustBe(TokenType::eWord);
//			if ( stream->current().text != xml->name ) {
//				throw String::formatString("Error[%s,%d]: [line#]: malformed XML: expecting name '%s' but instead got '%s'", __FILE__, __LINE__, xml->name.getChars(), stream->current().text.getChars());
//			}
			TokenStream stream(token.text);
			stream.mustBe(TokenType::eLeftAngleBracket);
			stream.mustBe(TokenType::eSlash);


//			if ( stream.next() != TokenType::eLeftAngleBracket )
		}
		return xml;
	}

	std::vector<Xml*> Xml::operator[](const String& name) {
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

	String Xml::operator()(const String& key) {
		return attributes[key];
	}


	String Xml::toString(void) const {
		String collector;
		collector += "<" + name;
		for ( std::pair<String, String> attrib : attributes ) {
			collector += " " + attrib.first + "=" + attrib.second;
		}

	//--- An "atom" is a tag that has no subelements.
		if ( elements.size() == 0 ) {
			collector += "/>";
		} else {
			for ( Klass *element : elements ) {
				collector += element->toString();
			}
			collector += "</" + name + ">";
		}
		return collector;
	}

//editor: tab=4
/*
 * Xml.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#include "Xml.hpp"

/*
 * void Xml::collect_tag_pieces(const Token& token, Xml *xml)
 *
 * General algorithm: List walker within a list
 */
	void Xml::collect_tag_pieces(const Token& token, Xml *xml) {
		TokenStream stream(token.text);
		stream.mustBe('<');
		xml->name = stream.mustBe('word').text;

		Token attrib;
		while ( (attrib = stream.mayBe(TokenType::eWord)).type == 'word' ) {
			String attrib_name = attrib.text;
			Token t = stream.mustBe(TokenType::eEquals);
			t = stream.mustBe(TokenType::eString);
			if ( xml->attributes.find(attrib_name) != xml->attributes.end() ) {
				std::cerr << "Warning: overwriting attribute '" <<  attrib_name << "'!" << std::endl;
			}
			xml->attributes[attrib_name] = t.text;
		}

		if ( token == TokenType::eStartTag ) {
			stream.mustBe(TokenType::eRightAngleBracket);

		} else {
			stream.mustBe(TokenType::eSlash);
			stream.mustBe(TokenType::eRightAngleBracket);
		}
	}

/*
 * Xml* Xml::parse(XmlTokenStream *xml_stream)
 *
 * General algorithm: Recursive descent
 */
	Xml* Xml::parse(XmlTokenStream *xml_stream) {
		Xml *xml = new Xml();
		Token token = xml_stream->current();

	//--- This is the "home" position, or the name of the tag. As long there exists another tag, recurse.
		if ( token.type == TokenType::eStartTag ) {
			collect_tag_pieces(token, xml);

		//--- For each new tag, ...
			while ( xml_stream->next().type != TokenType::eEOF ) {

			//--- If it is plain text, store and proceed.
				if ( xml_stream->current().type == TokenType::eText ) {
					xml->elements.push_back(new String(xml_stream->current().text));

			//--- If it is a tag (start or atom), recurse.
				} else if ( xml_stream->current().type == TokenType::eStartTag  ||  xml_stream->current().type == TokenType::eAtomTag) {
					xml->elements.push_back(parse(xml_stream));

			//--- If end tag, exit loop and end recursion.
				} else if ( xml_stream->current().type == TokenType::eEndTag ) {
					break;

			//--- We should never get here...!
				} else {
					THROW_ERROR("Error[%s:%d]: Unreachable code. Current stream token: %s", __FILE__, __LINE__, xml_stream->current().toString().getChars());
				}
			}

	//--- Atom tag has no subelements, end recursion.
		} else if ( token.type == TokenType::eAtomTag ) {
			collect_tag_pieces(token, xml);

	//--- End tag, end recursion.
		} else if ( token.type == TokenType::eEndTag ) {
			TokenStream stream(token.text);
			stream.mustBe(TokenType::eLeftAngleBracket);
			stream.mustBe(TokenType::eSlash);

		} else {
			std::cerr << "Error:unreachable code [" << __FILE__<< ":" << __LINE__ << "] EndTag " << xml_stream->current() << std::endl;
		}
		return xml;
	}

/*
 * std::vector<Xml*>* Xml::findPath(const std::vector<String>& name_path) const;
 *
 * General Algorithm: List walker within a list of lists
 */
	std::vector<Xml*>* Xml::findPath(const std::vector<String>& name_path) const {

	//--- Collect all of the Xml elements (skipping Text elements)
		std::vector<Xml*>* collection = new std::vector<Xml*>();
		for ( Klass *element : elements ) {
			if ( dynamic_cast<Xml*>(element) ) {
				collection->push_back(dynamic_cast<Xml*>(element));
			}
		}

	//--- For each name in name_path, find matches in Xml* vector.
		for ( uint index = 0; index < name_path.size()  &&  collection->size() > 0; index++ ) {

		//--- For all Xml* in collection, collect matching names in path
			std::vector<Xml*>* new_collection = new std::vector<Xml*>();
			for ( Xml *xml : *collection ) {
				if ( name_path[index] == xml->name ) {
					new_collection->push_back(xml);
				}
			}

		//--- Replace old with new.
			delete collection;
			collection = new_collection;
		}

	//--- Return results.
		return collection;
	}

/*
 * std::vector<Xml*>* Xml::operator[](const String& name_path);
 *
 * General algorithm: Straight call to findPath()
 */
	std::vector<Xml*>* Xml::operator[](const String& name_path) {
		std::vector<Xml*>* xmls = new std::vector<Xml*>();
		//...
		return xmls;
	}

/*
 * String& Xml::operator()(const String& key);
 *
 * General algorithm: Straight call indexing attribute map<>
 */
	String& Xml::operator()(const String& key) {
		return attributes[key];
	}

/*
 * String Xml::toString(void) const;
 *
 * General algorithm: Attribute list walker with recursive descent.
 */
	String Xml::toString(void) const {
		String collector = "<" + name;

	//--- Collect all attributes.
		for ( std::pair<String, String> attrib : attributes ) {
			collector += " " + attrib.first + "=" + attrib.second;
		}

	//--- Walk through each subelement, collecting their toString()'s
		if ( elements.size() == 0 ) { //--- An "atom" is a tag that has no subelements.
			collector += "/>";

		} else {
			collector += ">";
			for ( Klass *element : elements ) {
				collector += element->toString();
			}
			collector += "</" + name + ">";
		}

	//--- Return result.
		return collector;
	}

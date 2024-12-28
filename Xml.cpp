//editor: tab=4
/*
 * Xml.cpp
 *
 *  Created on: Nov 19, 2024
 *      Author: swalton
 */

#include "Xml.hpp"

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

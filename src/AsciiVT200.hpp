//editor: tab=4
/*
 * AsciiVT200.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: swalton
 */

#ifndef ASCIIVT200_HPP_
#define ASCIIVT200_HPP_

struct AsciiVT200 {
	static constexpr const char *resetTerminal = "\x1B[0m";

//--- Foreground presets
	static constexpr const char *blackForeground = "\x1B[30m";
	static constexpr const char *redForeground = "\x1B[31m";
	static constexpr const char *greenForeground = "\x1B[32m";
	static constexpr const char *yellowForeground = "\x1B[93m";
	static constexpr const char *blueForeground = "\x1B[34m";
	static constexpr const char *magentaForeground = "\x1B[35m";
	static constexpr const char *cyanForeground = "\x1B[36m";
	static constexpr const char *whiteForeground = "\x1B[37m";
};

#endif /* ASCIIVT200_HPP_ */

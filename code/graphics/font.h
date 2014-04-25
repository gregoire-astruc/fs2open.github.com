/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell
 * or otherwise commercially exploit the source or things you created based on the
 * source.
 *
*/



#ifndef _FONT_H
#define _FONT_H

#include "globalincs/pstypes.h"

#include "graphics/font/enums.h"
#include "graphics/font/FontManager.h"
#include "graphics/font/FSFont.h"
#include "graphics/font/FTGLFont.h"
#include "graphics/font/VFNTFont.h"

#include <FTGL/ftgl.h>

namespace font
{
	/**
	* @brief Parses the first font name
	*
	* Parses the first font name in the font table and stuffs it into @c firstFont
	*
	* @param firstFont The SCP_string which should contain the name
	*/
	void stuff_first(SCP_string &firstFont);

	/**
	* Crops a string if required to force it to not exceed max_width pixels when printed.
	* Does this by dropping characters at the end of the string and adding '...' to the end.
	*
	* @param str		string to crop.  Modifies this string directly
	* @param max_str	max characters allowed in str
	* @param max_width number of pixels to limit string to (less than or equal to).
	* @return			Returns same pointer passed in for str.
	*/
	char *force_fit_string(char *str, int max_str, int max_width);

	/**
	* @brief Inites the font system
	*
	* Initializes the font system by setting up the FontManager, parse the font table(s) and
	* set the current font id to 0.
	*/
	void init();

	/**
	* @brief Closes the Font system
	*
	* Deallocates all allocated memory for the fonts and the respective font data.
	*/
	void close();

	/**
	* For a string @c s returns a x-coordinate which will center the first line of the string
	* on the screen
	*
	* @param s
	* @return
	*/
	int get_centered_x(const char *s);

	/**
	* Retrieves the font which is located at index @c font_num and sets this font
	* as the current font
	* @param font_num The new font number, may not be an illegal font number
	*/
	inline void set_font(int fontnum)
	{
		FontManager::setCurrentFont(FontManager::getFont(fontnum));
	}

	/**
	* @brief Parses a font number
	*
	* Parses a font using either the font name or the font index and stuffs the index into @c destination .
	* If the font could not be parsed because of a syntax error, -1 is stuffed instead
	*
	* @param destination The variable that should hold the font
	* @param tag The font that should precede the font name or number. Defaults to "Font:"
	* @return true when a font has been parsed, even when this font was invalid. False when the tag wasn't found
	*/
	bool parse_font(int &destination, const char *tag = "Font:");

	/**
	* @brief The currently active font index
	* @return The font index or -1 when the FontManager hasnt't been initialized yet
	*/
	inline int get_current_fontnum()
	{
		return FontManager::getCurrentFontIndex();
	}

	/**
	* @brief The current font object
	*
	* @return The current font object or NULL when not yet ready
	*/
	FSFont *get_current_font();

	/**
	* @brief Retrieves a font by index
	*
	* Gets a font by index into the internal font vector
	*
	* @param fontNum The index which should be returned
	* @return A font pointer or NULL of the index is not valid
	*/
	inline FSFont *get_font(int fontNum)
	{
		return FontManager::getFont(fontNum);
	}

	/**
	* @brief Retrieves a font by name
	*
	* @param name The name which should be searched
	* @return The font pointer or NULL if no font with that name could be found
	*/
	FSFont *get_font(const SCP_string& name);
}

#endif

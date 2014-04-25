
#ifndef FONT_ENUMS_H
#define FONT_ENUMS_H
#pragma once

namespace font
{
	/**
	* @brief Enum to specify the type of a font
	*
	* Specifies the type of a font object and can be used to implement font specific behavior
	*/
	enum FontType
	{
		VFNT_FONT,		//!< A normal FreeSpace font
		FTGL_FONT,		//!< A TrueType font as used by the FreeType library
		UNKNOWN_FONT,	//!< An unknown font type. Probably means that an error happened
	};

	/**
	* @brief The type of a FTGL TrueType font
	*/
	enum FTGLFontType
	{
		OUTLINE,	//!< A font where only the outlines of the characters are visible
		TEXTURE,	//!< A font where each character gets saved as a texture. Gives the best visual results but changing size is expensive
	};

	const int FONT1 = 0;	//<! The first loaded font. Used to be hardcoded to reference font01.vf in retail
	const int FONT2 = 1;	//<! The second loaded font. Used to be hardcoded to reference font02.vf in retail
	const int FONT3 = 2;	//<! The third loaded font. Used to be hardcoded to reference font03.vf in retail

}

#endif // FONT_ENUMS_H

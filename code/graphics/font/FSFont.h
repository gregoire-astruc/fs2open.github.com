
#ifndef FONT_FSFONT_H
#define FONT_FSFONT_H
#pragma once

#include "globalincs/pstypes.h"

#include "graphics/font/enums.h"

namespace font
{
	/**
	* @brief	Abstract font class.
	*
	* An abstract class which is the superclass of every font class
	*
	* @author	m!m
	* @date	23.11.2011
	*/
	class FSFont
	{
	private:
		SCP_string name;	//!< The name of this font
	protected:
		int offsetTop;		//!< The offset at the top of a line of text
		int offsetBottom;	//!< The offset at the bottom of a line of text

	public:

		/**
		* @brief	Default constructor.
		*
		* @date	23.11.2011
		*/
		FSFont();

		/**
		* @brief	Destructor.
		*
		* @date	23.11.2011
		*/
		virtual ~FSFont();

		/**
		* @brief	Sets the name of this font.
		*
		* @date	23.11.2011
		*
		* @param	name	The new name.
		*/
		void setName(const SCP_string& name);

		/**
		* @brief	Gets the name of this font.
		*
		* @date	23.11.2011
		*
		* @return	The name.
		*/
		const SCP_string& getName() const;

		/**
		* @brief	Gets the type of this font.
		*
		* The return value depends on the implementing subclass.
		*
		* @date	23.11.2011
		*
		* @return	The type.
		*
		* @see FontType::VFNT_FONT
		* @see FontType::FTGL_FONT
		*/
		virtual FontType getType() const = 0;

		/**
		* @brief	Gets the height of this font in pixels with regard to font top and bottom offsets.
		*
		* @date	23.11.2011
		*
		* @return	The height.
		*/
		virtual int getHeight() const;

		/**
		* @brief	Gets the height of this font in pixels without the top and bottom offsets.
		*
		* @date	29.1.2012
		*
		* @return	The height.
		*/
		virtual int getTextHeight() const = 0;

		/**
		* @brief	Gets a string size.
		* Computes the size of the given string when it would be drawn by this font
		*
		* @date	23.11.2011
		*
		* @param [in]	text  	If non-null, the text.
		* @param textLen		Length of the text. Use -1 if the string should be checked until the next \0 character.
		* @param [out]	width 	If non-null, the width.
		* @param [out]	height	If non-null, the height.
		*/
		virtual void getStringSize(const char *text, int textLen = -1, int *width = NULL, int *height = NULL) const = 0;

		/**
		* @brief	Gets the offset of this font from the top of the drawing line
		*
		* @date	27.11.2012
		*
		* @return	The top offset.
		*/
		int getTopOffset() const;

		/**
		* @brief	Gets the offset of this font from the bottom of the end of the text to where the next line will start.
		*
		* @date	27.1.2012
		*
		* @return	The bottom offset.
		*/
		int getBottomOffset() const;


		/**
		* @brief	Sets the top offset for this font
		*
		* @date	27.1.2012
		*
		* @param	newOffset The new top offset for this font
		*/
		void setTopOffset(int newOffset);


		/**
		* @brief	Sets the bottom offset for this font
		*
		* @date	27.1.2012
		*
		* @param	newOffset The new bottom offset for this font
		*/
		void setBottomOffset(int newOffset);
	};
}

#endif // FONT_FSFONT_H


#ifndef FONT_FTGLFONT_H
#define FONT_FTGLFONT_H
#pragma once

#include "globalincs/pstypes.h"

#include "graphics/font/FSFont.h"

class FTFont;

namespace font
{
	/**
	* @brief A TrueType font
	*
	* Class that contains a TrueType font in form of a FTGL FTFont object
	*/
	class FTGLFont : public FSFont
	{
	private:
		FTFont *ftFont; //<! The font pointer of the actual FTGL font

		float yOffset;			//<! An offset to be applied to the y coordinate to compensate for mirroring the text
		FTGLFontType fontType;	//<! The type of font this is
		float lineWidth;		//<! The width for the lines when the type of FTGLFontType::Outline
		float tabWidth;			//<! The width of one tab (\t) character

		const char* separators;

	public:
		/**
		* @brief Constructor
		*
		* Constructs this object with the specified FTGl font and type
		*
		* @param ftglFont The FTGL font pointer
		* @param type The type this font has
		*/
		FTGLFont(FTFont *ftFont, FTGLFontType type);

		/**
		* @brief Deallocates the FTGL font pointer
		*/
		~FTGLFont();

		// Getters

		/**
		* @brief Gets the FTGL font pointer
		* @return The font pointer
		*/
		FTFont *getFTFont() { return ftFont; }

		/**
		* Gets the Y offset to be applied when rendering text
		* @return The Y offset
		*/
		float getYOffset() const;


		/**
		* @brief Gets the type of this font. Always returns FontType::FTGL_FONT
		* @return FontType::FTGL_FONT
		*/
		virtual FontType getType() const;

		/**
		* @brief The text height of this font
		*
		* Gets the actual text height of this font without applying top and bottom offsets
		*
		* @return The height in pixels
		*/
		virtual int getTextHeight() const;

		/**
		* @brief Gets the width of the string
		*
		* Gets the width of the specified string also accounting for multiple lines of text.
		*
		* @param text
		* @param length
		* @return
		*/
		virtual int getStringWidth(const char *text, int length = -1) const;

		/**
		* @brief Gets the size of a text
		*
		* Gets the size of the specified text and stuffs the results in @c width and @c height.
		*
		* @param text The text to be used
		* @param textLen The length which should be checked. Specify -1 when the whole string should be checked
		* @param [out] width Destination for width. May be NULL
		* @param [out] height Destination for height, may be NULL
		*/
		virtual void getStringSize(const char *text, int textLen, int *width, int *height) const;

		/**
		* @brief The FTGL font type of this font
		* @return Font type
		*/
		FTGLFontType getFontType() const;

		/**
		* @brief Gets the line width of the font
		*
		* Retrieves the line width of this font which should be used when type if FTGLFontType::Outline
		*
		* @return Line width
		*/
		float getLineWidth() const;

		/**
		* @brief Gets the width of a tab character
		*
		* Retrieves the width a single tab character has in this font
		* @return The width of a tab character
		*/
		float getTabWidth() const;

		size_t getTokenLength(const char *string, int length = -1) const;

		//Setters

		/**
		* @brief Sets line width
		*
		* Sets the line width of this font, only effective when type if FTGLFontType::Outline
		* @param width The new line width
		*/
		void setLineWidth(float width);

		/**
		* @brief Sets the width of a tab character
		* @param width New width of a tab character
		*/
		void setTabWidth(float width);
	};
}

#endif // FONT_FTGLFONT_H

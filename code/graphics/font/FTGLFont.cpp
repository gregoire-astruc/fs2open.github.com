
#include "graphics/font/FTGLFont.h"
#include "graphics/font/enums.h"

#include <FTGL/ftgl.h>

#include <utf8.h>

namespace font
{
	FTGLFont::FTGLFont(FTFont *ftFont, FTGLFontType type) : FSFont(), lineWidth(1.0f), separators("\n\t"), ftFont(ftFont)
	{
		Assertion(ftFont != NULL, "Invalid font passed to constructor of FTGLFont!");

		this->yOffset = ftFont->Ascender() + ftFont->Descender();

		setTabWidth(1.0f);
	}

	FTGLFont::~FTGLFont()
	{
		delete ftFont;
		ftFont = NULL;
	}

	FontType FTGLFont::getType() const
	{
		return FTGL_FONT;
	}

	int FTGLFont::getTextHeight() const
	{
		return fl2i(ftFont->Ascender());
	}

	float FTGLFont::getYOffset() const
	{
		return yOffset;
	}

	void FTGLFont::getStringSize(const char *text, int textLen, int *width, int *height) const
	{
		bool checkLength = textLen >= 0;

		if (textLen < 0)
		{
			textLen = strlen(text);
		}

		float w = 0.0f;
		float h = i2fl(this->getHeight());

		size_t tokenLength;

		const char *s = text;
		bool specialChar = false;
		float lineWidth = 0.0f;

		while ((tokenLength = this->getTokenLength(s, textLen)) > 0)
		{
			if (checkLength)
			{
				if (tokenLength > (size_t)textLen)
				{
					tokenLength = (size_t)textLen;
				}
			}

			if (tokenLength == 1)
			{
				// We may have encoutered a special character
				switch (*s)
				{
				case '\n':
					specialChar = true;

					h += this->getHeight();
					lineWidth = 0.0f;
					break;
				case '\t':
					specialChar = true;

					lineWidth += this->getTabWidth();
					break;
				default:
					break;
				}
			}

			if (!specialChar)
			{
				lineWidth += ftFont->Advance(s, utf8::distance(s, s + tokenLength));
			}

			w = MAX(w, lineWidth);

			specialChar = false;

			// Advance the string pointer
			s += tokenLength;

			if (checkLength)
			{
				textLen -= tokenLength;

				if (textLen <= 0)
				{
					break;
				}
			}
		}

		if (height)
			*height = fl2i(ceil(h));

		if (width)
			*width = fl2i(ceil(w));
	}

	FTGLFontType FTGLFont::getFontType() const
	{
		return fontType;
	}

	int FTGLFont::getStringWidth(const char *s, int maxLength) const
	{
		int width;

		this->getStringSize(s, maxLength, &width, NULL);

		return width;
	}

	float FTGLFont::getLineWidth() const
	{
		if (this->fontType == OUTLINE)
		{
			if (lineWidth <= 0.0f)
			{
				return 1.0f;
			}

			return lineWidth;
		}
		else
		{
			return -1.0f;
		}
	}

	float FTGLFont::getTabWidth() const
	{
		return this->tabWidth;
	}

	size_t FTGLFont::getTokenLength(const char *string, int maxLength) const
	{
		Assert(string != NULL);

		size_t length = 0;

		if (maxLength < 0)
		{
			maxLength = strlen(string);
		}

		if (maxLength <= 0)
			return 0;

		const char *nullPtr = strchr(const_cast<char*>(string), '\0');
		const char *nextToken = strpbrk(const_cast<char*>(string), this->separators);

		// WOHOO! Pointer arithmetic!!!
		if (nullPtr != NULL && (nextToken == NULL || nullPtr < nextToken))
		{
			length = nullPtr - string;
		}
		else if (nextToken != NULL)
		{
			if (nextToken == string)
			{
				length = 1;
			}
			else
			{
				length = nextToken - string;
			}
		}
		else
		{
			length = strlen(string);
		}

		if (length > (size_t)maxLength)
		{
			length = (size_t)maxLength;
		}

		return length;
	}

	void FTGLFont::setLineWidth(float width)
	{
		if (width <= 0.0f)
		{
			Error(LOCATION, "A line width of %f is not allowed! Must be greater than zero.", width);
			width = 1.0f;
		}

		this->lineWidth = width;
	}

	void FTGLFont::setTabWidth(float spaceNum)
	{
		Assert(spaceNum > 0.0f);

		this->tabWidth = this->ftFont->Advance(" ") * spaceNum;
	}
}

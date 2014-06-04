/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <sstream>

#include "graphics/font/font_internal.h"
#include "graphics/font.h"

#include "graphics/2d.h"

#include "globalincs/def_files.h"
#include "bmpman/bmpman.h"
#include "cfile/cfile.h"
#include "localization/localize.h"
#include "parse/parselo.h"

#include <utf8.h>

namespace
{
	namespace fo = font;
	using namespace font;

	bool parse_type(FontType &type, SCP_string &fileName)
	{
		int num = optional_string_either("$TrueType:", "$Font:");
		if (num == 0)
		{
			type = FTGL_FONT;
		}
		else if (num == 1)
		{
			type = VFNT_FONT;
		}
		else
		{
			type = UNKNOWN_FONT;
		}


		if (type != UNKNOWN_FONT)
		{
			stuff_string(fileName, F_NAME);
			return true;
		}
		else
		{
			return false;
		}
	}

	FTGLFontType parse_ftgl_type()
	{
		FTGLFontType type;

		SCP_string value;

		stuff_string(value, F_NAME);

		if (!stricmp(value.c_str(), "Outline"))
		{
			type = OUTLINE;
		}
		else if (!stricmp(value.c_str(), "Texture"))
		{
			type = TEXTURE;
		}
		else
		{
			Warning(LOCATION, "Illegal type value \"%s\".", value.c_str());
			type = TEXTURE;
		}

		return type;
	}

	void parse_ftgl_font(const SCP_string& fontFilename)
	{
		float size = 8.0f;
		FTGLFontType type = TEXTURE;
		SCP_string fontStr;
		bool hasName = false;

		if (optional_string("+Name:"))
		{
			stuff_string(fontStr, F_NAME);

			hasName = true;
		}

		if (optional_string("+Size:"))
		{
			stuff_float(&size);

			if (size <= 0.0f)
			{
				Warning(LOCATION, "+Size has to be bigger than 0 for font \"%s\".", fontFilename.c_str());
				size = 8;
			}
		}

		if (optional_string("+Type:"))
		{
			type = parse_ftgl_type();
		}

		// Build name from existing values if no name is specified
		if (!hasName)
		{
			SCP_stringstream ss;

			switch (type)
			{
			case OUTLINE:
				ss << "Outline-";
				break;
			case TEXTURE:
				ss << "Texture-";
				break;
			default:
				ss << "InvalidType-";
				break;
			}

			ss << fontFilename << "-";

			ss << size;

			fontStr = ss.str();
		}

		if (FontManager::getFont(fontStr) != NULL)
		{
			if (hasName)
			{
				Warning(LOCATION, "Font with name \"%s\" is already present! Font names have to be unique!", fontStr.c_str());
				return;
			}
			else
			{
				Warning(LOCATION, "Found font with same default name (\"%s\"). This is most likely a duplicate.", fontStr.c_str());
				return;
			}
		}

		FTGLFont *ftglFont = FontManager::loadFTGLFont(fontFilename, size, type);

		if (ftglFont == NULL)
		{
			Warning(LOCATION, "Couldn't load font \"%s\".", fontFilename.c_str());
			return;
		}

		if (optional_string("+Top offset:"))
		{
			int temp;

			stuff_int(&temp);

			ftglFont->setTopOffset(temp);
		}

		if (optional_string("+Bottom offset:"))
		{
			int temp;

			stuff_int(&temp);

			ftglFont->setBottomOffset(temp);
		}

		if (type == OUTLINE)
		{
			if (optional_string("+Line Width:"))
			{
				float width;
				stuff_float(&width);

				if (width <= 0.0f)
				{
					Warning(LOCATION, "Invalid value %f for line width. Must be greater than zero.", width);
				}
				else
				{
					ftglFont->setLineWidth(width);
				}
			}
		}

		if (optional_string("+Tab width:"))
		{
			float temp;
			stuff_float(&temp);

			if (temp < 0.0f)
			{
				Warning(LOCATION, "Invalid tab spacing %f. Has to be greater or equal to zero.");
			}
			else
			{
				ftglFont->setTabWidth(temp);
			}
		}

		ftglFont->setName(fontStr);
	}

	void parse_vfnt_font(const SCP_string& fontFilename)
	{
		VFNTFont *font = FontManager::loadVFNTFont(fontFilename);

		if (font == NULL)
		{
			Warning(LOCATION, "Couldn't load font\"%s\".", fontFilename.c_str());
			return;
		}

		SCP_string fontName;

		if (optional_string("+Name:"))
		{
			stuff_string(fontName, F_NAME);
		}
		else
		{
			fontName.assign(fontFilename);
		}

		font->setName(fontName);

		if (optional_string("+Top offset:"))
		{
			int temp;

			stuff_int(&temp);

			font->setTopOffset(temp);
		}

		if (optional_string("+Bottom offset:"))
		{
			int temp;

			stuff_int(&temp);

			font->setBottomOffset(temp);
		}
	}

	bool font_parse_setup(const char *fileName)
	{
		bool noTable = false;
		if (!strcmp(fileName, "fonts.tbl"))
		{
			if (!cf_exists_full(fileName, CF_TYPE_TABLES))
			{
				noTable = true;
			}
		}

		int rval;
		if ((rval = setjmp(parse_abort)) != 0)
		{
			mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n", (noTable) ? NOX("<default fonts.tbl>") : fileName, rval));
			return false;
		}

		if (noTable)
		{
			read_file_text_from_array(defaults_get_file("fonts.tbl"));
		}
		else
		{
			read_file_text(fileName, CF_TYPE_TABLES);
		}

		reset_parse();

		// start parsing
		required_string("#Fonts");

		return true;
	}

	void parse_font_tbl(const char *fileName)
	{
		if (!font_parse_setup(fileName))
			return;

		FontType type;
		SCP_string fontName;

		while (parse_type(type, fontName))
		{
			switch (type)
			{
			case VFNT_FONT:
				parse_vfnt_font(fontName);
				break;
			case FTGL_FONT:
				parse_ftgl_font(fontName);
				break;
			default:
				Warning(LOCATION, "Unknown font type %d! Get a coder!", (int)type);
				break;
			}
		}

		// done parsing
		required_string("#End");
	}

	void parse_fonts_tbl()
	{
		//Parse main TBL first
		parse_font_tbl("fonts.tbl");

		//Then other ones
		parse_modular_table("*-fnt.tbm", parse_font_tbl);

		// double check
		if (FontManager::numberOfFonts() < 3) {
			Error(LOCATION, "At least three fonts have to be loaded by only %d valid entries were found!", FontManager::numberOfFonts());
		}
	}
}

namespace font
{
	char *force_fit_string(char *str, int max_str, int max_width)
	{
		int w;

		gr_get_string_size(&w, NULL, str);
		if (w > max_width) {
			if ((int)strlen(str) > max_str - 3) {
				Assert(max_str >= 3);
				str[max_str - 3] = 0;
			}

			strcpy(str + strlen(str) - 1, "...");
			gr_get_string_size(&w, NULL, str);
			while (w > max_width) {
				Assert(strlen(str) >= 4);  // if this is hit, a bad max_width was passed in and the calling function needs fixing.
				strcpy(str + strlen(str) - 4, "...");
				gr_get_string_size(&w, NULL, str);
			}
		}

		return str;
	}

	void init()
	{
		FontManager::init();

		parse_fonts_tbl();

		set_font(0);
	}

	void close()
	{
		FontManager::close();
	}

	// NOTE: this returns an unscaled size for non-standard resolutions
	int get_centered_x(const char *s)
	{
		int w;
		int length = -1;

		char *nlPtr = strchr(const_cast<char*>(s), '\n');

		if (nlPtr != NULL)
		{
			length = nlPtr - s + 1;
		}

		gr_get_string_size(&w, NULL, s, length);

		return ((gr_screen.clip_width_unscaled - w) / 2);
	}

	void stuff_first(SCP_string &firstFont)
	{
		if (!font_parse_setup("fonts.tbl"))
		{
			Error(LOCATION, "Failed to setup font parsing. This may be caused by an empty fonts.tbl file.");
			return;
		}

		FontType type;
		parse_type(type, firstFont);
	}

	bool parse_font(int &destination, const char *tag)
	{
		if (optional_string(tag))
		{
			SCP_string input;
			stuff_string(input, F_NAME);
			SCP_stringstream ss(input);

			int fontNum;
			ss >> fontNum;

			if (ss.fail())
			{
				fontNum = FontManager::getFontIndex(input);

				if (fontNum < 0)
				{
					Warning(LOCATION, "Invalid font name \"%s\"!", input.c_str());
					destination = -1;
				}
				else
				{
					destination = fontNum;
				}
			}
			else
			{
				if (fontNum < 0 || fontNum >= FontManager::numberOfFonts())
				{
					Warning(LOCATION, "Invalid font number %d! must be greater or equal to zero and smaller than %d.", fontNum, FontManager::numberOfFonts());
					destination = -1;
				}
				else
				{
					destination = fontNum;
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	FSFont *get_current_font()
	{
		return FontManager::getCurrentFont();
	}

	FSFont *get_font(const SCP_string& name)
	{
		return FontManager::getFont(name);
	}
}

int gr_get_font_height()
{
	if (FontManager::isReady())
	{
		return FontManager::getCurrentFont()->getHeight();
	}
	else
	{
		return 16;
	}
}

void gr_get_string_size(int *w1, int *h1, const char *text, int len)
{
	if (!FontManager::isReady())
	{
		if (w1)
			*w1 = 16;

		if (h1)
			*h1 = 16;

		return;
	}

	FontManager::getCurrentFont()->getStringSize(text, len, w1, h1);
}

MONITOR(FontChars)

#ifdef _WIN32

void gr_string_win(int x, int y, char *s)
{
	using namespace font;

	int old_bitmap = gr_screen.current_bitmap;
	set_font(FONT1);
	gr_string(x, y, s);
	gr_screen.current_bitmap = old_bitmap;
}

#endif   // ifdef _WIN32

char grx_printf_text[2048];

void _cdecl gr_printf(int x, int y, char * format, ...)
{
	if (!FontManager::isReady())
		return;

	va_list args;

	va_start(args, format);
	vsprintf(grx_printf_text, format, args);
	va_end(args);

	gr_string(x, y, grx_printf_text);
}

void _cdecl gr_printf_no_resize(int x, int y, char * format, ...)
{
	va_list args;

	if (!FontManager::isReady())
		return;

	va_start(args, format);
	vsprintf(grx_printf_text, format, args);
	va_end(args);

	gr_string(x, y, grx_printf_text, false);
}

void _cdecl gr_printf(int x, int y, const char * format, ...)
{
	va_list args;

	if (!FontManager::isReady()) return;

	va_start(args, format);
	vsprintf(grx_printf_text, format, args);
	va_end(args);

	gr_string(x, y, grx_printf_text);
}

void _cdecl gr_printf_no_resize(int x, int y, const char * format, ...)
{
	va_list args;

	if (!FontManager::isReady()) return;

	va_start(args, format);
	vsprintf(grx_printf_text, format, args);
	va_end(args);

	gr_string(x, y, grx_printf_text, false);
}

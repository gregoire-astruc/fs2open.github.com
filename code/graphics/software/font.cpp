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

#include "graphics/software/font_internal.h"
#include "graphics/font.h"

#include "graphics/2d.h"

#include "globalincs/def_files.h"
#include "bmpman/bmpman.h"
#include "cfile/cfile.h"
#include "localization/localize.h"
#include "parse/parselo.h"

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
			Warning(LOCATION, "Illegal type value \"%s\".", value);
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

		if (optional_string("+Special Character Font:"))
		{
			SCP_string fontName;
			stuff_string(fontName, F_NAME);

			fo::font* fontData = FontManager::loadFontOld(const_cast<char*>(fontName.c_str()));

			if (fontData == NULL)
			{
				Warning(LOCATION, "Failed to load font \"%s\" for special characters of font \"%s\"!", fontName.c_str(), fontFilename.c_str());
			}
			else
			{
				ftglFont->setSpecialCharacterFont(fontName, fontData);
			}
		}
		else
		{
			fo::font* fontData = FontManager::loadFontOld("font01.vf");
			ftglFont->setSpecialCharacterFont(SCP_string("font01.vf"), fontData);
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
	SCP_map<SCP_string, TrueTypeFontData> FontManager::allocatedData;
	SCP_map<SCP_string, font*> FontManager::vfntFontData;
	SCP_vector<FSFont*> FontManager::fonts;

	FSFont* FontManager::currentFont = NULL;

	FSFont* FontManager::getFont(const SCP_string& name)
	{
		for (SCP_vector<FSFont*>::iterator iter = fonts.begin(); iter != fonts.end(); iter++)
		{
			if (!(*iter)->getName().compare(name))
				return (*iter);
		}

		return NULL;
	}

	FSFont *FontManager::getCurrentFont()
	{
		return currentFont;
	}

	int FontManager::getCurrentFontIndex()
	{
		if (!FontManager::isReady())
			return -1;

		return FontManager::getFontIndex(currentFont);
	}

	int FontManager::getFontIndex(const SCP_string& name)
	{
		int index = 0;

		for (SCP_vector<FSFont*>::iterator iter = fonts.begin(); iter != fonts.end(); iter++, index++)
		{
			if (!(*iter)->getName().compare(name))
				return index;
		}

		return -1;
	}

	int FontManager::getFontIndex(FSFont *font)
	{
		if (font == NULL)
			return -1;

		int index = 0;

		for (SCP_vector<FSFont*>::iterator iter = fonts.begin(); iter != fonts.end(); iter++, index++)
		{
			if ((*iter) == font)
				return index;
		}

		return -1;
	}

	int FontManager::numberOfFonts()
	{
		return (int)fonts.size();
	}

	bool FontManager::isReady()
	{
		return currentFont != NULL;
	}

	bool FontManager::isFontNumberValid(int id)
	{
		return id >= 0 && id < (int)fonts.size();
	}

	void FontManager::setCurrentFont(FSFont *font)
	{
		Assertion(font != NULL, "New font pointer may not be NULL!");
		currentFont = font;
	}

	font *FontManager::loadFontOld(char *typeface)
	{
		SCP_string typefaceString(typeface);
		if (vfntFontData.find(typefaceString) != vfntFontData.end())
		{
			font* data = vfntFontData[typefaceString];

			Assert(data != NULL);

			return data;
		}

		CFILE *fp;
		font *fnt;

		fnt = new font;
		if (!fnt)
		{
			mprintf(("Unable to allocate memory for \"%s\"", typeface));
			return NULL;
		}

		bool localize = true;

		fp = cfopen(typeface, "rb", CFILE_NORMAL, CF_TYPE_ANY, localize);
		if (fp == NULL)
		{
			mprintf(("Unable to find font file \"%s\".", typeface));
			delete fnt;
			return NULL;
		}

		strncpy(fnt->filename, typeface, MAX_FILENAME_LEN);
		cfread(&fnt->id, 4, 1, fp);
		cfread(&fnt->version, sizeof(int), 1, fp);
		cfread(&fnt->num_chars, sizeof(int), 1, fp);
		cfread(&fnt->first_ascii, sizeof(int), 1, fp);
		cfread(&fnt->w, sizeof(int), 1, fp);
		cfread(&fnt->h, sizeof(int), 1, fp);
		cfread(&fnt->num_kern_pairs, sizeof(int), 1, fp);
		cfread(&fnt->kern_data_size, sizeof(int), 1, fp);
		cfread(&fnt->char_data_size, sizeof(int), 1, fp);
		cfread(&fnt->pixel_data_size, sizeof(int), 1, fp);

		fnt->id = INTEL_SHORT(fnt->id); //-V570
		fnt->version = INTEL_INT(fnt->version); //-V570
		fnt->num_chars = INTEL_INT(fnt->num_chars); //-V570
		fnt->first_ascii = INTEL_INT(fnt->first_ascii); //-V570
		fnt->w = INTEL_INT(fnt->w); //-V570
		fnt->h = INTEL_INT(fnt->h); //-V570
		fnt->num_kern_pairs = INTEL_INT(fnt->num_kern_pairs); //-V570
		fnt->kern_data_size = INTEL_INT(fnt->kern_data_size); //-V570
		fnt->char_data_size = INTEL_INT(fnt->char_data_size); //-V570
		fnt->pixel_data_size = INTEL_INT(fnt->pixel_data_size); //-V570

		if (fnt->kern_data_size)	{
			fnt->kern_data = (font_kernpair *)vm_malloc(fnt->kern_data_size);
			Assert(fnt->kern_data != NULL);
			cfread(fnt->kern_data, fnt->kern_data_size, 1, fp);
		}
		else {
			fnt->kern_data = NULL;
		}
		if (fnt->char_data_size)	{
			fnt->char_data = (font_char *)vm_malloc(fnt->char_data_size);
			Assert(fnt->char_data != NULL);
			cfread(fnt->char_data, fnt->char_data_size, 1, fp);

			for (int i = 0; i<fnt->num_chars; i++) {
				fnt->char_data[i].spacing = INTEL_INT(fnt->char_data[i].spacing); //-V570
				fnt->char_data[i].byte_width = INTEL_INT(fnt->char_data[i].byte_width); //-V570
				fnt->char_data[i].offset = INTEL_INT(fnt->char_data[i].offset); //-V570
				fnt->char_data[i].kerning_entry = INTEL_INT(fnt->char_data[i].kerning_entry); //-V570
				fnt->char_data[i].user_data = INTEL_SHORT(fnt->char_data[i].user_data); //-V570
			}
		}
		else {
			fnt->char_data = NULL;
		}
		if (fnt->pixel_data_size)	{
			fnt->pixel_data = (ubyte *)vm_malloc(fnt->pixel_data_size);
			Assert(fnt->pixel_data != NULL);
			cfread(fnt->pixel_data, fnt->pixel_data_size, 1, fp);
		}
		else {
			fnt->pixel_data = NULL;
		}
		cfclose(fp);

		// Create a bitmap for hardware cards.
		// JAS:  Try to squeeze this into the smallest square power of two texture.
		// This should probably be done at font generation time, not here.
		int w, h;
		if (fnt->pixel_data_size * 4 < 64 * 64) {
			w = h = 64;
		}
		else if (fnt->pixel_data_size * 4 < 128 * 128) {
			w = h = 128;
		}
		else if (fnt->pixel_data_size * 4 < 256 * 256) {
			w = h = 256;
		}
		else if (fnt->pixel_data_size * 4 < 512 * 512) {
			w = h = 512;
		}
		else {
			w = h = 1024;
		}

		fnt->bm_w = w;
		fnt->bm_h = h;
		fnt->bm_data = (ubyte *)vm_malloc(fnt->bm_w*fnt->bm_h);
		fnt->bm_u = (int *)vm_malloc(sizeof(int)*fnt->num_chars);
		fnt->bm_v = (int *)vm_malloc(sizeof(int)*fnt->num_chars);

		memset(fnt->bm_data, 0, fnt->bm_w * fnt->bm_h);

		int i, x, y;
		x = y = 0;
		for (i = 0; i<fnt->num_chars; i++)	{
			ubyte * ubp;
			int x1, y1;
			ubp = &fnt->pixel_data[fnt->char_data[i].offset];
			if (x + fnt->char_data[i].byte_width >= fnt->bm_w)	{
				x = 0;
				y += fnt->h;
				if (y + fnt->h > fnt->bm_h) {
					Error(LOCATION, "Font too big!\n");
				}
			}
			fnt->bm_u[i] = x;
			fnt->bm_v[i] = y;

			for (y1 = 0; y1<fnt->h; y1++)	{
				for (x1 = 0; x1<fnt->char_data[i].byte_width; x1++)	{
					uint c = *ubp++;
					if (c > 14) c = 14;
					fnt->bm_data[(x + x1) + (y + y1)*fnt->bm_w] = (unsigned char)(c);
				}
			}
			x += fnt->char_data[i].byte_width;
		}

		fnt->bitmap_id = bm_create(8, fnt->bm_w, fnt->bm_h, fnt->bm_data, BMP_AABITMAP);

		vfntFontData[typefaceString] = fnt;

		return fnt;
	}

	VFNTFont *FontManager::loadVFNTFont(const SCP_string& name)
	{
		font* font = FontManager::loadFontOld(const_cast<char*>(name.c_str()));

		if (font == NULL)
		{
			return NULL;
		}
		else
		{
			VFNTFont* vfnt = new VFNTFont(font);

			fonts.push_back(vfnt);

			return vfnt;
		}
	}

	FTGLFont *FontManager::loadFTGLFont(const SCP_string& fileName, float fontSize, FTGLFontType type)
	{
		TrueTypeFontData data;

		if (allocatedData.find(fileName) != allocatedData.end())
		{
			data = allocatedData.find(fileName)->second;
		}
		else
		{
			CFILE *fontFile = cfopen(const_cast<char*>(fileName.c_str()), "rb", CFILE_NORMAL, CF_TYPE_ANY);

			if (fontFile == NULL)
			{
				mprintf(("Couldn't open font file \"%s\"", fileName.c_str()));
				return NULL;
			}
			ubyte *fontData = NULL;
			size_t size = cfilelength(fontFile);

			fontData = new ubyte[size];

			if (!fontData)
			{
				mprintf(("Couldn't allocate %d bytes for reading font file \"%s\"!", size, fileName.c_str()));
				return NULL;
			}

			if (!cfread(fontData, (int)size, 1, fontFile))
			{
				mprintf(("Error while reading font data from \"%s\"", fileName.c_str()));
				delete[] fontData;
				return NULL;
			}

			cfclose(fontFile);

			data.size = size;
			data.data = fontData;

			allocatedData[fileName] = data;
		}

		FTFont *fnt = NULL;

		switch (type)
		{
		case OUTLINE:
			fnt = new FTOutlineFont(data.data, data.size);
			break;
		case TEXTURE:
			fnt = new FTTextureFont(data.data, data.size);
			break;
		default:
			Error(LOCATION, "Invalid FTGL font type passed to loadFTGLFont!");
			fnt = new FTTextureFont(data.data, data.size);
			break;
		}

		if (fnt == NULL)
		{
			mprintf(("Couldn't allocated memory for font object for file \"%s\"", fileName.c_str()));
			return NULL;
		}

		if (fnt->Error())
		{
			mprintf(("Font loading of font \"%s\" ended with errors! Error code is %d.", fileName.c_str(), fnt->Error()));
			delete fnt;
			return NULL;
		}

		if (!fnt->FaceSize(fl2i(fontSize)))
		{
			mprintf(("Couldn't set face size of font \"%s\" to %d!", fileName.c_str(), fontSize));
			delete fnt;
			return NULL;
		}

		fnt->UseDisplayList(true);

		FTGLFont *fsFont = new FTGLFont(fnt, type);

		fonts.push_back(fsFont);

		return fsFont;
	}
	void FontManager::init()
	{
	}

	void FontManager::close()
	{
		for (SCP_vector<FSFont*>::iterator iter = fonts.begin(); iter != fonts.end(); iter++)
		{
			delete (*iter);
		}

		for (SCP_map<SCP_string, TrueTypeFontData>::iterator iter = allocatedData.begin(); iter != allocatedData.end(); iter++)
		{
			delete[] iter->second.data;
		}

		for (SCP_map<SCP_string, font*>::iterator iter = vfntFontData.begin(); iter != vfntFontData.end(); iter++)
		{
			delete iter->second;
		}

		allocatedData.clear();
		vfntFontData.clear();
		fonts.clear();

		currentFont = NULL;
	}

	VFNTFont::VFNTFont(font *fnt) : FSFont()
	{
		Assertion(fnt != NULL, "Invalid font passed to constructor of VFNTFont!");

		this->fontPtr = fnt;

		setName(SCP_string(fnt->filename));
	}

	VFNTFont::~VFNTFont()
	{
	}

	FontType VFNTFont::getType() const
	{
		return VFNT_FONT;
	}

	int VFNTFont::getTextHeight() const
	{
		return fontPtr->h;
	}

	font *VFNTFont::getFontData()
	{
		return this->fontPtr;
	}

	extern int get_char_width_old(font* fnt, ubyte c1, ubyte c2, int *width, int* spacing);
	void VFNTFont::getStringSize(const char *text, int textLen, int *w1, int *h1) const
	{
		int longest_width;
		int width, spacing;
		int w, h;

		w = 0;
		h = 0;
		longest_width = 0;

		bool checkLength = textLen >= 0;

		if (text != NULL) {
			h += this->getHeight();
			while (*text)
			{
				// Process one or more 
				while (*text == '\n')
				{
					text++;

					if (checkLength)
					{
						if (textLen < 0)
							break;

						textLen--;
					}

					if (*text)
					{
						h += this->getHeight();
					}

					w = 0;
				}

				if (*text == 0)
				{
					break;
				}

				get_char_width_old(fontPtr, text[0], text[1], &width, &spacing);
				w += spacing;
				if (w > longest_width)
					longest_width = w;

				text++;

				if (checkLength)
				{
					if (textLen < 0)
						break;

					textLen--;
				}
			}
		}

		if (h1)
			*h1 = h;

		if (w1)
			*w1 = longest_width;
	}

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
					if (*s >= Lcl_special_chars || *s < 0)
					{
						specialChar = true;

						int width;
						int spacing;
						get_char_width_old(this->specialCharacterData, *s, '\0', &width, &spacing);

						lineWidth += i2fl(spacing);
					}
					break;
				}
			}

			if (!specialChar)
			{
				lineWidth += ftFont->Advance(s, tokenLength);
			}

			w = MAX(w, lineWidth);

			specialChar = false;

			s = s + tokenLength;

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

		if (*string >= Lcl_special_chars || *string < 0)
			return 1;

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

		for (size_t i = 0; i < length; i++)
		{
			if (string[i] >= Lcl_special_chars || string[i] < 0)
			{
				// Special character needs to be handled seperately
				return i;
			}
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

	FSFont::FSFont() : offsetBottom(0), offsetTop(0), name(SCP_string("<Invalid>"))
	{
	}

	FSFont::~FSFont()
	{
	}

	void FSFont::setBottomOffset(int offset)
	{
		Assertion(offset >= 0, "Bottom offset for font %s has to be larger than zero but it is %d", this->getName().c_str(), offset);

		this->offsetBottom = offset;
	}

	void FSFont::setTopOffset(int offset)
	{
		Assertion(offset >= 0, "Top offset for font %s has to be larger than zero but it is %d", this->getName().c_str(), offset);

		this->offsetTop = offset;
	}

	void FSFont::setName(const SCP_string& name)
	{
		this->name = name;
	}

	int FSFont::getBottomOffset() const
	{
		return this->offsetBottom;
	}

	int FSFont::getTopOffset() const
	{
		return this->offsetTop;
	}

	int FSFont::getHeight() const
	{
		return this->getTextHeight() + this->offsetTop + this->offsetBottom;
	}

	const SCP_string& FSFont::getName() const
	{
		return this->name;
	}

	font::font() : kern_data(NULL),
		char_data(NULL),
		pixel_data(NULL),
		bm_data(NULL),
		bm_u(NULL),
		bm_v(NULL)
	{
	}

	font::~font()
	{
		if (this->kern_data) {
			vm_free(this->kern_data);
			this->kern_data = NULL;
		}

		if (this->char_data) {
			vm_free(this->char_data);
			this->char_data = NULL;
		}

		if (this->pixel_data) {
			vm_free(this->pixel_data);
			this->pixel_data = NULL;
		}

		if (this->bm_data) {
			vm_free(this->bm_data);
			this->bm_data = NULL;
		}

		if (this->bm_u) {
			vm_free(this->bm_u);
			this->bm_u = NULL;
		}

		if (this->bm_v) {
			vm_free(this->bm_v);
			this->bm_v = NULL;
		}
	}

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
				int fontNum = FontManager::getFontIndex(input);

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

	

	/**
	* @brief	Gets the width of an character.
	*
	* Returns the width of the specified charachter also taking account of kerning.
	*
	* @param fnt				The font data
	* @param c1				The character that should be checked.
	* @param c2				The character which follows this character. Used to compute the kerning
	* @param [out]	width   	If non-null, the width.
	* @param [out]	spaceing	If non-null, the spaceing.
	*
	* @return	The character width.
	*/
	int get_char_width_old(fo::font* fnt, ubyte c1, ubyte c2, int *width, int* spacing)
	{
		int i, letter;

		letter = c1 - fnt->first_ascii;

		//not in font, draw as space
		if (letter < 0 || letter >= fnt->num_chars)
		{
			*width = 0;
			*spacing = fnt->w;
			return -1;
		}

		*width = fnt->char_data[letter].byte_width;
		*spacing = fnt->char_data[letter].spacing;

		i = fnt->char_data[letter].kerning_entry;
		if (i > -1)
		{
			if (!(c2 == 0 || c2 == '\n'))
			{
				int letter2;

				letter2 = c2 - fnt->first_ascii;

				if ((letter2 >= 0) && (letter2 < fnt->num_chars))
				{
					font_kernpair *k = &fnt->kern_data[i];
					while ((k->c1 == (char)letter) && (k->c2 < (char)letter2) && (i < fnt->num_kern_pairs))
					{
						i++;
						k++;
					}

					if (k->c2 == (char)letter2)
					{
						*spacing += k->offset;
					}
				}
			}
		}

		return letter;
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

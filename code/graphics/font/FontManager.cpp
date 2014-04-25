
#include "graphics/font/FontManager.h"
#include "graphics/font/font_internal.h"

#include "bmpman/bmpman.h"
#include "cfile/cfile.h"

#include <FTGL/ftgl.h>

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
}

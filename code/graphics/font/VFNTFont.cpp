
#include "graphics/font/VFNTFont.h"
#include "graphics/font/font_internal.h"
#include "graphics/font/enums.h"

#include "localization/localize.h"

namespace
{
	ubyte codepoint_to_old(uint32_t point)
	{
		switch (point)
		{
		case 169: // Copyright symbol
			return static_cast<ubyte>(Lcl_special_chars + 4);
		case 8734: // Infinity symbol
			return static_cast<ubyte>(Lcl_special_chars);
		default:
			// No special treatment, just truncate
			return static_cast<ubyte>(point);
		}
	}
}

namespace font
{
	namespace fo = font;

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

	extern int get_char_width_old(fo::font* fnt, uint32_t c1, uint32_t c2, int *width, int* spacing);
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

	/**
	* @brief	Gets the width of an character.
	*
	* Returns the width of the specified charachter also taking account of kerning.
	*
	* @param fnt				The font data
	* @param code1				The character that should be checked.
	* @param code2				The character which follows this character. Used to compute the kerning
	* @param [out]	width   	If non-null, the width.
	* @param [out]	spaceing	If non-null, the spaceing.
	*
	* @return	The character width.
	*/
	int get_char_width_old(fo::font* fnt, uint32_t code1, uint32_t code2, int *width, int* spacing)
	{
		int i, letter;

		ubyte c1 = codepoint_to_old(code1);
		ubyte c2 = codepoint_to_old(code2);

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

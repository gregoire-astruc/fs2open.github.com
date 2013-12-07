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

#include <FTGL/ftgl.h>


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
    PIXMAP,		//!< A font which gets painted directly onto the viewport
    POLYGON,	//!< A font which consists of polygons
    OUTLINE,	//!< A font where only the outlines of the characters are visible
    TEXTURE,	//!< A font where each character gets saved as a texture. Gives the best visual results but changing size is expensive
};

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

    /**
     * @brief	Sets a new size for this font. This operation will fail for font types of type FontType::VFNT_FONT.
     *
     * @date	23.11.2011
     *
     * @param	newSize	Size of the new.
     *
     * @return	true if it succeeds, false if it fails.
     */
    virtual bool setSize(int newSize) = 0;
};

struct font;

/**
 * @brief	A VFNT font
 *
 * Class that contains a font of type FontType::VFNT_FONT
 * Name of the class is derived from the first four bytes of a font file which are "VFNT"
 */
class VFNTFont : public FSFont
{
private:
    font *fontPtr;  //!< The font pointer

public:

    /**
     * @brief	Constructor that initializes the object with a primary font pointer
     *
     * @param [in]	fnt	A pointer to the font data. Has to be non-null
     */
    VFNTFont(font *fnt);

    /**
     * @brief	Destroys the allocated font pointer
     */
    ~VFNTFont();

    /**
     * @brief	Gets the font data struct.
     *
     * @return	the font data.
     */
    font *getFontData();

    /**
     * @brief	Gets the type. This will always return FontType::VFNT_FONT
     *
     * @return	The type.
     */
    virtual FontType getType() const;

    /**
     * @brief	Gets the height of this font
     *
     * @see FSFont::getHeight()
     *
     * @return	The height.
     */
    virtual int getTextHeight() const;

    /**
     * @brief	Gets the size of the specified string in pixels.
     *
     * @param [in]	text  	the text which should be checked.
     * @param	textLen		  	Length of the text.
     * @param [out]	width 	If non-null, the width.
     * @param [out]	height	If non-null, the height.
     */
    virtual void getStringSize(const char *text, int textLen, int *width, int *height) const;

    /**
     * @brief	Sets the size of this font.
     *
     * Sets the size of this font but as VFNT fonts are raster fonts this will have no effect.
     *
     * @param	newSize	The new size.
     *
     * @return	always false, VFNT fonts are not scalable
     */
    virtual bool setSize(int newSize);
};

/**
 * @brief A TrueType font
 *
 * Class that contains a TrueType font in form of a FTGL FTFont object
 */
class FTGLFont : public FSFont
{
private:
    FTFont *ftglFont;		//<! The font pointer of the actual FTGL font
    float yOffset;			//<! An offset to be applied to the y coordinate to compensate for mirroring the text
    FTGLFontType fontType;	//<! The type of font this is
    float lineWidth;		//<! The width for the lines when the type of FTGLFontType::Outline
    float tabWidth;			//<! The width of one tab (\t) character

    const char* separators;

	font* specialCharacterData; //<! Data used for rendering special characters
	SCP_string specialFontName; //<! Name of the font used for special characters

public:
    /**
     * @brief Constructor
     *
     * Constructs this object with the specified FTGl font and type
     *
     * @param ftglFont The FTGL font pointer
     * @param type The type this font has
     */
    FTGLFont(FTFont *ftglFont, FTGLFontType type);

    /**
     * @brief Deallocates the FTGL font pointer
     */
    ~FTGLFont();

    // Getters

    /**
     * @brief Gets the FTGL font pointer
     * @return The font pointer
     */
    FTFont *getFontData();

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

    int getTokenLength(const char *string, int length = -1) const;

	font* getSpecialCharacterFont() const
	{
		return specialCharacterData;
	}

	const SCP_string& getSpecialFontName() const
	{
		return specialFontName;
	}

    //Setters

    /**
     * @brief Sets line width
     *
     * Sets the line width of this font, only effective when type if FTGLFontType::Outline
     * @param width The new line width
     */
    void setLineWidth(float width);

    /**
     * @brief Sets a new size
     *
     * Sets the size of this font to a new value.
     * @warning This function may cause performance problems depending on the font type
     * @param newSize The new size value
     * @return True if size was set, false otherwise
     */
    virtual bool setSize(int newSize);

    /**
     * @brief Sets the width of a tab character
     * @param width New width of a tab character
     */
	void setTabWidth(float width);

	void setSpecialCharacterFont(const SCP_string& fontName, font* fontData)
	{
		specialCharacterData = fontData;
		specialFontName = fontName;
	}
};

/**
 * @struct	TrueTypeFontData
 *
 * @brief	True type font data to save data for already read fonts.
 *
 * Used to store the data of a true type font which can be used by multiple FTFont objects which are all using the same data.
 *
 * @author	m!m
 * @date	24.11.2011
 */

struct TrueTypeFontData
{
    int size;		//<! Size of allocated memory
    ubyte *data;	//<! Allocated font data
};

typedef struct font_char {
    int					spacing;
    int					byte_width;
    int					offset;
    short				kerning_entry;
    short				user_data;
} font_char;

typedef struct font_kernpair {
    char				c1,c2;
    signed char			offset;
} font_kernpair;

typedef struct font {
    char				filename[MAX_FILENAME_LEN];
    int					id;							//!< Should be 'VFNT'
    int					version;					//!< font version
    int					num_chars;
    int					first_ascii;
    int					w;
    int					h;
    int					num_kern_pairs;
    int					kern_data_size;
    int					char_data_size;
    int					pixel_data_size;
    font_kernpair		*kern_data;
    font_char			*char_data;
    ubyte				*pixel_data;

    // Data for 3d cards
    int				bitmap_id;			//!< A bitmap representing the font data
    int				bm_w, bm_h;			//!< Bitmap width and height
    ubyte			*bm_data;			//!< The actual font data
    int				*bm_u;				//!< U offset of each character
    int				*bm_v;				//!< V offset of each character

    font();
    ~font();
} font;

#define FONT1				0	//<! The first loaded font. Used to be hardcoded to reference font01.vf in retail
#define FONT2				1	//<! The second loaded font. Used to be hardcoded to reference font02.vf in retail
#define FONT3				2	//<! The third loaded font. Used to be hardcoded to reference font03.vf in retail

/**
* @brief Manages the fonts used by FreeSpace
*
* This class is responsible for the creation, management and disposal of fonts used
* by the FreeSpace engine. It contains static functions to load a specific font type
* and functions to set or get the current font and can be used to retrieve a font by
* name or index.
*
* @warning Don't @c delete a font which was retrieved from this class as they are managed internally
*/
class FontManager
{
public:
	/**
	* @brief Returns a pointer to the font with the specified name
	*
	* Searches the internal list of fonts for a font with the name @c name and returns it to the caller.
	* If the specified name could not be found then @c NULL is returned instead.
	*
	* @param name The name that should be searched for
	* @return The font pointer or @c NULL when font could not be found.
	*/
	static FSFont *getFont(const SCP_string& name);

	/**
	* @brief Returns a pointer to the font at the specified index
	*
	* Returns the font pointer which is located as the specified index or @c NULL when the specified index is invald
	*
	* @param index The index that should be returns
	* @return Font pointer
	*/
	inline static FSFont *getFont(int index)
	{
		Assertion(index >= 0 && index < (int)fonts.size(), "Invalid font index %d given!", index);

		return fonts[index];
	}

	/**
	* @brief Returns the index of the currently used font
	*
	* Returns the index the currently active font pointer has.
	*
	* @return The current font index. 0 by default, -1 when FontManager isn't initialized yet
	*/
	static int getCurrentFontIndex();

	/**
	* @brief Returns a pointer to the current font
	*
	* Returns a pointer of the currently active font.
	*
	* @return Pointer of currently active font or @c NULL when no font is currently active
	*/
	static FSFont *getCurrentFont();


	/**
	* @brief Returns the index of the font with the specified @c name.
	*
	* @param name The name which should be searched
	* @return The index or -1 when font could not be found
	*/
	static int getFontIndex(const SCP_string& name);

	/**
	* @brief Returns the index of the specified font pointer
	*
	* Searches through the internal font vector and returns the index for which @verbatim (*fontIterator) == font @endverbatim
	* is true.
	*
	* @param font The font which should be searched
	* @return The index of the font or -1 when font could not be found
	*/
	static int getFontIndex(FSFont* font);

	/**
	* @brief Returns the number of fonts currently saved in the manager
	* @return The number of fonts
	*/
	static int numberOfFonts();

	/**
	* @brief Specifies if the font system is ready to be used for rendering text
	* @return @c true if ready (there is a current font), @c false if not
	*/
	static bool isReady();

	/**
	* @brief Checks if the specified number is a valid font ID
	* @return @c true if the number can be used with #getFont(int), @c false otherwise
	*/
	static bool isFontNumberValid(int fontNumber);

	/**
	* @brief Sets the currently active font
	* @param font The font which should be used, may not be @c NULL
	*
	* @warning The integrity of the specified pointer is not checked
	*/
	static void setCurrentFont(FSFont *font);

	/**
	* @brief Loads a TrueType font
	*
	* Loads a TrueType font with the specified @c fileName and initializes it with the specified
	* @c size and @c type. The size can be changed later where the type is not changeable.
	*
	* @param fileName The name of the font file which should be loaded
	* @param fontSize The initial size of the font
	* @param type The type of the font
	* @return A FTGLFont pointer or @c NULL when font could not be loaded
	*/
	static FTGLFont *loadFTGLFont(const SCP_string& fileName, int fontSize = 16, FTGLFontType type = TEXTURE);

	/**
	* @brief Loads an old VFNT font
	*
	* Loads the VFNT font with the specified @c fileName and returns it in form of a VFNTFont pointer
	*
	* @param fileName The name of the font file
	* @return The font pointer or @c null on error
	*/
	static VFNTFont *loadVFNTFont(const SCP_string& fileName);

	/**
	* @brief Loads old volition font data
	*/
	static font *loadFontOld(char *name);

	/**
	*	@brief Initializes the font system
	*/
	static void init();

	/**
	* @brief Disposes the fonts saved in the class
	*
	* Cleans up the memory used by the font system and sets the current font to @c NULL
	*/
	static void close();
private:
	FontManager() {};
	FontManager(const FontManager&) {};

	~FontManager();

	static SCP_map<SCP_string, TrueTypeFontData> allocatedData;
	static SCP_map<SCP_string, font*> vfntFontData;
	static SCP_vector<FSFont*> fonts;

	static FSFont* currentFont;
};

/**
 * @brief Parses the first font name
 *
 * Parses the first font name in the font table and stuffs it into @c firstFont
 *
 * @param firstFont The SCP_string which should contain the name
 */
void gr_stuff_first_font(SCP_string &firstFont);

/**
 * @brief Prints the current time
 *
 * Draws the timestamp of the current #Missiontime in the format @c h:mm:ss at the specified coordinates
 *
 * @param x The x position where the timestamp should be draw
 * @param y The y position where the timestamp should be draw
 * @param timestamp The timespamp in milliseconds to be printed
 */
void gr_print_timestamp(int x, int y, int timestamp);

/**
 * Crops a string if required to force it to not exceed max_width pixels when printed.
 * Does this by dropping characters at the end of the string and adding '...' to the end.
 *
 * @param str		string to crop.  Modifies this string directly
 * @param max_str	max characters allowed in str
 * @param max_width number of pixels to limit string to (less than or equal to).
 * @return			Returns same pointer passed in for str.
 */
char *gr_force_fit_string(char *str, int max_str, int max_width);

/**
 * @brief Inites the font system
 *
 * Initializes the font system by setting up the FontManager, parse the font table(s) and
 * set the current font id to 0.
 */
void gr_font_init();

/**
 * @brief Closes the Font system
 *
 * Deallocates all allocated memory for the fonts and the respective font data.
 */
void gr_font_close();

/**
 * For a string @c s returns a x-coordinate which will center the first line of the string
 * on the screen
 *
 * @param s
 * @return
 */
int get_centered_x(const char *s);

/**
 * Sets the current font to the specified FSFont pointer
 * @param font The font that should be set. May not be NULL
 */
inline void gr_set_font(FSFont* font)
{
	FontManager::setCurrentFont(font);
}

/**
 * Retrieves the font which is located at index @c font_num and sets this font
 * as the current font
 * @param font_num The new font number, may not be an illegal font number
 */
inline void gr_set_font(int fontnum)
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
bool parse_font(int &destination, char *tag = "Font:");

/**
 * @brief The currently active font index
 * @return The font index or -1 when the FontManager hasnt't been initialized yet
 */
inline int gr_get_current_fontnum()
{
	return FontManager::getCurrentFontIndex();
}

/**
 * @brief The current font object
 *
 * @return The current font object or NULL when not yet ready
 */
FSFont *gr_get_current_font();

/**
 * @brief Retrieves a font by index
 *
 * Gets a font by index into the internal font vector
 *
 * @param fontNum The index which should be returned
 * @return A font pointer or NULL of the index is not valid
 */
inline FSFont *gr_get_font(int fontNum)
{
	return FontManager::getFont(fontNum);
}

/**
 * @brief Retrieves a font by name
 *
 * @param name The name which should be searched
 * @return The font pointer or NULL if no font with that name could be found
 */
FSFont *gr_get_font(SCP_string name);

#endif

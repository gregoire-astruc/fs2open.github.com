# top-level files
set (file_root
	FTBuffer.cpp
	FTCharmap.cpp
	FTContour.cpp
	FTFace.cpp
	FTGlyphContainer.cpp
	FTLibrary.cpp
	FTPoint.cpp
	FTSize.cpp
	FTVectoriser.cpp
	FTCharmap.h
	FTCharToGlyphIndexMap.h
	FTContour.h
	FTFace.h
	FTGlyphContainer.h
	FTInternals.h
	FTLibrary.h
	FTList.h
	FTSize.h
	FTUnicode.h
	FTVector.h
	FTVectoriser.h
	config.h
)

set(file_root_ftfont
	FTFont/FTBitmapFont.cpp
	FTFont/FTBufferFont.cpp
	FTFont/FTExtrudeFont.cpp
	FTFont/FTFont.cpp
	FTFont/FTFontGlue.cpp
	FTFont/FTOutlineFont.cpp
	FTFont/FTPixmapFont.cpp
	FTFont/FTPolygonFont.cpp
	FTFont/FTTextureFont.cpp
	FTFont/FTBitmapFontImpl.h
	FTFont/FTBufferFontImpl.h
	FTFont/FTExtrudeFontImpl.h
	FTFont/FTFontImpl.h
	FTFont/FTOutlineFontImpl.h
	FTFont/FTPixmapFontImpl.h
	FTFont/FTPolygonFontImpl.h
	FTFont/FTTextureFontImpl.h
)

set(file_root_ftgl
	FTGL/FTBBox.h
	FTGL/FTBitmapGlyph.h
	FTGL/FTBuffer.h
	FTGL/FTBufferFont.h
	FTGL/FTBufferGlyph.h
	FTGL/FTExtrdGlyph.h
	FTGL/FTFont.h
	FTGL/ftgl.h
	FTGL/FTGLBitmapFont.h
	FTGL/FTGLExtrdFont.h
	FTGL/FTGLOutlineFont.h
	FTGL/FTGLPixmapFont.h
	FTGL/FTGLPolygonFont.h
	FTGL/FTGLTextureFont.h
	FTGL/FTGlyph.h
	FTGL/FTLayout.h
	FTGL/FTOutlineGlyph.h
	FTGL/FTPixmapGlyph.h
	FTGL/FTPoint.h
	FTGL/FTPolyGlyph.h
	FTGL/FTSimpleLayout.h
	FTGL/FTTextureGlyph.h
)

set(file_root_ftglyph
	FTGlyph/FTBitmapGlyph.cpp
	FTGlyph/FTBufferGlyph.cpp
	FTGlyph/FTExtrudeGlyph.cpp
	FTGlyph/FTGlyph.cpp
	FTGlyph/FTGlyphGlue.cpp
	FTGlyph/FTOutlineGlyph.cpp
	FTGlyph/FTPixmapGlyph.cpp
	FTGlyph/FTPolygonGlyph.cpp
	FTGlyph/FTTextureGlyph.cpp
	FTGlyph/FTBitmapGlyphImpl.h
	FTGlyph/FTBufferGlyphImpl.h
	FTGlyph/FTExtrudeGlyphImpl.h
	FTGlyph/FTGlyphImpl.h
	FTGlyph/FTOutlineGlyphImpl.h
	FTGlyph/FTPixmapGlyphImpl.h
	FTGlyph/FTPolygonGlyphImpl.h
	FTGlyph/FTTextureGlyphImpl.h
)

set(file_root_ftlayout
	FTLayout/FTLayout.cpp
	FTLayout/FTLayoutGlue.cpp
	FTLayout/FTLayoutImpl.h
	FTLayout/FTSimpleLayout.cpp
	FTLayout/FTSimpleLayoutImpl.h
)

# the source groups
source_group(""                                  FILES ${file_root})
source_group("FTFont"                            FILES ${file_root_ftfont})
source_group("FTGL"                              FILES ${file_root_ftgl})
source_group("FTGlyph"                           FILES ${file_root_ftglyph})
source_group("FTLayout"                          FILES ${file_root_ftlayout})

# append all files to the file_root
set (file_root
	${file_root}
	${file_root_ftfont}
	${file_root_ftgl}
	${file_root_ftglyph}
	${file_root_autopilot}
	${file_root_ftlayout}
)

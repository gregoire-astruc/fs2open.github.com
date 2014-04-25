
#include "graphics/font/FSFont.h"

namespace font
{
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
}

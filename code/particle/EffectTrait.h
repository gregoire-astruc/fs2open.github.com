
#ifndef _EFFECT_TRAIT_H
#define _EFFECT_TRAIT_H

#include "particle/particle.h"

class ParticleSource;

class EffectTrait
{
protected:
	SCP_string name;

public:
	EffectTrait(const SCP_string& name) : name(name) {}
	virtual ~EffectTrait() {}

	const SCP_string& getName() const { return name; }

	virtual void pageIn() {}

	virtual void associate(ParticleEffect& effect) {}

	virtual void update(const ParticleSource& source, float frametime) = 0;

	virtual bool isActive(const ParticleSource& source) const { return false; }

	virtual void initializeTrait(ParticleSource& source) {}

	virtual void doParse() {}
};

#endif // _EFFECT_TRAIT_H

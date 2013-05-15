
#ifndef _EFFECT_TRAIT_H
#define _EFFECT_TRAIT_H

#include "globalincs/pstypes.h"
#include "object/object.h"

#include <boost/smart_ptr.hpp>

class ParticleSource;

class EffectTrait
{
protected:
	SCP_string name;

public:
	EffectTrait(const SCP_string& name) : name(name) {}
	virtual ~EffectTrait() {}

	const SCP_string& getName() const { return name; }

	void associate(ParticleEffect& effect) {}

	virtual void update(const ParticleSource& source, float frametime) = 0;

	virtual bool isActive(const ParticleSource& source) const { return false; }

	virtual void initializeTrait(ParticleSource& source) {}

	virtual void doParse() {}
};

struct GenericTraitConfiguration
{
	int beginDelay;
	int effectDuration;
	int trailEffectID;
	int effectAnimation;

	GenericTraitConfiguration() : beginDelay(0), effectDuration(0), trailEffectID(-1), effectAnimation(-1)
	{}
};

class GenericEffectTrait : public EffectTrait
{
private:
	int genericTraitID;

protected:
	GenericTraitConfiguration config;

	static float randomRange(float upper, float lower);

	bool spawningParticles(const ParticleSource& source);

	vec3d getEffectPosition(const ParticleSource& source, bool* success = NULL) const;

	vec3d getEffectDirection(const ParticleSource& source, bool* success = NULL) const;

	boost::weak_ptr<particle> createParticle(vec3d* position, vec3d* vel, int animation, float lifetime, float radius, object* obj = NULL);
public:
	GenericEffectTrait(const SCP_string& name);

	void associate(ParticleEffect& effect);

	virtual void update(const ParticleSource& source, float frametime);

	virtual void initializeTrait(ParticleSource& source);

	virtual bool isActive(const ParticleSource& source) const;

	virtual void doParse();
};

#endif // _EFFECT_TRAIT_H

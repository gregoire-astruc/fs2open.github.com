
#ifndef _GENERIC_EFFECT_TRAIT_H
#define _GENERIC_EFFECT_TRAIT_H

#include "globalincs/pstypes.h"
#include "object/object.h"
#include "particle/EffectTrait.h"
#include "particle/particle.h"
#include "particle/TraitUtil.h"

#include <boost/smart_ptr.hpp>

struct GenericTraitConfiguration
{
	TraitUtil::ValueRange<int> beginDelay;
	TraitUtil::ValueRange<int> effectDuration;
	int trailEffectID;
	int effectAnimation;
	int effectFrameNumber;

	TraitUtil::ValueRange<float> particleLifetime;
	TraitUtil::ValueRange<float> particleSize;
	TraitUtil::ValueRange<int> particleNumber;
	TraitUtil::ValueRange<float> particleVelocity;

	bool addObjectVelocity;
	bool reflectDirection;

	GenericTraitConfiguration() : trailEffectID(-1), effectAnimation(-1), addObjectVelocity(true),
		particleLifetime(TraitUtil::ValueRange<float>(-1.0f)), particleNumber(TraitUtil::ValueRange<int>(1)),
		beginDelay(TraitUtil::ValueRange<int>(0)), particleVelocity(TraitUtil::ValueRange<float>(0.0f)),
		reflectDirection(false)
	{}
};

class particle;

class GenericEffectTrait : public EffectTrait
{
private:
	int genericTraitID;

protected:
	GenericTraitConfiguration config;

	bool spawningParticles(const ParticleSource& source);

	boost::weak_ptr<particle> createParticle(vec3d* position, vec3d* vel, int animation, float lifetime, float radius, object* obj = NULL);
public:
	GenericEffectTrait(const SCP_string& name);

	void associate(ParticleEffect& effect);

	virtual void pageIn();

	virtual void update(const ParticleSource& source, float frametime);

	virtual void initializeTrait(ParticleSource& source);

	virtual bool isActive(const ParticleSource& source) const;

	virtual void doParse();
};

#endif // _GENERIC_EFFECT_TRAIT_H

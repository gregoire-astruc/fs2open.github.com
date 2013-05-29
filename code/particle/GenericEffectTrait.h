
#ifndef _GENERIC_EFFECT_TRAIT_H
#define _GENERIC_EFFECT_TRAIT_H

#include "globalincs/pstypes.h"
#include "object/object.h"
#include "particle/EffectTrait.h"
#include "particle/particle.h"

#include <boost/smart_ptr.hpp>

template<class ValueType>
struct ValueRange
{
	ValueType upper;
	ValueType lower;

	ValueRange() {}

	ValueRange(const ValueType& value) : upper(value), lower(value) {}

	ValueRange(const ValueType& value1, const ValueType& value2) : upper(value1), lower(value2) {}
};

struct GenericTraitConfiguration
{
	ValueRange<int> beginDelay;
	ValueRange<int> effectDuration;
	int trailEffectID;
	int effectAnimation;

	ValueRange<float> particleLifetime;
	ValueRange<float> particleSize;
	ValueRange<int> particleNumber;
	ValueRange<float> particleVelocity;

	bool addObjectVelocity;

	GenericTraitConfiguration() : trailEffectID(-1), effectAnimation(-1), addObjectVelocity(true),
		particleLifetime(ValueRange<float>(-1.0f)), particleNumber(ValueRange<int>(1)), beginDelay(ValueRange<int>(0)),
		particleVelocity(ValueRange<float>(0.0f))
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

	vec3d getEffectPosition(const ParticleSource& source, bool* success = NULL) const;

	vec3d getEffectDirection(const ParticleSource& source, bool* success = NULL, bool addVelocity = true) const;

	boost::weak_ptr<particle> createParticle(vec3d* position, vec3d* vel, int animation, float lifetime, float radius, object* obj = NULL);
public:
	GenericEffectTrait(const SCP_string& name);

	void associate(ParticleEffect& effect);

	virtual void update(const ParticleSource& source, float frametime);

	virtual void initializeTrait(ParticleSource& source);

	virtual bool isActive(const ParticleSource& source) const;

	virtual void doParse();
};

namespace TraitUtil
{
	template<class ValueType>
	ValueType randomRange(const ValueType lower, const ValueType upper);

	template<class ValueType>
	inline ValueType randomRange(const ValueRange<ValueType>& range)
	{
		return randomRange(range.lower, range.upper);
	}
	
	template<class ValueType>
	ValueRange<ValueType> parseValueRange(const ValueType lowerBound, const ValueType upperBound);
};

#endif // _GENERIC_EFFECT_TRAIT_H

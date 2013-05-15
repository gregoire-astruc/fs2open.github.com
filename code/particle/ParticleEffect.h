
#ifndef _PARTICLE_EFFECT_H
#define _PARTICLE_EFFECT_H

#include "globalincs/pstypes.h"
#include "particle/ParticleSystem.h"
#include "particle/ParticleSource.h"
#include "particle/EffectTrait.h"

#include <boost/smart_ptr.hpp>

class ParticleSource;
class ParticleSystem;

class ParticleEffect
{
private:
	SCP_vector<boost::shared_ptr<EffectTrait>> effectTraits;

	boost::weak_ptr<ParticleSystem> parentSystem;

	SCP_string name;

	int nextTraitID;

public:
	ParticleEffect(boost::weak_ptr<ParticleSystem> parent, const SCP_string& name);
	~ParticleEffect();

	int requestTraitID();

	void addTrait(boost::shared_ptr<EffectTrait> trait);

	boost::shared_ptr<EffectTrait> getTrait(const SCP_string& name) const;

	const SCP_string& getName() const;

	bool isActive(const ParticleSource& source) const;

	void update(const ParticleSource& source, float frametime);
};

#endif

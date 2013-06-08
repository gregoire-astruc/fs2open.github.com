
#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H

#include "globalincs/pstypes.h"
#include "particle/ParticleEffect.h"
#include "particle/ParticleSource.h"

#include <boost/smart_ptr.hpp>

class ParticleEffect;
class ParticleSource;
class ParticleSystem;

class ParticleSystem
{
private:
	static boost::shared_ptr<ParticleSystem> globalInstance;

	SCP_vector<boost::shared_ptr<ParticleEffect>> particleEffects;
	SCP_vector<ParticleSource> particleSources;

	// Do not allow new instances
	ParticleSystem();
public:
	~ParticleSystem();
	
	void pageIn();

	void update(float frametime);

	void levelClose();

	int addParticleEffect(boost::shared_ptr<ParticleEffect>& effect);

	ParticleSource* createParticleEffect(int effectId);

	inline boost::shared_ptr<ParticleEffect> getEffect(int id) const;

	int getEffectId(const SCP_string& name) const;

	static boost::shared_ptr<ParticleSystem> getInstance();
};

boost::shared_ptr<ParticleEffect> ParticleSystem::getEffect(int id) const
{
	Assertion(id < (int) particleEffects.size(), 
		"Tried to access particle effect %d, got %d definitions.", id, (int) particleEffects.size());

	return this->particleEffects[id];
}

#endif // _PARTICLE_SYSTEM_H

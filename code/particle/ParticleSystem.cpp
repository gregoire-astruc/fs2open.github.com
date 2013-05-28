#include "particle/ParticleSystem.h"

using namespace boost;

shared_ptr<ParticleSystem> ParticleSystem::globalInstance = shared_ptr<ParticleSystem>();

ParticleSystem::ParticleSystem() : lastSignature(0)
{
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::update(float frametime)
{
	for (SCP_vector<ParticleSource>::iterator iter = particleSources.begin();
		iter != particleSources.end(); ++iter)
	{
		if (iter->isActive())
		{
			iter->update(frametime);
		}
		else
		{
			// Remove this entry if it isn't active anymore
			if (iter + 1 == particleSources.end())
			{
				particleSources.pop_back();
				break;
			}
			else
			{
				*iter = particleSources.back();
				particleSources.pop_back();
				continue;
			}
		}
	}
}

void ParticleSystem::levelClose()
{
	particleSources.clear();
}

int ParticleSystem::addParticleEffect(shared_ptr<ParticleEffect>& effect)
{
	Assert(effect);

	size_t id = particleEffects.size();

	particleEffects.push_back(effect);

	return id;
}

ParticleSource* ParticleSystem::createParticleEffect(int effectId)
{
	shared_ptr<ParticleEffect> effect = this->getEffect(effectId);

	weak_ptr<ParticleEffect> ptr(effect);
	ParticleSource source(ptr, ++lastSignature);

	particleSources.push_back(source);

	return &particleSources.back();
}

int ParticleSystem::getEffectId(const SCP_string& name) const
{
	int id = 0;
	for (SCP_vector<shared_ptr<ParticleEffect>>::const_iterator iter = particleEffects.begin();
		iter != particleEffects.end(); ++iter)
	{
		if (!(*iter)->getName().compare(name))
		{
			return id;
		}

		id++;
	}

	return -1;
}

shared_ptr<ParticleSystem> ParticleSystem::getInstance()
{
	if (globalInstance)
	{
		globalInstance = shared_ptr<ParticleSystem>(new ParticleSystem());
	}

	return ParticleSystem::globalInstance;
}
